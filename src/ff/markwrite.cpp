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
  m_properties = PoorConfig::parse(scanner);
  check(m_properties->is_map(), "Expected a map as document metadata");
  m_title = m_properties->get_opt("title").value_or(""_t);
  m_featured_image = m_properties->get_opt("image").value_or(""_t);

  auto pubTime = m_properties->get_opt("date");
  m_published_time = pubTime.has_value() ? DateTime::parse(*pubTime) : defaultDate;

  pubTime = m_properties->get_opt("updated");
  m_last_update = pubTime.has_value() ? DateTime::parse(*pubTime) : m_published_time;

  auto m = m_properties->as_map();
  if (m_properties->as_map().has("author")) {
    m_authors = m_properties->get("author")->get_array_value();
  }
}

void DocumentContent::read_paragraph() {}

void DocumentContent::read(TextScanner& scanner) {
  m_base_content = scanner.remainder();
  while (!scanner.empty()) {
    read_paragraph();
  }
}
} // namespace kl
