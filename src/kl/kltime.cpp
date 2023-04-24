#include "kltime.hpp"
#include <algorithm>
#include <chrono>
#include "kl/textscanner.hpp"
// #include <format>

namespace kl {

static constexpr bool kltime_leap_year(int32_t year) { // Leap year, for one-based 1-400 interval
  if (year < 0) {
    return false;
  }
  // NOLINTNEXTLINE(readability-magic-numbers)
  return ((year & 0x03) == 0) && year != 100 && year != 200 && year != 300;
}

using month_size_t = std::array<int32_t, TimeLimits::MonthsPerYear>;

static constexpr month_size_t MonthSizesArray = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static constexpr month_size_t LeapMonthSizesArray = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static constexpr const month_size_t& kltime_month_sizes(int32_t year) { // year in the interval 1-400.
  return kltime_leap_year(year) ? LeapMonthSizesArray : MonthSizesArray;
}

using delta_months_array_t = std::array<int32_t, TimeLimits::RegularYearsOffset * TimeLimits::MonthsPerYear + 1>;

static constexpr delta_months_array_t kltime_calculate_delta_months() {
  delta_months_array_t res;
  int32_t total_days = 0;
  res[0] = 0;
  int32_t index = 1;

  for (int32_t y = 0; y < TimeLimits::RegularYearsOffset; y++) {
    const auto& sizes = kltime_month_sizes(y + 1);
    for (int32_t m = 0; m < TimeLimits::MonthsPerYear; m++, index++) {
      total_days += sizes[m];
      res[index] = total_days;
    }
  }
  return res;
}

static const delta_months_array_t DeltaMonths = kltime_calculate_delta_months();

const DateTime DateTime::UnixEpoch(1970, 1, 1);
const DateTime DateTime::Max = DateTime::fromTicks(TimeLimits::MaxTicks);
const DateTime DateTime::Min = DateTime::fromTicks(TimeLimits::MinTicks);

Date DateTime::date() const {
  const auto d = m_ticks / TimeLimits::TicksPerDay;
  const auto fh = std::lldiv(d, TimeLimits::DaysInRegularInterval);
  const auto it = std::lower_bound(DeltaMonths.begin(), DeltaMonths.end(), fh.rem + 1) - 1;
  const auto delta_months = std::distance(DeltaMonths.begin(), it);
  const auto delta_years = delta_months / TimeLimits::MonthsPerYear;
  const auto leftover_months = delta_months % TimeLimits::MonthsPerYear;
  return Date{.year = static_cast<uint32_t>(fh.quot * TimeLimits::RegularYearsOffset + delta_years + 1),
              .month = static_cast<uint32_t>(leftover_months) + 1,
              .day = 1 + static_cast<uint32_t>(fh.rem) - *it};
}

DateTime DateTime::now() {
  const auto n = std::chrono::system_clock::now();
  const int64_t nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(n.time_since_epoch()).count();
  return DateTime(nanos / TimeLimits::NanosecondsPerSecond, nanos % TimeLimits::NanosecondsPerSecond);
}

int64_t DateTime::ticks() const { return m_ticks; }
int32_t DateTime::days() const { return static_cast<int32_t>(m_ticks / TimeLimits::TicksPerDay); }
TimeOfDay DateTime::timeOfDay() const {
  const int64_t day_ticks = m_ticks % TimeLimits::TicksPerDay;
  auto sec = lldiv(day_ticks, TimeLimits::TicksPerSecond);
  auto mn = div(static_cast<uint32_t>(sec.quot), TimeLimits::SecondsPerMinute);
  auto hr = div(mn.quot, TimeLimits::MinutesPerHour);
  return {.hour = static_cast<uint32_t>(hr.quot),
          .min = static_cast<uint32_t>(hr.rem),
          .sec = static_cast<uint32_t>(mn.rem),
          .nanos = static_cast<uint32_t>(sec.rem * TimeLimits::NanosecondsPerTick)};
}

DateTime DateTime::fromTicks(int64_t ticks) {
  DateTime dt;
  dt.m_ticks = ticks < TimeLimits::MinTicks   ? TimeLimits::MinTicks
               : ticks > TimeLimits::MaxTicks ? TimeLimits::MaxTicks
                                              : ticks;
  return dt;
}

DateTime::DateTime(time_t seconds, int32_t nsec) {
  m_ticks = DateTime::UnixEpoch.ticks() + seconds * TimeLimits::TicksPerSecond + nsec / TimeLimits::NanosecondsPerTick;
}

DateTime::DateTime(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t sec,
                   uint32_t nsec) {
  if (year < TimeLimits::MinYear || year > TimeLimits::MaxYear || month < 1 || month > TimeLimits::MonthsPerYear ||
      day < 1 || nsec >= TimeLimits::NanosecondsPerSecond || sec >= TimeLimits::SecondsPerMinute ||
      minute >= TimeLimits::MinutesPerHour || hour > TimeLimits::HoursPerDay) [[unlikely]] {
    m_ticks = 0;
    return;
  }
  year--;
  month--;
  day--;
  const auto regular_intervals = year / TimeLimits::RegularYearsOffset;
  const auto remaining_years = year % TimeLimits::RegularYearsOffset;
  const auto& monthsizes = kltime_month_sizes(static_cast<int32_t>(remaining_years + 1));
  if (day >= monthsizes[month]) [[unlikely]] {
    m_ticks = 0;
    return;
  }
  const auto seconds = sec + minute * TimeLimits::SecondsPerMinute + hour * TimeLimits::SecondsPerHour;
  const auto days = DeltaMonths[remaining_years * TimeLimits::MonthsPerYear + month] + day +
                    regular_intervals * TimeLimits::DaysInRegularInterval;
  m_ticks = nsec / TimeLimits::NanosecondsPerTick;
  m_ticks += seconds * TimeLimits::TicksPerSecond;
  m_ticks += days * TimeLimits::TicksPerDay;
}

TimeSpan DateTime::operator-(const DateTime d) const { return {.ticks = m_ticks - d.m_ticks}; }
DateTime DateTime::operator-(TimeSpan ts) const { return fromTicks(m_ticks - ts.ticks); }
DateTime DateTime::operator+(TimeSpan ts) const { return fromTicks(m_ticks + ts.ticks); }

std::ostream& operator<<(std::ostream& os, kl::DateTime t) {
  auto date = t.date();
  auto time = t.timeOfDay();
  return os << fmt::format("{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0>3}", date.year, date.month, date.day,
                           time.hour, time.min, time.sec, time.nanos / TimeLimits::NanosecondsPerMillisecond);
}

std::ostream& operator<<(std::ostream& os, TimeSpan t) {
  if (t.ticks < 0) {
    os << "-";
    t.ticks = -t.ticks;
  }
  const uint32_t days = t.days();
  const uint32_t hours = t.hours();
  const uint32_t minutes = t.minutes();
  const uint32_t seconds = t.seconds();
  const uint32_t millis = t.milliseconds();
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

  const bool has_splitter = sc.top_char() == '-';

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
  uint32_t hh = 0;
  uint32_t mm = 0;
  uint32_t ss = 0;
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
  const bool has_splitter = sc.top_char() == ':';
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
    ff *= 1000;                                         // NOLINT(readability-magic-numbers)
    ff += static_cast<uint64_t>(sc.read_digit() * 100); // NOLINT(readability-magic-numbers)
    ff += static_cast<uint64_t>(sc.read_digit() * 10);  // NOLINT(readability-magic-numbers)
    ff += static_cast<uint64_t>(sc.read_digit());
    ff *= 1000; // NOLINT(readability-magic-numbers)
  } else {
    ff *= 1'000'000; // NOLINT(readability-magic-numbers)
  }
  return {hh, mm, ss, ff};
}

std::tuple<bool, uint32_t, uint32_t> kltime_read_timezone(TextScanner& sc) {
  uint32_t ts_hours = 0;
  uint32_t ts_minutes = 0;
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
  const size_t min_date_length = 8;
  if (src.size() < min_date_length) [[unlikely]] {
    throw InvalidInputData(src, "A valid date has at least 8 characters"_t);
  }
  TextScanner sc(src);
  const auto [year, month, day] = kltime_read_date(sc);
  const auto [hh, mm, ss, ff] = kltime_read_time(sc);
  const auto [plus, ts_hours, ts_minutes] = kltime_read_timezone(sc);

  const DateTime dt(year, month, day, hh, mm, ss, ff);
  const auto ts = TimeSpan::fromMinutes(ts_hours * TimeLimits::MinutesPerHour + ts_minutes);
  if (plus) {
    return dt - ts; // timezones with + are behind UTC
  }
  return dt + ts;
}
} // namespace kl
