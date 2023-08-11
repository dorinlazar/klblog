#pragma once

#include "kltext.hpp"
#include <unordered_map>

namespace kl {

class Argument {};

// Inspiration source: https://docs.python.org/3/library/argparse.html
class ArgumentParser {
public:
  ArgumentParser(Text prog_name, Text description, Text epilog);
  ArgumentParser(const ArgumentParser&) = default;
  ArgumentParser(ArgumentParser&&) = default;
  ArgumentParser& operator=(const ArgumentParser&) = default;
  ArgumentParser& operator=(ArgumentParser&&) = default;
  ~ArgumentParser() = default;

  void add(const Argument& argument);
  ArgumentParser& create_subparser(kl::Text feature);

private:
  ArgumentParser();
  bool m_child_parser = false;
  std::unordered_map<kl::Text, ArgumentParser> m_children;
};

} // namespace kl
