#pragma once

#include "kltext.hpp"
#include <unordered_map>

namespace kl {

class Argument {};

// Inspiration source: https://docs.python.org/3/library/argparse.html
class ArgumentParser {
public:
  ArgumentParser(Text prog_name, Text description, Text epilog);
  ArgumentParser(const ArgumentParser&) = delete;
  ArgumentParser(ArgumentParser&&) = delete;
  ArgumentParser& operator=(const ArgumentParser&) = delete;
  ArgumentParser& operator=(ArgumentParser&&) = delete;
  ~ArgumentParser() = default;

  void add(const Argument& argument);
  std::shared_ptr<ArgumentParser> create_subparser(kl::Text feature);

private:
  ArgumentParser();
  bool m_child_parser = false;
  std::unordered_map<kl::Text, std::shared_ptr<ArgumentParser>> m_children;
};

} // namespace kl
