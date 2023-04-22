#pragma once
#include <iostream>
#include <functional>
#include <source_location>
#include <fmt/format.h>
#include <fmt/ranges.h>

// TODO: use fmt::print

namespace kl {
template <typename... Args>
inline void log(std::string_view frmt, Args&&... args) {
  // std::cout << fmt::format(frmt, std::forward<Args>(args)...) << "\n";
  std::cout << fmt::vformat(frmt, fmt::make_format_args(std::forward<Args>(args)...)) << "\n";
}

template <typename... Args>
inline void err(std::string_view frmt, Args&&... args) {
  std::cerr << fmt::vformat(frmt, fmt::make_format_args(std::forward<Args>(args)...)) << "\n";
}

template <typename... Args>
[[noreturn]] void fatal(std::string_view frmt, Args&&... args) {
  kl::err(frmt, std::forward<Args>(args)...);
  std::cout.flush();
  std::cerr.flush();
  std::abort();
}

inline void check(bool value) {
  if (value) [[likely]] {
    return;
  }
  fatal("");
}

template <typename... Ts>
inline void check(bool value, const Ts&... args) {
  if (value) [[likely]] {
    return;
  }
  fatal(args...);
}

inline void check_st(bool value, const std::source_location& location = std::source_location::current()) {
  if (value) {
    return;
  }
  fatal("Failure at:", location.file_name(), location.line());
}

template <typename EX>
inline void expect_ex(std::function<void()> op,
                      const std::source_location& location = std::source_location::current()) {
  try {
    op();
    fatal("Expected exception was not triggered at: {}:{}", location.file_name(), location.line());
  } catch (const EX&) {
  } catch (...) {
    fatal("Invalid exception triggered at: {}:{}", location.file_name(), location.line());
  }
}

} // namespace kl
