#pragma once

#include "kl/kltext.hpp"

namespace kl::mw {
using namespace kl;

enum class BlockType { Open, Paragraph };

struct Block {
  TextChain contents;
  BlockType type = BlockType::Open;

public:
  Block();

  bool opened();
  void close();
  bool try_consume_line(const kl::Text& text);
};

} // namespace kl::mw
