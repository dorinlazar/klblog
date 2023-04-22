#pragma once

// Markwrite file format (.mw), format definition in progress.

#include "kl/klds.hpp"
#include "kl/kltext.hpp"
#include "kl/kltime.hpp"
#include "kl/klvalue.hpp"
#include "kl/textscanner.hpp"
#include <variant>

namespace kl {

class DocumentMetadata {

  Text m_title;
  DateTime m_published_time;
  DateTime m_last_update;
  List<Text> m_authors;
  Text m_featured_image;
  PValue m_properties;

public:
  void read(TextScanner& scanner);
};

class DocumentContent {
  Text m_base_content;

  void read_paragraph();

public:
  void read(TextScanner& scanner);
};

class Document {
  DocumentMetadata m_metadata;
  DocumentContent m_content;

public:
  explicit Document(const Text& content);
};

} // namespace kl
