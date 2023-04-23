#pragma once
#include <exception>
#include "kl/kltext.hpp"

namespace kl {
class ParsingError : public std::logic_error {
  uint32_t _line;
  uint32_t _column;

public:
  ParsingError(const Text& why, uint32_t line, uint32_t column);
  uint32_t line() const;
  uint32_t column() const;
};

struct ParsedCharacter {
  char character = '\0';
  bool escaped = false;
};

enum class NewLineHandling { Skip, Keep };

class TextScanner {

public:
  TextScanner(const Text& source);

  uint32_t line() const;
  uint32_t column() const;

  void skip_whitespace(NewLineHandling handling = NewLineHandling::Skip);
  bool empty() const;
  char top_char() const;
  ParsedCharacter read_char();
  ParsedCharacter readCharEscaped();
  Text read_quoted_string();
  Text readUntil(char character);
  Text read_word();
  Text read_line();
  Text remainder() const;
  uint32_t read_digit();
  void expect(char character);
  void expect_ws(char character, NewLineHandling handling = NewLineHandling::Keep);
  bool starts_with(const Text& txt);
  void skip(uint32_t nChars);
  uint32_t get_indent_level() const;

public:
  struct DataLocation {
    friend class TextScanner;

  protected:
    uint32_t _line = 1;
    uint32_t _column = 1;
    uint32_t _offset = 0;
    uint32_t _dataLeft = 0;
    const char* _current;
  };

  void rewind();
  const DataLocation& location() const;
  void restore_location(const DataLocation& location);

public:
  void error(const Text& why) const;

private:
  DataLocation loc;
  Text _originalSource;
  void advance();
};

} // namespace kl
