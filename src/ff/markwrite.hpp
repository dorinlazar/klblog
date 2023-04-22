#pragma once

// Markwrite file format (.mw), format definition in progress.

#include "kl/klds.hpp"
#include "kl/kltext.hpp"
#include "kl/kltime.hpp"
#include "kl/klvalue.hpp"
#include "kl/textscanner.hpp"
#include <variant>

namespace kl {

struct DocumentMetadata {
  Text title;
  DateTime publish;
  DateTime last_update;
  List<Text> authors;
  Text featured_image;
  PValue properties;

public:
  void read(TextScanner& scanner);
};

struct DocumentContent {
  Text base_content;

  void read_paragraph();

public:
  void read(TextScanner& scanner);
};

class Document {
  DocumentMetadata m_metadata;
  DocumentContent m_content;

public:
  Document(const Text& content);
};

} // namespace kl
