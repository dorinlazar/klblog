#include "klvalue.hpp"
#include "klfs.hpp"

namespace kl {
Value::Value(ValueType vt) {
  switch (vt) {
  case ValueType::Null: _value = NullValue(); break;
  case ValueType::Scalar: _value = Text(); break;
  case ValueType::Map: _value = MapValue(); break;
  case ValueType::List: _value = ListValue(); break;
  }
}
Value::Value(const Text& value) { _value = value; }
Value::Value() { _value = NullValue(); }

PValue Value::create_null() { return std::make_shared<Value>(); }
PValue Value::create_scalar(const Text& value) { return std::make_shared<Value>(value); }
PValue Value::create_map() { return std::make_shared<Value>(ValueType::Map); }
PValue Value::create_list() { return std::make_shared<Value>(ValueType::List); }

bool Value::is_null() const { return _value.index() == (size_t)ValueType::Null; }
bool Value::is_scalar() const { return _value.index() == (size_t)ValueType::Scalar; }
bool Value::is_map() const { return _value.index() == (size_t)ValueType::Map; }
bool Value::is_list() const { return _value.index() == (size_t)ValueType::List; }

MapValue& Value::as_map() {
  if (is_null()) {
    create_map();
  }
  return std::get<(int)ValueType::Map>(_value);
}
ListValue& Value::as_list() {
  if (is_null()) {
    create_list();
  }
  return std::get<(int)ValueType::List>(_value);
}
const MapValue& Value::as_map() const { return std::get<(int)ValueType::Map>(_value); }
const ListValue& Value::as_list() const { return std::get<(int)ValueType::List>(_value); }
Text Value::as_scalar() const { return std::get<(int)ValueType::Scalar>(_value); }

ValueType Value::type() const { return ValueType(_value.index()); }
void Value::setValue(const Text& txt) { _value = txt; }
Text Value::getValue() const { return as_scalar(); }
List<Text> Value::getArrayValue() const {
  if (is_scalar()) {
    return {as_scalar()};
  }
  return as_list().transform<Text>([](const PValue& p) { return p->as_scalar(); });
}

void Value::add(PValue v) { as_list().add(v); }
void Value::add(const Text& txt, PValue v) { as_map().add(txt, v); }
void Value::add(const Text& txt) { as_list().add(create_scalar(txt)); }
void Value::add(const Text& txt, const Text& v) { as_map().add(txt, create_scalar(v)); }
void Value::clear() {
  perform(
      nullptr, [](Text& textv) { textv = ""_t; }, [](MapValue& mapv) { mapv.clear(); },
      [](ListValue& listv) { listv.clear(); });
}
Value& Value::operator[](int index) const { return *as_list()[index]; }
Value& Value::operator[](const Text& key) const { return *(as_map()[key]); }
PValue Value::get(int index) const { return as_list()[index]; }
PValue Value::get(const Text& key) const { return as_map()[key]; }
bool Value::has(const Text& key) const { return is_map() && as_map().has(key); }

size_t Value::size() const {
  if (is_null()) {
    return 0;
  }
  if (is_scalar()) {
    return 1;
  }
  if (is_list()) {
    return std::get<(size_t)ValueType::List>(_value).size();
  }
  if (is_map()) {
    return std::get<(size_t)ValueType::Map>(_value).size();
  }
  return 0;
}

inline void Value::perform(std::function<void(NullValue&)> nullOp, std::function<void(Text&)> scalarOp,
                           std::function<void(MapValue&)> mapOp, std::function<void(ListValue&)> listOp) {
  if (is_null()) {
    if (nullOp) {
      nullOp(std::get<(size_t)ValueType::Null>(_value));
    }
  } else if (is_scalar()) {
    if (scalarOp) {
      scalarOp(std::get<(size_t)ValueType::Scalar>(_value));
    }
  } else if (is_map()) {
    if (mapOp) {
      mapOp(std::get<(size_t)ValueType::Map>(_value));
    }
  } else if (is_list()) {
    if (listOp) {
      listOp(std::get<(size_t)ValueType::List>(_value));
    }
  }
}

TextChain Value::to_string() const {
  TextChain tc;
  bool commaNeeded = false;
  switch (type()) {
  case ValueType::Null: return {"null"_t};
  case ValueType::Scalar: return {"\""_t, as_scalar(), "\""_t};
  case ValueType::List:
    tc.add("["_t);
    for (const auto& v: as_list()) {
      if (commaNeeded) {
        tc.add(","_t);
      } else {
        commaNeeded = true;
      }
      tc.add(v->to_string());
    }
    tc.add("]"_t);
    break;
  case ValueType::Map:
    tc.add("{"_t);
    for (const auto& [k, v]: as_map()) {
      if (commaNeeded) {
        tc.add(","_t);
      } else {
        commaNeeded = true;
      }
      tc.add(k);
      tc.add(":"_t);
      tc.add(v->to_string());
    }
    tc.add("}"_t);
    break;
  }
  return tc;
}

std::optional<Text> Value::getOpt(const kl::Text& path) {
  Value* v = this;
  for (const auto& val: kl::FilePath(path).breadcrumbs()) {
    if (v->is_map()) {
      auto& map = v->as_map();
      v = map.get(val).get();
    } else {
      v = nullptr;
    }
    if (v == nullptr) {
      break;
    }
  }
  if (v != nullptr && v->is_scalar()) {
    return v->getValue();
  }
  return {};
}

} // namespace kl

bool operator==(const kl::Value& v, const kl::Text& t) { return v.is_scalar() && v.getValue() == t; }
