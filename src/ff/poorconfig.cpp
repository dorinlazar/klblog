#include "poorconfig.hpp"
#include <kl/textscanner.hpp>
#include <kl/klfs.hpp>

namespace kl {
class PoorConfigParser {
  TextScanner& m_scanner;
  char m_split;
  const char m_comment = '#';
  bool m_preamble = false;

  bool is_useless_line() {
    auto start_of_line = m_scanner.location();
    auto line = m_scanner.read_line().trim_left();
    if (line.size() == 0 || line[0] == m_comment) {
      return true;
    }
    m_scanner.restore_location(start_of_line);
    return false;
  }

  void discard_after_value_junk() {
    m_scanner.skip_whitespace(NewLineHandling::Keep);
    if (!m_scanner.empty()) {
      if (m_scanner.top_char() != m_comment && m_scanner.top_char() != '\n') {
        m_scanner.error("Trash at the end of the value");
      }
      m_scanner.read_line();
    }
  }

public:
  PoorConfigParser(TextScanner& scanner, char split) : m_scanner(scanner), m_split(split) {
    if (m_scanner.starts_with("---\n"_t)) {
      m_scanner.read_line();
      m_preamble = true;
    }
  }

  PValue read_array() {
    auto value = Value::create_list();
    m_scanner.expect_ws('[');
    bool empty = true;
    while (!m_scanner.empty()) {
      m_scanner.skip_whitespace();
      if (empty && m_scanner.top_char() == ']') {
        m_scanner.read_char();
        discard_after_value_junk();
        return value;
      }
      empty = false;
      value->add(m_scanner.read_quoted_string());
      m_scanner.skip_whitespace();
      auto next = m_scanner.read_char();
      if (next.character == ']' && !next.escaped) {
        discard_after_value_junk();
        return value;
      }
      if (next.escaped || next.character != ',') {
        m_scanner.error("Field separator expected when reading list");
      }
    }
    m_scanner.error("Unexpected end of input while scanning list");
    return nullptr;
  }

  PValue read_map(uint32_t min_indent = 0) {
    auto value = Value::create_map();
    std::optional<uint32_t> indent_level;
    while (!m_scanner.empty()) {
      if (is_useless_line()) {
        continue;
      }
      if (m_preamble && (m_scanner.starts_with("---") || m_scanner.starts_with("..."))) {
        if (min_indent == 0) {
          m_scanner.skip(3);
          auto loc = m_scanner.location();
          if (m_scanner.read_line().trim().size() != 0) {
            m_scanner.restore_location(loc);
          }
        }
        break;
      }
      if (!indent_level.has_value()) {
        indent_level = m_scanner.get_indent_level();
        if (*indent_level < min_indent) {
          m_scanner.error("Invalid indentation for start of map");
        }
      }
      auto current_indent = m_scanner.get_indent_level();
      if (indent_level > current_indent) { // end for this level of indentation
        return value;
      }
      if (indent_level < m_scanner.get_indent_level()) {
        m_scanner.error("Bad indentation for map key/value pair");
      }
      m_scanner.skip_whitespace(NewLineHandling::Keep);
      auto key = m_scanner.read_word();
      if (key.size() == 0) {
        m_scanner.error("Empty key or invalid indentation");
      }
      m_scanner.expect_ws(m_split);
      m_scanner.skip_whitespace(NewLineHandling::Keep);
      const char top_char = m_scanner.top_char();
      if (top_char == '\n' || top_char == m_comment) { // we'll have a map value
        m_scanner.read_line();
        value->add(key, read_map(*indent_level));
      } else if (top_char == '"') {
        const Text v = m_scanner.read_quoted_string();
        discard_after_value_junk();
        value->add(key, v);
      } else if (top_char == '[') {
        value->add(key, read_array());
      } else {
        auto [v, comment] = m_scanner.read_line().split_next_char(m_comment);
        value->add(key, v.trim());
      }
    }
    return value;
  }
};

PValue PoorConfig::parse(const Text& fragment, char split) {
  TextScanner scanner(fragment);
  PoorConfigParser parser(scanner, split);
  return parser.read_map();
}

PValue PoorConfig::parse(TextScanner& scanner, char split) {
  PoorConfigParser parser(scanner, split);
  return parser.read_map();
}

PoorConfig::PoorConfig(const Text& filename) { m_value = parse(FileReader(filename).read_all()); }
PValue PoorConfig::top() const { return m_value; }
} // namespace kl
