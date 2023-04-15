#pragma once
#include "kl/kltime.hpp"

namespace kl {
class TimeCounter {
  kl::DateTime _start;
  kl::Text _op;

public:
  TimeCounter(const Text& operation);
  ~TimeCounter();
};
}