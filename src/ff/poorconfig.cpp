#include "poorconfig.hpp"
#include "kl/textscanner.hpp"
#include "kl/klfs.hpp"
using namespace kl;

class PoorConfigParser {
  TextScanner& m_scanner;
  char _split;
  const char _comment = '#';
  bool _preamble = false;

  bool _uselessLine() {
    auto startOfLine = m_scanner.location();
    auto line = m_scanner.read_line().trim_left();
    if (line.size() == 0 || line[0] == _comment) {
      return true;
    }
    m_scanner.restoreLocation(startOfLine);
    return false;
  }

  void _discardAfterValueJunk() {
    m_scanner.skipWhitespace(NewLineHandling::Keep);
    if (!m_scanner.empty()) {
      if (m_scanner.topChar() != _comment && m_scanner.topChar() != '\n') {
        m_scanner.error("Trash at the end of the value");
      }
      m_scanner.read_line();
    }
  }

public:
  PoorConfigParser(TextScanner& scanner, char split) : m_scanner(scanner), _split(split) {
    if (m_scanner.startsWith("---\n"_t)) {
      m_scanner.read_line();
      _preamble = true;
    }
  }

  PValue readArray() {
    auto value = Value::createList();
    m_scanner.expectws('[');
    bool empty = true;
    while (!m_scanner.empty()) {
      m_scanner.skipWhitespace();
      if (empty && m_scanner.topChar() == ']') {
        m_scanner.read_char();
        _discardAfterValueJunk();
        return value;
      }
      value->add(m_scanner.readQuotedString());
      m_scanner.skipWhitespace();
      auto next = m_scanner.read_char();
      if (next.character == ']' && !next.escaped) {
        _discardAfterValueJunk();
        return value;
      }
      if (next.escaped || next.character != ',') {
        m_scanner.error("Field separator expected when reading list");
      }
    }
    m_scanner.error("Unexpected end of input while scanning list");
    return nullptr;
  }

  PValue readMap(uint32_t minIndent = 0) {
    auto value = Value::createMap();
    std::optional<uint32_t> indentLevel;
    while (!m_scanner.empty()) {
      if (_uselessLine()) {
        continue;
      }
      if (_preamble && (m_scanner.startsWith("---") || m_scanner.startsWith("..."))) {
        if (minIndent == 0) {
          m_scanner.skip(3);
          auto loc = m_scanner.location();
          if (m_scanner.read_line().trim().size() != 0) {
            m_scanner.restoreLocation(loc);
          }
        }
        break;
      }
      if (!indentLevel.has_value()) {
        indentLevel = m_scanner.getIndentationLevel();
        if (*indentLevel < minIndent) {
          m_scanner.error("Invalid indentation for start of map");
        }
      }
      auto currentIndent = m_scanner.getIndentationLevel();
      if (indentLevel > currentIndent) { // end for this level of indentation
        return value;
      }
      if (indentLevel < m_scanner.getIndentationLevel()) {
        m_scanner.error("Bad indentation for map key/value pair");
      }
      m_scanner.skipWhitespace(NewLineHandling::Keep);
      auto key = m_scanner.readWord();
      if (key.size() == 0) {
        m_scanner.error("Empty key or invalid indentation");
      }
      m_scanner.expectws(_split);
      m_scanner.skipWhitespace(NewLineHandling::Keep);
      char topChar = m_scanner.topChar();
      if (topChar == '\n' || topChar == _comment) { // we'll have a map value
        m_scanner.read_line();
        value->add(key, readMap(*indentLevel));
      } else if (topChar == '"') {
        Text v = m_scanner.readQuotedString();
        _discardAfterValueJunk();
        value->add(key, v);
      } else if (topChar == '[') {
        value->add(key, readArray());
      } else {
        auto [v, comment] = m_scanner.read_line().splitNextChar(_comment);
        value->add(key, v.trim());
      }
    }
    return value;
  }
};

PValue PoorConfig::parse(const Text& fragment, char split) {
  TextScanner scanner(fragment);
  PoorConfigParser parser(scanner, split);
  return parser.readMap();
}

PValue PoorConfig::parse(TextScanner& scanner, char split) {
  PoorConfigParser parser(scanner, split);
  return parser.readMap();
}

PoorConfig::PoorConfig(const Text& filename) { m_value = parse(FileReader(filename).read_all()); }
PValue PoorConfig::top() const { return m_value; }
