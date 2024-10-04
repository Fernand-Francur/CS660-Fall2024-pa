#include <algorithm>
#include <cstring>
#include <db/Tuple.hpp>
#include <stdexcept>

using namespace db;

Tuple::Tuple(const std::vector<field_t> &fields) : fields(fields) {}

type_t Tuple::field_type(size_t i) const {
  const field_t &field = fields.at(i);
  if (std::holds_alternative<int>(field)) {
    return type_t::INT;
  }
  if (std::holds_alternative<double>(field)) {
    return type_t::DOUBLE;
  }
  if (std::holds_alternative<std::string>(field)) {
    return type_t::CHAR;
  }
  throw std::logic_error("Unknown field type");
}

size_t Tuple::size() const { return fields.size(); }

const field_t &Tuple::get_field(size_t i) const { return fields.at(i); }

TupleDesc::TupleDesc(const std::vector<type_t> &types, const std::vector<std::string> &names)
  // TODO pa2: add initializations if needed
{
  if (types.size() != names.size()) {
    throw std::logic_error("type and names vectors are not the same size");
  }
  for (int i = 1; i < names.size(); i++) {
    if (std::count(names.begin(), names.end(), names.at(i)) > 1) {
      throw std::logic_error("names are not unique");
    }
  }
  this->types = types;
  this->names = names;
  this->sizeTuple = types.size();
}

bool TupleDesc::compatible(const Tuple &tuple) const {
  if (tuple.size() != this->types.size()) {
    return false;
  }
  for (int i = 0; i < tuple.size(); i++) {
    if (tuple.field_type(i) != this->types.at(i)) {
      return false;
    }
  }
  return true;
}

size_t TupleDesc::index_of(const std::string &name) const {
  bool exists = false;
  size_t index;
  for (int i = 0; i < this->names.size(); i++) {
    if (this->names.at(i) == name) {
      exists = true;
      index = i;
    }
  }
  if (!exists) {
    throw std::logic_error("No such name in tuple");
  }
  return index;
}

size_t TupleDesc::offset_of(const size_t &index) const {
  if (index >= this->sizeTuple) {
    throw std::logic_error("Index out of bounds");
  }
  size_t offset = 0;
  for (int i = 0; i < index; i++) {
    if (this->types[i] == type_t::INT) {
      offset += INT_SIZE;
    } else if (this->types[i] == type_t::CHAR) {
      offset += CHAR_SIZE;
    } else if (this->types[i] == type_t::DOUBLE) {
      offset += DOUBLE_SIZE;
    } else {
      throw std::logic_error("Unknown type");
    }
  }
  return offset;
}

size_t TupleDesc::length() const {
  size_t length = 0;
  for (int i = 0; i < this->sizeTuple; i++) {
    if (this->types[i] == type_t::INT) {
      length += INT_SIZE;
    } else if (this->types[i] == type_t::CHAR) {
      length += CHAR_SIZE;
    } else if (this->types[i] == type_t::DOUBLE) {
      length += DOUBLE_SIZE;
    } else {
      throw std::logic_error("Unknown type");
    }
  }
  return length;
}

size_t TupleDesc::size() const {
  return this->sizeTuple;
}

Tuple TupleDesc::deserialize(const uint8_t *data) const {
  // TODO pa2: implement
  size_t currSize = this->sizeTuple;
  std::vector<field_t> fields;
  int offset = 0;
  for (int i = 0; i < currSize; i++) {
    type_t currType = this->types[i];
    if (currType == type_t::INT) {
      int newField;
      memcpy(&newField,(data + offset),INT_SIZE);
      fields.emplace_back(newField);// = newField;
      offset += INT_SIZE;
    } else if (currType == type_t::CHAR) {
      char newField[64];
      memcpy(newField,(data + offset),CHAR_SIZE);
      fields.emplace_back(newField); //fields.at(i) = (std::string)newField;
      offset += CHAR_SIZE;
    } else if (currType == type_t::DOUBLE) {
      double newField;
      memcpy(&newField,(data + offset),DOUBLE_SIZE);
      fields.emplace_back(newField); //fields.at(i) = newField;
      offset += DOUBLE_SIZE;
    } else {
      throw std::logic_error("Unknown type");
    }
  }
  return Tuple(fields);
}

void TupleDesc::serialize(uint8_t *data, const Tuple &t) const {
  // TODO pa2: implement
    size_t currSize = t.size();
    int offset = 0;
    for (int i = 0; i < currSize; i++) {
      type_t currType = this->types[i];
      if (currType == type_t::INT) {
        int newField = std::get<int>(t.get_field(i));
        memcpy((data + offset),&newField,sizeof(int));
        offset += INT_SIZE;
      } else if (currType == type_t::CHAR) {
        const char* newField = (std::get<std::string>(t.get_field(i))).c_str();
        memcpy((data + offset),newField,CHAR_SIZE);
        offset += CHAR_SIZE;
      } else if (currType == type_t::DOUBLE) {
        double newField = std::get<double>(t.get_field(i));
        memcpy((data + offset), &newField,DOUBLE_SIZE);
        offset += DOUBLE_SIZE;
      } else {
        throw std::logic_error("Unknown type");
      }
    }
}

db::TupleDesc TupleDesc::merge(const TupleDesc &td1, const TupleDesc &td2) {
  std::vector<type_t> newTypes = td1.types;
  newTypes.insert(newTypes.end(), td2.types.begin(), td2.types.end());
  std::vector<std::string> newNames = td1.names;
  newNames.insert(newNames.end(), td2.names.begin(), td2.names.end());
  return TupleDesc(newTypes, newNames);
}
