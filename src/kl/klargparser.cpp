#include "klargparser.hpp"
#include "klexcept.hpp"

namespace kl {
std::shared_ptr<ArgumentParser> ArgumentParser::create_subparser(kl::Text feature) {
  if (m_children.contains(feature)) {
    throw DuplicateIndex(feature);
  }
  // auto parser = std::make_shared<ArgumentParser>();
  // m_children.emplace(feature, parser);
  // return parser;
  return {};
}

} // namespace kl
