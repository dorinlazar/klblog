#pragma once

#include <exception>
#include <stdexcept>
#include "kltext.hpp"

namespace kl {
class OperationNotSupported : public std::logic_error {
public:
  OperationNotSupported(const Text& op, const Text& reason);
};

class InvalidInputData : public std::logic_error {
public:
  InvalidInputData(const Text& received, const Text& expected);
};

class IOException {
  Text m_why;

public:
  explicit IOException(const Text& why);
  const Text& what();
  static IOException current_standard_error();
};

} // namespace kl
