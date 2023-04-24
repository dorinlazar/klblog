#pragma once
#include "kl/klvalue.hpp"
#include "kl/textscanner.hpp"

namespace kl {
class PoorConfig {
  PValue m_value;

public:
  explicit PoorConfig(const Text& filename);
  [[nodiscard]] PValue top() const;

public:
  static PValue parse(const Text& fragment, char split = ':');
  static PValue parse(TextScanner& scanner, char split = ':');
};
} // namespace kl
