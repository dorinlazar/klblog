#include "kltime.hpp"
#include <algorithm>
#include <chrono>
#include "kl/textscanner.hpp"
// #include <format>

namespace kl {
static constexpr bool _leap_year(uint32_t year) { // Leap year, for one-based 1-400 interval
  // NOLINTNEXTLINE(readability-magic-numbers)
  return ((year & 0x03) == 0) && year != 100 && year != 200 && year != 300;
}

static constexpr std::array<uint32_t, 12> MSIZES = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static constexpr std::array<uint32_t, 12> MSIZES_LEAP = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static constexpr const std::array<uint32_t, 12>& _month_sizes(int32_t year) { // year in the interval 1-400.
  return _leap_year(year) ? MSIZES_LEAP : MSIZES;
}

static constexpr std::array<int32_t, 400 * 12 + 1> _calculateDeltaMonths() {
  std::array<int32_t, 400 * 12 + 1> res; // NOLINT(readability-magic-numbers)
  int32_t total_days = 0;
  res[0] = 0;
  int32_t index = 1;

  for (int32_t y = 0; y < 400; y++) { // NOLINT(readability-magic-numbers)
    auto& sizes = _month_sizes(y + 1);
    for (int32_t m = 0; m < 12; m++, index++) { // NOLINT(readability-magic-numbers)
      total_days += sizes[m];
      res[index] = total_days;
    }
  }
  return res;
}

static const std::array<int32_t, 400 * 12 + 1> DeltaMonths = _calculateDeltaMonths();

const DateTime DateTime::UnixEpoch(1970, 1, 1);
const DateTime DateTime::MAX = DateTime::fromTicks(TimeLimits::MAX_TICKS);
const DateTime DateTime::MIN = DateTime::fromTicks(TimeLimits::MIN_TICKS);

Date DateTime::date() const {
  auto d = m_ticks / TimeLimits::TICKS_PER_DAY;
  auto fh = std::lldiv(d, TimeLimits::DAYS_IN_400_YEARS);
  auto it = std::lower_bound(DeltaMonths.begin(), DeltaMonths.end(), fh.rem + 1) - 1;
  auto mo =
      std::div(static_cast<uint32_t>(std::distance(DeltaMonths.begin(), it)), 12); // NOLINT(readability-magic-numbers)
  return Date{.year = static_cast<uint32_t>(fh.quot) * 400 + mo.quot + 1,          // NOLINT(readability-magic-numbers)
              .month = static_cast<uint32_t>(mo.rem) + 1,
              .day = 1 + static_cast<uint32_t>(fh.rem) - *it};
}

DateTime DateTime::now() {
  // auto n = std::chrono::utc_clock::now(); TODO(dorin) FIX THIS WHEN THE COMPILER IMPLEMENTS utc_clock
  auto n = std::chrono::system_clock::now();
  int64_t nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(n.time_since_epoch()).count();
  return DateTime(nanos / 1'000'000'000, nanos % 1'000'000'000); // NOLINT(readability-magic-numbers)
}

int64_t DateTime::ticks() const { return m_ticks; }
int32_t DateTime::days() const { return m_ticks / TimeLimits::TICKS_PER_DAY; }
TimeOfDay DateTime::timeOfDay() const {
  int64_t day_ticks = m_ticks % TimeLimits::TICKS_PER_DAY;
  auto sec = lldiv(day_ticks, TimeLimits::TICKS_PER_SECOND);
  auto mn = div(sec.quot, 60); // NOLINT(readability-magic-numbers)
  auto hr = div(mn.quot, 60);  // NOLINT(readability-magic-numbers)
  return {.hour = static_cast<uint32_t>(hr.quot),
          .min = static_cast<uint32_t>(hr.rem),
          .sec = static_cast<uint32_t>(mn.rem),
          .nanos = static_cast<uint32_t>(sec.rem) * 100}; // NOLINT(readability-magic-numbers)
}

DateTime DateTime::fromTicks(int64_t ticks) {
  DateTime dt;
  dt.m_ticks = ticks < TimeLimits::MIN_TICKS   ? TimeLimits::MIN_TICKS
               : ticks > TimeLimits::MAX_TICKS ? TimeLimits::MAX_TICKS
                                               : ticks;
  return dt;
}

DateTime::DateTime(time_t seconds, int32_t nsec) {
  // NOLINTNEXTLINE(readability-magic-numbers)
  m_ticks = DateTime::UnixEpoch.ticks() + seconds * TimeLimits::TICKS_PER_SECOND + nsec / 100;
}

DateTime::DateTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t sec,
                   uint32_t nsec) {
  // NOLINTNEXTLINE(readability-magic-numbers)
  if (year < 1 || year > 9999 || month < 1 || month > 12 || day < 1 || nsec > 999'999'999ULL || sec > 59 ||
      minute > 59 || hour > 23) [[unlikely]] {
    m_ticks = 0;
    return;
  }
  year--;
  month--;
  day--;
  // This isn't constexpr in C++20 :( auto years = std::div(year - 1, 400);
  // ldiv_t years{.quot = year / 400, .rem = year % 400};
  auto years = std::div(year, 400); // NOLINT(readability-magic-numbers)
  auto& monthsizes = _month_sizes(years.rem + 1);
  if (day >= monthsizes[month]) [[unlikely]] {
    m_ticks = 0;
    return;
  }
  auto seconds = sec + minute * 60 + hour * 3600; // NOLINT(readability-magic-numbers)
  auto days = DeltaMonths[years.rem * 12 + month] + day +
              years.quot * TimeLimits::DAYS_IN_400_YEARS; // NOLINT(readability-magic-numbers)
  m_ticks = nsec / 100;                                   // NOLINT(readability-magic-numbers)
  m_ticks += seconds * TimeLimits::TICKS_PER_SECOND;
  m_ticks += days * TimeLimits::TICKS_PER_DAY;
}

const DateTime& DateTime::operator=(const DateTime d) {
  m_ticks = d.m_ticks;
  return *this;
}
TimeSpan DateTime::operator-(const DateTime d) { return {.ticks = m_ticks - d.m_ticks}; }
DateTime DateTime::operator-(TimeSpan ts) { return fromTicks(m_ticks - ts.ticks); }
DateTime DateTime::operator+(TimeSpan ts) { return fromTicks(m_ticks + ts.ticks); }

std::ostream& operator<<(std::ostream& os, kl::DateTime t) {
  auto date = t.date();
  auto time = t.timeOfDay();
  return os << fmt::format("{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0>3}", date.year, date.month, date.day,
                           time.hour, time.min, time.sec, time.nanos / 1'000'000);
}

std::ostream& operator<<(std::ostream& os, TimeSpan t) {
  if (t.ticks < 0) {
    os << "-";
    t.ticks = -t.ticks;
  }
  uint32_t days = t.days();
  uint32_t hours = t.hours();
  uint32_t minutes = t.minutes();
  uint32_t seconds = t.seconds();
  uint32_t millis = t.milliseconds();
  if (days != 0) [[unlikely]] {
    os << days << "d ";
  }

  return os << fmt::format("{:0>2}:{:0>2}:{:0>2}.{:0>3}", hours, minutes, seconds, millis);
}

inline std::tuple<uint32_t, uint32_t, uint32_t> kltime_read_date(TextScanner& sc) {
  uint32_t year = sc.read_digit() * 1000; // NOLINT(readability-magic-numbers)
  year += sc.read_digit() * 100;          // NOLINT(readability-magic-numbers)
  year += sc.read_digit() * 10;           // NOLINT(readability-magic-numbers)
  year += sc.read_digit();

  bool has_splitter = sc.top_char() == '-';

  if (has_splitter) {
    sc.expect('-');
  }

  uint32_t month = sc.read_digit() * 10; // NOLINT(readability-magic-numbers)
  month += sc.read_digit();

  if (has_splitter) {
    sc.expect('-');
  }

  uint32_t day = sc.read_digit() * 10; // NOLINT(readability-magic-numbers)
  day += sc.read_digit();
  return {year, month, day};
}

inline std::tuple<uint32_t, uint32_t, uint32_t, uint64_t> kltime_read_time(TextScanner& sc) {
  uint32_t hh = 0, mm = 0, ss = 0;
  uint64_t ff = 0;

  if (sc.empty()) {
    return {hh, mm, ss, ff};
  }

  if (sc.top_char() == 'T' || sc.top_char() == ' ') [[likely]] {
    sc.read_char();
  } else {
    sc.error("Expected Date-Time split");
  }
  hh += sc.read_digit() * 10; // NOLINT(readability-magic-numbers)
  hh += sc.read_digit();

  if (sc.empty()) {
    return {hh, mm, ss, ff};
  }
  bool has_splitter = sc.top_char() == ':';
  if (has_splitter) {
    sc.expect(':');
  } else if (sc.top_char() == '+' || sc.top_char() == '-' || sc.top_char() == 'Z') {
    return {hh, mm, ss, ff};
  }

  mm += sc.read_digit() * 10; // NOLINT(readability-magic-numbers)
  mm += sc.read_digit();
  if (sc.empty() || sc.top_char() == '+' || sc.top_char() == '-' || sc.top_char() == 'Z') {
    return {hh, mm, ss, ff};
  }
  if (has_splitter) {
    sc.expect(':');
  }

  ss += sc.read_digit() * 10; // NOLINT(readability-magic-numbers)
  ss += sc.read_digit();
  if (sc.empty() || sc.top_char() == '+' || sc.top_char() == '-' || sc.top_char() == 'Z') {
    return {hh, mm, ss, ff};
  }

  sc.expect('.');
  ff += static_cast<uint64_t>(sc.read_digit() * 100); // NOLINT(readability-magic-numbers)
  ff += static_cast<uint64_t>(sc.read_digit() * 10);  // NOLINT(readability-magic-numbers)
  ff += static_cast<uint64_t>(sc.read_digit());
  if (!sc.empty() && sc.top_char() >= '0' && sc.top_char() <= '9') {
    ff *= 1000;                  // NOLINT(readability-magic-numbers)
    ff += sc.read_digit() * 100; // NOLINT(readability-magic-numbers)
    ff += sc.read_digit() * 10;  // NOLINT(readability-magic-numbers)
    ff += sc.read_digit();
    ff *= 1000; // NOLINT(readability-magic-numbers)
  } else {
    ff *= 1'000'000; // NOLINT(readability-magic-numbers)
  }
  return {hh, mm, ss, ff};
}

std::tuple<bool, uint32_t, uint32_t> kltime_read_timezone(TextScanner& sc) {
  uint32_t ts_hours = 0, ts_minutes = 0;
  bool plus = false;
  if (!sc.empty()) {
    if (sc.top_char() == '+' || sc.top_char() == '-') {
      plus = sc.read_char().character == '+';
      ts_hours += sc.read_digit() * 10; // NOLINT(readability-magic-numbers)
      ts_hours += sc.read_digit();
      sc.expect(':');
      ts_minutes += sc.read_digit() * 10; // NOLINT(readability-magic-numbers)
      ts_minutes += sc.read_digit();
    } else {
      sc.expect('Z');
    }
  }
  return {plus, ts_hours, ts_minutes};
}

// Limited date parsing.
// Allowed formats:
// YYYY-?MM-?DD[T ]hh:mm:ss.fff{fff}?
DateTime DateTime::parse(const Text& src) {
  if (src.size() < 8) [[unlikely]] {
    throw InvalidInputData(src, "A valid date has at least 8 characters"_t);
  }
  TextScanner sc(src);
  auto [year, month, day] = kltime_read_date(sc);
  auto [hh, mm, ss, ff] = kltime_read_time(sc);
  auto [plus, ts_hours, ts_minutes] = kltime_read_timezone(sc);

  DateTime dt(year, month, day, hh, mm, ss, ff);
  auto ts = TimeSpan::fromMinutes(ts_hours * 60 + ts_minutes); // NOLINT(readability-magic-numbers)
  if (plus) {
    return dt - ts; // timezones with + are behind UTC
  }
  return dt + ts;
}
} // namespace kl
