#pragma once

#include <variant>

#include "klds.hpp"
#include "kltext.hpp"

namespace kl {

enum class ValueType : int { Null = 0, Scalar = 1, Map = 2, List = 3 };

class Value {
public:
  using PValue = ptr<Value>;
  using ListValue = List<PValue>;
  using MapValue = Dict<Text, PValue>;
  using NullValue = struct {};

private:
  std::variant<NullValue, Text, MapValue, ListValue> _value;

public:
  Value(ValueType vt);
  Value(const Text& value);
  Value();

  static PValue create_null();
  static PValue create_scalar(const Text& value = ""_t);
  static PValue create_map();
  static PValue create_list();

  bool is_null() const;
  bool is_scalar() const;
  bool is_map() const;
  bool is_list() const;

  MapValue& as_map();
  ListValue& as_list();
  const MapValue& as_map() const;
  const ListValue& as_list() const;
  Text as_scalar() const;

public:
  ValueType type() const;
  TextChain to_string() const;

public:
  void set_value(const Text& txt);
  Text get_value() const;
  List<Text> get_array_value() const;

  void add(PValue v);
  void add(const Text& txt, PValue v);
  void add(const Text& txt);
  void add(const Text& txt, const Text& v);
  void clear();
  Value& operator[](int index) const;
  Value& operator[](const Text& key) const;
  PValue get(int index) const;
  PValue get(const Text& key) const;
  bool has(const Text& key) const;
  size_t size() const;
  std::optional<Text> get_opt(const kl::Text& path);

public:
  void perform(const std::function<void(NullValue&)>& null_op, const std::function<void(Text&)>& scalar_op,
               const std::function<void(MapValue&)>& map_op, const std::function<void(ListValue&)>& list_op);
};

using PValue = Value::PValue;
using ListValue = Value::ListValue;
using MapValue = Value::MapValue;
using NullValue = Value::NullValue;

} // namespace kl

bool operator==(const kl::Value& v, const kl::Text& t);
