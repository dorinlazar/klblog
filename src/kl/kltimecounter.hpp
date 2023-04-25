#pragma once
#include "kl/kltime.hpp"

namespace kl {
class TimeCounter {
  kl::DateTime m_start;
  kl::Text m_op;

public:
  TimeCounter(const Text& operation);
  ~TimeCounter();
};
}