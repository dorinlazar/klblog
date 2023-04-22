#include "markwrite.hpp"
#include "ff/poorconfig.hpp"

namespace kl {
Document::Document(const Text& content) {
  TextScanner scanner(content);
  m_metadata.read(scanner);
  m_content.read(scanner);
}

void DocumentMetadata::read(TextScanner& scanner) {
  static DateTime defaultDate(2020, 1, 1);
  properties = PoorConfig::parse(scanner);
  check(properties->isMap(), "Expected a map as document metadata");
  title = properties->getOpt("title").value_or(""_t);
  featured_image = properties->getOpt("image").value_or(""_t);

  auto pubTime = properties->getOpt("date");
  publish = pubTime.has_value() ? DateTime::parse(*pubTime) : defaultDate;

  pubTime = properties->getOpt("updated");
  last_update = pubTime.has_value() ? DateTime::parse(*pubTime) : publish;

  auto m = properties->asMap();
  if (properties->asMap().has("author")) {
    authors = properties->get("author")->getArrayValue();
  }
}

void DocumentContent::read_paragraph() {}

void DocumentContent::read(TextScanner& scanner) {
  base_content = scanner.remainder();
  while (!scanner.empty()) {
    read_paragraph();
  }
}
} // namespace kl