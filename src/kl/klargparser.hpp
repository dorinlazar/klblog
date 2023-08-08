#pragma once

#include "kltext.hpp"

namespace kl {

class Argument {};

// Inspiration source: https://docs.python.org/3/library/argparse.html
class ArgumentParser {
public:
  ArgumentParser(Text prog_name, Text description, Text epilog);

  void add(const Argument& argument);
  std::shared_ptr<ArgumentParser> create_subparser();
};

} // namespace kl
