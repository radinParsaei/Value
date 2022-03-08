#ifndef VALUE_H
#define VALUE_H

#ifndef USE_DOUBLE
#ifdef USE_BIG_NUMBER
#include <BigNumber.h>
#define NUMBER BigNumber
#define NUMBER_FROM_STRING NUMBER
#else
#include <sstream>
#include <iomanip>
#include <gmpxx.h>
#define NUMBER mpf_class
#define NUMBER_FROM_STRING NUMBER
#endif
#else
#define NUMBER double
#define NUMBER_FROM_STRING atof
#if !__has_include("Arduino.h")
#include <sstream>
#include <iomanip>
#include <cmath>
#endif
#endif

#ifdef linux
#include <math.h>
#include <algorithm>
#endif

#ifdef USE_ARDUINO_STRING
#define TEXT String
#else
#include <string>
#define TEXT std::string
#endif

#ifdef USE_ARDUINO_ARRAY
#include <Array.h> // library by peterpolidoro (https://github.com/janelia-arduino/Array)
#define ARRAY Array<Value*, MAX_FIXED_ARRAY_SIZE>
#else
#include <vector>
#define ARRAY std::vector<Value>
#ifdef USE_BIG_NUMBER
#ifndef USE_ARDUINO_STRING
#include <sstream>
#include <iomanip>
#endif
#endif
#endif

#ifdef USE_DOUBLE
#define IS_NUM(x) x.getType() == Types::Number
#else
#define IS_NUM(x) x.getType() == Types::Number || x.getType() == Types::BigNumber
#endif

#define USE_COUNT_TYPE char
#define modify_linked()     \
    if (copyBeforeModification) { \
      clone(); \
      copyBeforeModification = false; \
    }

#define _release_value(elseExp) \
    if (useCount != 0) { \
      if (*useCount == 0) { \
        delete useCount; \
        useCount = 0; \
      } else { \
        (*useCount) --; \
        elseExp; \
      } \
    }

class Value;

#ifdef USE_ARDUINO_STRING
int compareValue(const void *cmp1, const void *cmp2);
int compareValueNumeric(const void *cmp1, const void *cmp2);
#endif

#ifdef USE_NOSTD_MAP
class Pair {
public:
  Value* key;
  Value* value;
  Pair() {}
  Pair& operator= (const Pair&);
};
#else
#include <unordered_map>
class HashFunction {
public:
  size_t operator() (const Value& v) const;
};
#endif

enum class Types { Null = 0, True, False, Number, BigNumber, Text, Array, Map };

#ifndef MAX_FIXED_MAP_SIZE
#define MAX_FIXED_MAP_SIZE MAX_FIXED_ARRAY_SIZE
#endif

class Value {
private:
  Value(Value* v) {
    this->data = v->data;
    this->type = v->type;
    useCount = v->useCount;
    if (useCount) (*useCount) ++;
  }
  typedef union {
#ifndef USE_DOUBLE
    NUMBER* number;
    double smallNumber;
#else
    double number;
#endif
    TEXT* text;
    ARRAY* array;
#ifdef USE_NOSTD_MAP
    Array<Pair, MAX_FIXED_MAP_SIZE>* map;
#else
    std::unordered_map<Value, Value, HashFunction>* map;
#endif
} Data;
  Data data;
  Types type = Types::Null;
public:
  USE_COUNT_TYPE* useCount = 0;
  bool copyBeforeModification = false;
  void clone() {
    if (type == Types::Text) {
      TEXT* t = new TEXT(*data.text);
      data.text = t;
      (*useCount) --;
      useCount = new USE_COUNT_TYPE; *useCount = 0;
    } 
#ifndef USE_DOUBLE
    else if (type == Types::BigNumber) {
      NUMBER* t = new NUMBER(*data.number);
      data.number = t;
      (*useCount) --;
      useCount = new USE_COUNT_TYPE; *useCount = 0;
    }
#endif
    else if (type == Types::Array) {
      ARRAY* t = new ARRAY(*data.array);
      data.array = t;
      (*useCount) --;
      useCount = new USE_COUNT_TYPE; *useCount = 0;
#if !defined(USE_ARDUINO_ARRAY) && defined(VECTOR_RESERVED_SIZE)
      data.array->reserve(VECTOR_RESERVED_SIZE);
#endif
    } else if (type == Types::Map) {
#ifndef USE_NOSTD_MAP
      std::unordered_map<Value, Value, HashFunction>* t = new std::unordered_map<Value, Value, HashFunction>();
      *t = *data.map;
      data.map = t;
#else
      Array<Pair, MAX_FIXED_MAP_SIZE>* t = new Array<Pair, MAX_FIXED_MAP_SIZE>(*data.map);
      data.map = t; 
#endif
      (*useCount) --;
      useCount = new USE_COUNT_TYPE; *useCount = 0;
    }
  }

  // free unused pointers
  void freeUnusedMemory() {
    _release_value(return)
    if (type == Types::Text && data.text != 0) {
      delete data.text;
      data.text = 0;
    } 
#ifndef USE_DOUBLE
    else if (type == Types::BigNumber && data.number != 0) {
      delete data.number;
      data.number = 0;
    }
#endif
    else if (type == Types::Array && data.array != 0) {
      delete data.array;
      data.array = 0;
    } else if (type == Types::Map && data.map != 0) {
      delete data.map;
      data.map = 0;
    }
  }
  Value () { type = Types::Null; }
  Value (bool b) {
    if (b) type = Types::True;
    else type = Types::False;
  }
  Value (Types t) {
    if (t == Types::Array) {
      useCount = new USE_COUNT_TYPE; *useCount = 0;
      data.array = new ARRAY();
#if !defined(USE_ARDUINO_ARRAY) && defined(VECTOR_RESERVED_SIZE)
      data.array->reserve(VECTOR_RESERVED_SIZE);
#endif
    } else if (t == Types::Map) {
      useCount = new USE_COUNT_TYPE; *useCount = 0;
#ifdef USE_NOSTD_MAP
      data.map = new Array<Pair, MAX_FIXED_MAP_SIZE>();
#else
      data.map = new std::unordered_map<Value, Value, HashFunction>();
#endif
    }
    type = t;
  }
#ifndef USE_DOUBLE
  Value (const NUMBER& n) {
    this->data.number = new NUMBER(n);
    type = Types::BigNumber;
    useCount = new USE_COUNT_TYPE; *useCount = 0;
  }
#endif
  Value (int n) {
#ifndef USE_DOUBLE
    this->data.smallNumber = n;
#else
    this->data.number = n;
#endif
    type = Types::Number;
  }
  Value (double n) {
#ifndef USE_DOUBLE
    this->data.smallNumber = n;
#else
    this->data.number = n;
#endif
    type = Types::Number;
  }
  Value (long n) {
#ifndef USE_DOUBLE
    this->data.smallNumber = n;
#else
    this->data.number = n;
#endif
    type = Types::Number;
  }
  Value (const TEXT& s) {
    data.text = new TEXT(s);
    type = Types::Text;
    useCount = new USE_COUNT_TYPE; *useCount = 0;
  }
  Value (const char* s) {
    data.text = new TEXT(s);
    type = Types::Text;
    useCount = new USE_COUNT_TYPE; *useCount = 0;
  }
  Value (Value&& v) : data(v.data), type(v.type), useCount(v.useCount) {
    if (useCount) (*useCount) ++;
  }
  Value (const Value& v) {
    data = v.data;
    type = v.type;
    copyBeforeModification = true;
    useCount = v.useCount;
    if (useCount) (*useCount) ++;
  }
  Value (Data data, Types type, USE_COUNT_TYPE* useCount): data(data), type(type), useCount(useCount) {
    if (useCount) (*useCount) ++;
  }
  // free unused pointers when the object is destructing
  ~Value () {
    freeUnusedMemory();
  }

  void operator= (const Value& v) {
    freeUnusedMemory();
    data = v.data;
    type = v.type;
    copyBeforeModification = true;
    useCount = v.useCount;
    if (useCount) (*useCount) ++;
  }

  void be(Value* v) {
    freeUnusedMemory();
    data = v->data;
    type = v->type;
    useCount = v->useCount;
    if (useCount) (*useCount) ++;
    copyBeforeModification = false;
  }

  void operator= (bool v) {
    freeUnusedMemory();
    if (v) type = Types::True;
    else type = Types::False;
    copyBeforeModification = false;
  }

  void operator= (Types t) {
    freeUnusedMemory();
    copyBeforeModification = false;
    if (t == Types::Array) {
      useCount = new USE_COUNT_TYPE; *useCount = 0;
      data.array = new ARRAY();
#if !defined(USE_ARDUINO_ARRAY) && defined(VECTOR_RESERVED_SIZE)
      data.array->reserve(VECTOR_RESERVED_SIZE);
#endif
    } else if (t == Types::Map) {
      useCount = new USE_COUNT_TYPE; *useCount = 0;
#ifdef USE_NOSTD_MAP
      data.map = new Array<Pair, MAX_FIXED_MAP_SIZE>();
#else
      data.map = new std::unordered_map<Value, Value, HashFunction>();
#endif
    }
    type = t;
  }

  void operator= (int n) {
    freeUnusedMemory();
    copyBeforeModification = false;
#ifndef USE_DOUBLE
    this->data.smallNumber = n;
#else
    this->data.number = n;
#endif
    type = Types::Number;
  }

  void operator= (long n) {
    freeUnusedMemory();
    copyBeforeModification = false;
#ifndef USE_DOUBLE
    this->data.smallNumber = n;
#else
    this->data.number = n;
#endif
    type = Types::Number;
  }

  void operator= (double n) {
    freeUnusedMemory();
    copyBeforeModification = false;
#ifndef USE_DOUBLE
    this->data.smallNumber = n;
#else
    this->data.number = n;
#endif
    type = Types::Number;
  }

  void operator= (const TEXT& t) {
    freeUnusedMemory();
    copyBeforeModification = false;
    this->data.text = new TEXT(t);
    type = Types::Text;
    useCount = new USE_COUNT_TYPE; *useCount = 0;
  }

  void operator= (const char* t) {
    freeUnusedMemory();
    copyBeforeModification = false;
    this->data.text = new TEXT(t);
    type = Types::Text;
    useCount = new USE_COUNT_TYPE; *useCount = 0;
  }

  void append(const Value& v, bool _clone = true) {
    modify_linked()
#ifdef USE_ARDUINO_ARRAY
    Value* value = new Value(v.data, v.type, v.useCount);
    data.array->push_back(value);
    (*data.array)[data.array->size() - 1]->copyBeforeModification = _clone;
#else
    data.array->emplace_back(v.data, v.type, v.useCount);
    (*data.array)[data.array->size() - 1].copyBeforeModification = _clone;
#endif
  }

  void remove(size_t i) {
    modify_linked()
#ifdef USE_ARDUINO_ARRAY
    data.array->remove(i);
#else
    data.array->erase(data.array->begin() + i);
#endif
  }

  void remove(const Value& i) {
    modify_linked()
    if (type == Types::Array) {
#ifdef USE_ARDUINO_ARRAY
      data.array->remove((long) i);
#else
      data.array->erase(data.array->begin() + (long) i);
#endif
    } else if (type == Types::Map) {
#ifdef USE_NOSTD_MAP
      for (int j = 0; j < data.map->size(); j++) {
        if (*(*data.map)[j].key == i) {
          data.map->remove((long) j);
        }
      }
#else
      data.map->erase(i);
#endif
    }
  }

  void clear() {
    modify_linked()
    if (type == Types::Array) {
      data.array->clear();
    } else if (type == Types::Text) {
      *data.text = "";
    }
  }

  NUMBER getNumber() const {
    if (type == Types::Number) {
#ifdef USE_DOUBLE
      return data.number;
#else
      return data.smallNumber;
    } else if (type == Types::BigNumber) {
      return *data.number;
#endif
    }
    return 0;
  }

  Value pop() {
    modify_linked()
    if (type == Types::Array) {
#ifdef USE_ARDUINO_ARRAY
      Value* v = (*data.array)[data.array->size() - 1];
      Value res(v->data, v->type, v->useCount);
#else
      Value& v = (*data.array)[data.array->size() - 1];
      Value res(v.data, v.type, v.useCount);
#endif
      data.array->pop_back();
      return res;
    } else if (type == Types::Text) {
#ifdef USE_ARDUINO_STRING
      size_t s = data.text->length() - 1;
      char t = data.text->charAt(s);
      data.text->remove(s);
      return TEXT(t);
#else
      char t = (*data.text)[data.text->size() - 1];
      data.text->pop_back();
      return TEXT("") + t;
#endif
    }
    return Types::Null;
  }

  void _pop() {
    modify_linked()
    if (type == Types::Array) {
      data.array->pop_back();
    } else if (type == Types::Text) {
#ifdef USE_ARDUINO_STRING
      size_t s = data.text->length() - 1;
      data.text->remove(s);
#else
      data.text->pop_back();
#endif
    }
  }

  inline Types getType() const {
    return type;
  }

  inline Data getData() const {
    return data;
  }

  void put(const Value& k, const Value& v) {
    modify_linked()
    if (type == Types::Map) {
#ifndef USE_NOSTD_MAP
      (*data.map)[k].be((Value*) &v);
#else
      Pair p;
      Value* value = new Value();
      *value = v;
      Value* key = new Value();
      *key = k;
      p.key = key;
      p.value = value;
      data.map->push_back(p);
#endif
    }
  }

  Value& get(const Value& k) const {
    if (type == Types::Map) {
#ifdef USE_NOSTD_MAP
      for (int i = 0; i < data.map->size(); i++) {
        if (*(*data.map)[i].key == k) {
          return *(*data.map)[i].value;
        }
      }
#else
      return (*data.map)[k];
#endif
    }
  }

  void set(const Value& i, const Value& v) {
    modify_linked()
    if (type == Types::Map) {
      put(i, v);
    } else if (type == Types::Array) {
#ifdef USE_ARDUINO_ARRAY
      Value* value = new Value();
      *value = v;
      long l = i;
      Value* n = new Value(Types::Null);
      while (data.array->size() < (l + 1)) {
        data.array->push_back(n);
      }
      (*data.array)[l] = value;
#else
      long l = i;
      if (data.array->size() < (l + 1)) data.array->resize(l + 1);
      (*data.array)[l] = v;
#endif
    } else if (type == Types::Text && v.type == Types::Text) {
#ifdef USE_ARDUINO_STRING
      long x = i;
      String s = v.toString();
      for (long j = x; j < x + v.length(); j++) {
        data.text->setCharAt(j, s[j - x]);
      }
#else
      data.text->replace((long) i, v.length(), v.toString());
#endif
    }
  }

  void insert(Value i, Value v) {
    modify_linked()
    if (type == Types::Array) {
#ifdef USE_ARDUINO_ARRAY
      long l = i;
      if (l < data.array->size()) {
        Value* value = new Value();
        *value = v;
        Value** p = data.array->data();
        ARRAY* tmp = new ARRAY();
        while (tmp->size() < data.array->size() + 1) {
          tmp->push_back(0);
        }
        memcpy(tmp->data(), p, l + 1);
        memcpy(tmp->data() + l + 1, p + l, (data.array->size()) - l + 2);
        (*tmp)[l] = value;
        freeUnusedMemory();
        data.array = tmp;
      } else {
        set(i, v);
      }
#else
      long l = i;
      if (data.array->size() < (l + 1)) data.array->resize(l + 1);
      data.array->insert(data.array->begin() + l, v);
#endif
    } else if (type == Types::Text && v.type == Types::Text) {
#ifdef USE_ARDUINO_STRING
      long l = i;
      *data.text = data.text->substring(0, l) + v.toString() + data.text->substring(l);
#else
      data.text->replace((long) i, 0, v.toString());
#endif
    }
  }

  TEXT toString() const {
    if (type == Types::Number) {
#ifdef USE_DOUBLE
#ifdef USE_ARDUINO_STRING
      unsigned char decimalPlaces = 0;
      double x = data.number;
      while (int(x) != x) {
        x *= 10;
        decimalPlaces++;
      }
      String s(data.number, decimalPlaces);
      s.trim();
      return s;
#else
      TEXT t = std::to_string(data.number);
      t.erase(t.find_last_not_of('0') + 1, std::string::npos);
      t.pop_back();
      return t;
#endif
#else
#ifdef USE_ARDUINO_STRING
      unsigned char decimalPlaces = 0;
      double x = data.smallNumber;
      while (int(x) != x) {
        x *= 10;
        decimalPlaces++;
      }
      String s(data.smallNumber, decimalPlaces);
      s.trim();
      return s;
#else
      TEXT t = std::to_string(data.smallNumber);
      t.erase(t.find_last_not_of('0') + 1, std::string::npos);
      t.pop_back();
      return t;
#endif
#endif
#ifndef USE_DOUBLE
    } else if (type == Types::BigNumber) {
#ifndef USE_ARDUINO_STRING
      std::ostringstream s;
      s << std::setprecision(500) << *data.number;
      return s.str();
#else
      return data.number->toString();
#endif
#endif
    } else if (type == Types::Text) {
      return *data.text;
    } else if (type == Types::True) {
      return "True";
    } else if (type == Types::False) {
      return "False";
    } else if (type == Types::Null) {
      return "null";
    } else if (type == Types::Array) {
#ifdef USE_ARDUINO_ARRAY
      String s = "[";
      for (int i = 0; i < data.array->size(); i++) {
        s += (*data.array)[i]->toString();
        if (i != data.array->size() - 1) s += ", ";
      }
      return s + "]";
#elif !defined(USE_ARDUINO_STRING)
      std::ostringstream s;
      s << '[';
      if (!data.array->empty()) {
        for (int i = 0; i < data.array->size(); i++) {
          s << (*data.array)[i].toString();
          if (i != data.array->size() - 1) s << ", ";
        }
      }
      s << ']';
      return s.str();
#else
      String s = "[";
      for (int i = 0; i < data.array->size(); i++) {
        s += (*data.array)[i].toString();
        if (i != data.array->size() - 1) s += ", ";
      }
      return s + "]";
#endif
    } else if (type == Types::Map) {
#ifndef USE_NOSTD_MAP
      std::ostringstream s;
      s << '{';
      std::unordered_map<Value, Value, HashFunction>::iterator it;
      size_t size = std::distance((*data.map).begin(), (*data.map).end()) - 1;
      for (it = (*data.map).begin(); it != (*data.map).end(); it++) {
        s << it->first.toString() << " = " << it->second.toString();
        if (std::distance((*data.map).begin(), it) != size) s << ", ";
      }
      s << '}';
      return s.str();
#else
      String s = "{";
      for (int i = 0; i < data.map->size(); i++) {
        s += (*data.map)[i].key->toString() + " = " + (*data.map)[i].value->toString();
        if (i != data.map->size() - 1) s += ", ";
      }
      return s + "}";
#endif
    }
    return "";
  }

  bool operator== (const Value& other) const {
#ifndef USE_DOUBLE
    if (type == Types::Number && other.type == Types::BigNumber) {
      return *other.data.number == data.smallNumber;
    } else if (type == Types::BigNumber && other.type == Types::Number) {
      return *data.number == other.data.smallNumber;
    }
#endif
    if (other.type == type) {
      if (type == Types::Number) {
#ifdef USE_DOUBLE
        return data.number == other.data.number;
#else
        return data.smallNumber == other.data.smallNumber;
      } else if (type == Types::BigNumber) {
        return *data.number == *other.data.number;
#endif
      }
      if (type == Types::Text) {
        return *data.text == *other.data.text;
      } else if (type == Types::True || type == Types::False || type == Types::Null) {
        return true;
      } else if (type == Types::Array) {
#ifdef USE_ARDUINO_ARRAY
        if (data.array->size() == other.data.array->size()) {
          for (size_t i = 0; i < data.array->size(); i++) {
            if (*(*data.array)[i] != *(*other.data.array)[i]) return false;
          }
          return true;
        }
#else
        return std::equal(data.array->begin(), data.array->end(), other.data.array->begin());
#endif
      } else if (type == Types::Map) {
#ifdef USE_NOSTD_MAP
        if (data.map->size() == other.data.map->size()) {
          for (size_t i = 0; i < data.map->size(); i++) {
            if (*(*data.map)[i].value != other.get((*data.map)[i].key)) return false;
          }
          return true;
        }
#else
        return *data.map == *other.data.map;
#endif
      }
    }
    return false;
  }

  bool operator!= (const Value& other) const {
    return !(*this == other);
  }

  bool operator> (const Value& other) const {
    if (other.type == Types::Number && type == Types::Number) {
#ifdef USE_DOUBLE
      return data.number > data.number;
#else
      return data.smallNumber > other.data.smallNumber;
#endif
    }
#ifndef USE_DOUBLE
    else if (other.type == Types::BigNumber && type == Types::BigNumber) {
      return *data.number > *other.data.number;
    } else if (type == Types::BigNumber && other.type == Types::Number) {
      return *data.number > other.data.smallNumber;
    } else if (type == Types::Number && other.type == Types::BigNumber) {
      return data.smallNumber > *other.data.number;
    }
#endif
    return false;
  }

  bool operator< (const Value& other) const {
    if (other.type == Types::Number && type == Types::Number) {
#ifdef USE_DOUBLE
      return data.number < data.number;
#else
      return data.smallNumber < other.data.smallNumber;
#endif
    }
#ifndef USE_DOUBLE
    else if (other.type == Types::BigNumber && type == Types::BigNumber) {
      return *data.number < *other.data.number;
    } else if (type == Types::BigNumber && other.type == Types::Number) {
      return *data.number < other.data.smallNumber;
    } else if (type == Types::Number && other.type == Types::BigNumber) {
      return data.smallNumber < *other.data.number;
    }
#endif
    return false;
  }

  bool operator<= (const Value& other) const {
    if (other.type == Types::Number && type == Types::Number) {
#ifdef USE_DOUBLE
      return data.number <= data.number;
#else
      return data.smallNumber <= other.data.smallNumber;
#endif
    }
#ifndef USE_DOUBLE
    else if (other.type == Types::BigNumber && type == Types::BigNumber) {
      return *data.number <= *other.data.number;
    } else if (type == Types::BigNumber && other.type == Types::Number) {
      return *data.number <= other.data.smallNumber;
    } else if (type == Types::Number && other.type == Types::BigNumber) {
      return data.smallNumber <= *other.data.number;
    }
#endif
    return false;
  }

  bool operator>= (const Value& other) const {
    if (other.type == Types::Number && type == Types::Number) {
#ifdef USE_DOUBLE
      return data.number >= data.number;
#else
      return data.smallNumber >= other.data.smallNumber;
#endif
    }
#ifndef USE_DOUBLE
    else if (other.type == Types::BigNumber && type == Types::BigNumber) {
      return *data.number >= *other.data.number;
    } else if (type == Types::BigNumber && other.type == Types::Number) {
      return *data.number >= other.data.smallNumber;
    } else if (type == Types::Number && other.type == Types::BigNumber) {
      return data.smallNumber >= *other.data.number;
    }
#endif
    return false;
  }

  bool operator! () const {
    if ((type == Types::Number || type == Types::BigNumber) && toLong() == 0)
      return true;
    if (type == Types::False) {
      return true;
    }
    return false;
  }

  double toDouble() const {
    if (type == Types::Number) {
#ifdef USE_DOUBLE
      return data.number;
#else
      return data.smallNumber;
#endif
    }
#ifndef USE_DOUBLE
    else if (type == Types::BigNumber) {
#ifdef USE_BIG_NUMBER
      return data.number->getDouble();
#else
      return data.number->get_d();
#endif
    }
#endif
    return 0;
  }

  long toLong() const {
    if (type == Types::Number) {
#ifdef USE_DOUBLE
      return data.number;
#else
      return data.smallNumber;
#endif
    }
#ifndef USE_DOUBLE
    else if (type == Types::BigNumber) {
#ifdef USE_BIG_NUMBER
      return data.number->toLong();
#else
      return data.number->get_si();
#endif
    }
#endif
    return 0;
  }

  long operator~ () const {
    if (type == Types::Number) {
      return ~toLong();
    }
    return 0;
  }

  Value operator- () const {
    if (type == Types::Number
#ifndef USE_DOUBLE
    || type == Types::BigNumber
#endif
    ) {
      return *this * Value(-1);
    }
    return 0;
  }

  inline operator bool() const {
    if (type == Types::True) return true;
    return false;
  }

  inline operator int() const {
    return toLong();
  }

  inline operator long() const {
    return toLong();
  }

  inline operator double() const {
    return toDouble();
  }

  inline bool equals(const Value& v) const {
    return *this == v;
  }

  bool looksEqual(const Value& v) const {
    return toString() == v.toString();
  }

  void sort() {
    modify_linked()
    if (type == Types::Array) {
#ifdef USE_ARDUINO_STRING
      qsort(data.array->data(), data.array->size(), sizeof(Value*), compareValue);
#else
      std::sort(data.array->begin(), data.array->end(), [=] (const Value& l, const Value& r) {
        return l.toString() < r.toString();
      });
#endif
    }
  }

  void numericSort() {
    modify_linked()
    if (type == Types::Array) {
#ifdef USE_ARDUINO_STRING
      qsort(data.array->data(), data.array->size(), sizeof(Value*), compareValueNumeric);
#else
      std::sort(data.array->begin(), data.array->end(), [=] (const Value& l, const Value& r) {
        if (l.type == Types::Number &&   r.type == Types::Number) {
          return l < r;
        } else {
          return false;
        }
      });
#endif
    }
  }

  void reverse() {
    modify_linked()
    if (type == Types::Array) {
#ifdef USE_ARDUINO_ARRAY
      size_t s = data.array->size();
      if (s > 1) {
        Value* tmp;
        for (int i = 0; i < s / 2; i++) {
          tmp = (*data.array)[i];
          (*data.array)[i] = (*data.array)[s - (i + 1)];
          (*data.array)[s - (i + 1)] = tmp;
        }
      }
#else
      std::reverse(data.array->begin(), data.array->end());
#endif
    } else if (type == Types::Text) {
#ifdef USE_ARDUINO_ARRAY
      size_t s = data.text->length();
      if (s > 1) {
        Value* tmp;
        for (int i = 0; i < s / 2; i++) {
          tmp = (*data.text)[i];
          (*data.text)[i] = (*data.text)[s - (i + 1)];
          (*data.text)[s - (i + 1)] = tmp;
        }
      }
#else
      std::reverse(data.text->begin(), data.text->end());
#endif
    }
  }

  void replace(Value a, Value b) {
    modify_linked()
    if (type == Types::Text) {
#ifndef USE_ARDUINO_STRING
      TEXT t = a.toString();
      int i = data.text->find(t);
      if (i != -1) data.text->replace(i, t.length(), b.toString());
#else
      data.text->replace(a.toString(), b.toString());
#endif
    }
  }

  int indexOf(Value v, int index = 0) const {
    if (type == Types::Text) {
#ifdef USE_ARDUINO_STRING
      return data.text->indexOf(v.toString(), index);
#else
      return data.text->find(v.toString(), index);
#endif
    } else if (type == Types::Array) {
#ifdef USE_ARDUINO_ARRAY
      for (int i = index; i < data.array->size(); i++) {
        if (*(*data.array)[i] == v) {
          return i;
        }
      }
#else
      auto it = std::find(data.array->begin() + index, data.array->end(), v);
      if (it != data.array->end()) {
        return it - data.array->begin();
      }
#endif
    }
    return -1;
  }

  int lastIndexOf(Value v, int index) const {
    if (type == Types::Text) {
#ifdef USE_ARDUINO_STRING
      return data.text->lastIndexOf(v.toString(), index);
#else
      return data.text->find_last_of(v.toString(), index);
#endif
    } else if (type == Types::Array) {
      for (int i = index; i >= 0; i--) {
#ifdef USE_ARDUINO_ARRAY
        if (*(*data.array)[i] == v) {
#else
        if ((*data.array)[i] == v) {
#endif
          return i;
        }
      }
    }
    return -1;
  }

  int lastIndexOf(Value v) const {
    if (type == Types::Text) {
#ifdef USE_ARDUINO_STRING
      return data.text->lastIndexOf(v.toString());
#else
      return data.text->find_last_of(v.toString());
#endif
    } else if (type == Types::Array) {
      for (int i = data.array->size(); i >= 0; i--) {
#ifdef USE_ARDUINO_ARRAY
        if (*(*data.array)[i] == v) {
#else
        if ((*data.array)[i] == v) {
#endif
          return i;
        }
      }
    }
    return -1;
  }

  Value substring(const Value& other) const {
#ifdef USE_ARDUINO_STRING
    return data.text->substring((long) other);
#else
    return data.text->substr((long) other);
#endif
  }

  Value substring(Value v1, Value v2) const {
#ifdef USE_ARDUINO_STRING
    return data.text->substring((long) v1, (long) v2);
#else
    return data.text->substr((long) v1, (long) v2 - (long) v1);
#endif
  }

  inline int length() const {
    if (type == Types::Text) {
#ifdef USE_ARDUINO_STRING
      return data.text->length();
#else
      return data.text->size();
#endif
    } else if (type == Types::Array) {
      return data.array->size();
    } else if (type == Types::Map) {
      return data.map->size();
    }
    return 0;
  }

  Value split(Value d) const {
    Value res = Types::Array;
    if (type == Types::Text) {
      int start = 0;
      int end = indexOf(d);
      while (end != -1) {
        res.append(substring(start, end));
        start = end + d.length();
        end = indexOf(d, start);
      }
      res.append(substring(start, end));
    }
    return res;
  }

  Value trimRight() const {
    if (type == Types::Text) {
      int i = length() - 1;
      while ((*data.text)[i] == '\n' || (*data.text)[i] == '\t' || (*data.text)[i] == ' ') i--;
      return substring(0, i + 1);
    }
    return "";
  }

  Value trimLeft() const {
    if (type == Types::Text) {
      int i = 0;
      while ((*data.text)[i] == '\n' || (*data.text)[i] == '\t' || (*data.text)[i] == ' ') i++;
      return substring(i);
    }
    return "";
  }

  Value trim() const {
    if (type == Types::Text) {
      return trimLeft().trimRight();
    }
    return "";
  }

  Value toUpper() const {
    TEXT t = *data.text;
    for (size_t c = 0; c < length(); c++) {
      if (t[c] > 96 && t[c] < 123) {
        t[c] -= 32;
      }
    }
    return t;
  }

  Value toLower() {
    TEXT t = *data.text;
    for (size_t c = 0; c < length(); c++) {
      if (t[c] < 91 && t[c] > 64) {
        t[c] += 32;
      }
    }
    return t;
  }

  bool startsWith(const Value& v) const {
#ifdef USE_ARDUINO_STRING
    return data.text->startsWith(v.toString());
#else
    return data.text->find(v.toString()) == 0;
#endif
  }

  bool endsWith(const Value& v) const {
#ifdef USE_ARDUINO_STRING
    return data.text->endsWith(v.toString());
#else
    TEXT t = v.toString();
    int i = lastIndexOf(t);
    return i == data.text->size() - t.size() || i == -1;
#endif
  }

  int codePointAt(const Value& at) const {
    if (type == Types::Text)
      return (int) (*data.text)[(long) at];
    return -1;
  }

  char charAt(const Value& at) const {
    if (type == Types::Text)
      return (*data.text)[(long) at];
    return 0;
  }

  Value& operator[] (const Value& i) const {
    if (type == Types::Array) {
#ifdef USE_ARDUINO_ARRAY
      return *(*data.array)[(long) i];
#else
      return (*data.array)[(long) i];
#endif
    } else if (type == Types::Map) {
      return get(i);
    }
  }

  void toNumber() {
    modify_linked()
    if (type == Types::Text) {
      TEXT t = toString();
      freeUnusedMemory();
#ifndef USE_DOUBLE
      if (t.length() < 16) {
        type = Types::Number;
        data.smallNumber = atof(t.c_str());
      } else {
        type = Types::BigNumber;
        data.number = new NUMBER(t);
      }
#else
      data.number = NUMBER_FROM_STRING(t.c_str());
      type = Types::Number;
#endif
    } else if (type == Types::True) {
#ifndef USE_DOUBLE
    data.smallNumber = 1;
#else
    data.number = 1;
#endif
      type = Types::Number;
    } else if (type == Types::False || type == Types::Null) {
#ifndef USE_DOUBLE
    data.smallNumber = 0;
#else
    data.number = 0;
#endif
      type = Types::Number;
    }
  }

  Value operator&=(const Value& other) {
    modify_linked()
    if ((type == Types::True || type == Types::False) &&
            (other.type == Types::True || other.type == Types::False)) {
      *this = *this && other;
    } else {
      long a, b;
      if (type == Types::True || type == Types::False) {
        a = (int) ((bool) *this);
      } else a = toLong();
      if (other.type == Types::True || other.type == Types::False) {
        b = (int) ((bool) other);
      } else b = other.toLong();
      *this = a & b;
    }
    return this;
  }

  Value operator&(const Value& other) const {
    Value v = *this;
    v &= other;
    return v;
  }

  Value operator|=(const Value& other) {
    modify_linked()
    if ((type == Types::True || type == Types::False) &&
            (other.type == Types::True || other.type == Types::False)) {
      *this = *this || other;
    } else {
      long a, b;
      if (type == Types::True || type == Types::False) {
        a = (int) ((bool) *this);
      } else a = toLong();
      if (other.type == Types::True || other.type == Types::False) {
        b = (int) ((bool) other);
      } else b = other.toLong();
      *this = a | b;
    }
    return this;
  }

  Value operator|(const Value& other) const {
    Value v = *this;
    v |= other;
    return v;
  }

  Value operator<<=(const Value& other) {
    modify_linked()
    if (IS_NUM(other) && (type == Types::Number || type == Types::BigNumber)) {
      *this = toLong() << other.toLong();
    }
    return this;
  }

  Value operator<<(const Value& other) const {
    Value v = *this;
    v <<= other;
    return v;
  }

  Value operator>>=(const Value& other) {
    modify_linked()
    if (IS_NUM(other) && (type == Types::Number || type == Types::BigNumber)) {
      *this = toLong() >> other.toLong();
    }
    return this;
  }

  Value operator>>(const Value& other) const {
    Value v = *this;
    v >>= other;
    return v;
  }

  Value operator^=(const Value& other) {
    modify_linked()
    if (IS_NUM(other) && (type == Types::Number || type == Types::BigNumber)) {
      *this = toLong() ^ other.toLong();
    }
    return this;
  }

  Value operator^(const Value& other) const {
    Value v = *this;
    v ^= other;
    return v;
  }

  Value operator+=(const Value& other) {
    modify_linked()
#ifndef USE_DOUBLE
    if (type == Types::BigNumber && other.type == Types::BigNumber) {
      *data.number += *other.data.number;
    } else if (type == Types::Number && other.type == Types::Number) {
      if (isinf(data.smallNumber + other.data.smallNumber)) {
        type = Types::BigNumber;
        data.number = new NUMBER(data.smallNumber);
        *data.number += other.data.smallNumber;
      } else {
        data.smallNumber += other.data.smallNumber;
      }
    } else if (type == Types::BigNumber && other.type == Types::Number) {
      *data.number += other.data.smallNumber;
    } else if (type == Types::Number && other.type == Types::BigNumber) {
      type = Types::BigNumber;
      data.number = new NUMBER(data.smallNumber);
      *data.number += *other.data.number;
#else
    if (type == Types::Number && other.type == Types::Number) {
      data.number += other.data.number;
#endif
    } else if (type == Types::Text || other.type == Types::Text) { // If either a or b is text
      if (type == Types::Text) {
        *data.text += other.toString();
      } else {
        _release_value()
        useCount = new USE_COUNT_TYPE; *useCount = 0;
        auto temp = data;
        data.text = new TEXT(toString() + other.toString());
#ifndef USE_DOUBLE
        if (type == Types::BigNumber) delete temp.number;
#endif
        if (type == Types::Array) delete temp.array;
        if (type == Types::Map) delete temp.map;
        type = Types::Text;
      }
    }
    return this;
  }

  Value operator+(const Value& other) const {
    Value v = *this;
    v += other;
    return v;
  }

  Value operator-=(const Value& other) {
    modify_linked()
    if (type == Types::Number && other.type == Types::Number) {
#ifdef USE_DOUBLE
      data.number -= other.data.number;
#else
      data.smallNumber -= other.data.smallNumber;
#endif
#ifndef USE_DOUBLE
    } else if (type == Types::BigNumber && other.type == Types::BigNumber) {
      *data.number -= *other.data.number;
    } else if (type == Types::BigNumber && other.type == Types::Number) {
      *data.number -= other.data.smallNumber;
    } else if (type == Types::Number && other.type == Types::BigNumber) {
      type = Types::BigNumber;
      data.number = new NUMBER(data.smallNumber);
      *data.number -= *other.data.number;
#endif
    } else if (type == Types::Text || other.type == Types::Text) { // If either a or b is text
      if (type == Types::Text) {
        *data.text = toString();
      } else {
        TEXT* t = new TEXT(toString());
        freeUnusedMemory();
        data.text = t;
      }
#ifndef USE_ARDUINO_STRING
      TEXT t = other.toString();
      int i = data.text->find(t);
      if (i != -1) data.text->replace(i, t.length(), "");
#else
      data.text->replace(other.toString(), "");
#endif
      type = Types::Text;
    }
    return this;
  }

  Value operator-(const Value& other) const {
    Value v = *this;
    v -= other;
    return v;
  }

  Value operator*=(const Value& other) {
    modify_linked()
    if (type == Types::Number && other.type == Types::Number) {
#ifdef USE_DOUBLE
      data.number *= other.data.number;
#else
      double d = other.toDouble();
      if (isinf(data.smallNumber * d)) {
        type = Types::BigNumber;
        data.number = new NUMBER(data.smallNumber);
        *data.number *= d;
      } else {
        data.smallNumber *= d;
      }
    } else if (type == Types::BigNumber && other.type == Types::BigNumber) {
      *data.number *= *other.data.number;
    } else if (type == Types::BigNumber && other.type == Types::Number) {
      *data.number *= other.data.smallNumber;
    } else if (type == Types::Number && other.type == Types::BigNumber) {
      type = Types::BigNumber;
      data.number = new NUMBER(data.smallNumber);
      *data.number *= *other.data.number;
#endif
    } else if (type == Types::Text && other.type == Types::BigNumber) {
#ifndef USE_ARDUINO_STRING
      std::ostringstream os;
      for (NUMBER i = 0; i < 
#ifndef USE_DOUBLE
      *other.data.number
#else
      other.data.number
#endif
      ; i++) {
        os << toString();
      }
      *data.text = os.str();
#else
      String s;
      for (NUMBER i = 0; i < 
#ifndef USE_DOUBLE
      *other.data.number
#else
      other.data.number
#endif
      ; i++) {
        s += toString();
      }
      *data.text = s;
#endif
    } else if (type == Types::BigNumber && other.type == Types::Text) {
#ifndef USE_ARDUINO_STRING
      std::ostringstream os;
      for (NUMBER i = 0; i < 
#ifndef USE_DOUBLE
      *data.number
#else
      data.number
#endif
      ; i++) {
        os << other.toString();
      }
      freeUnusedMemory();
      type = Types::Text;
      data.text = new TEXT(os.str());
#else
      String s;
      for (NUMBER i = 0; i < 
#ifndef USE_DOUBLE
      *data.number
#else
      data.number
#endif
      ; i++) {
        s += other.toString();
      }
      freeUnusedMemory();
      type = Types::Text;
      data.text = new TEXT(s);
#endif
    } else if (type == Types::Text && other.type == Types::Number) {
#ifndef USE_ARDUINO_STRING
      std::ostringstream os;
      for (long i = 0; i < 
#ifndef USE_DOUBLE
      other.data.smallNumber
#else
      other.data.number
#endif
      ; i++) {
        os << toString();
      }
      *data.text = os.str();
#else
      String s;
      for (long i = 0; i < 
#ifndef USE_DOUBLE
      other.data.smallNumber
#else
      other.data.number
#endif
      ; i++) {
        s += toString();
      }
      *data.text = s;
#endif
    } else if (type == Types::BigNumber && other.type == Types::Text) {
#ifndef USE_ARDUINO_STRING
      std::ostringstream os;
      for (NUMBER i = 0; i < 
#ifndef USE_DOUBLE
      *data.number
#else
      data.number
#endif
      ; i++) {
        os << other.toString();
      }
      freeUnusedMemory();
      type = Types::Text;
      data.text = new TEXT(os.str());
#else
      String s;
      for (NUMBER i = 0; i < 
#ifndef USE_DOUBLE
      *data.number
#else
      data.number
#endif
      ; i++) {
        s += other.toString();
      }
      freeUnusedMemory();
      type = Types::Text;
      data.text = new TEXT(s);
#endif
    }
    return this;
  }

  Value operator*(const Value& other) const {
    Value v = *this;
    v *= other;
    return v;
  }

  Value operator/=(const Value& other) {
    modify_linked()
    if (type == Types::Number && other.type == Types::Number) {
#ifdef USE_DOUBLE
      data.number /= other.data.number;
#else
      data.smallNumber /= other.data.smallNumber;
    } else if (type == Types::BigNumber && other.type == Types::BigNumber) {
      *data.number /= *other.data.number;
    } else if (type == Types::BigNumber && other.type == Types::Number) {
      *data.number /= other.data.smallNumber;
    } else if (type == Types::Number && other.type == Types::BigNumber) {
      type = Types::BigNumber;
      data.number = new NUMBER(data.smallNumber);
      *data.number /= *other.data.number;
#endif
    } else {
      *this = 0;
    }
    return this;
  }

  Value operator/(const Value& other) const {
    Value v = *this;
    v /= other;
    return v;
  }

  Value operator%=(const Value& other) {
    modify_linked()
    if (type == Types::Number && other.type == Types::Number) {
#ifdef USE_DOUBLE
      data.number = (long) data.number % (long) other.data.number;
#else
      data.smallNumber = (long) data.smallNumber % (long) other.data.smallNumber;
    } else if (type == Types::BigNumber && other.type == Types::BigNumber) {
#ifndef USE_BIG_NUMBER
      *data.number = floor(*data.number);
      mpz_class t(toString().c_str());
      t %= *other.data.number;
      *data.number = t;
#else
      *data.number = *data.number % *other.data.number;
#endif
    } else if (type == Types::BigNumber && other.type == Types::Number) {
#ifndef USE_BIG_NUMBER
      *data.number = floor(*data.number);
      mpz_class t(toString().c_str());
      t %= other.data.smallNumber;
      *data.number = t;
#else
      *data.number = *data.number % other.data.smallNumber;
#endif
    } else if (type == Types::Number && other.type == Types::BigNumber) {
      data.smallNumber = (long) data.smallNumber % (long) other;
#endif
    } else {
      *this = 0;
    }
    return this;
  }

  Value operator%(const Value& other) const {
    Value v = *this;
    v %= other;
    return v;
  }

  Value operator++(int) {
    Value tmp = this;
    if (type == Types::Number) {
#ifdef USE_DOUBLE
      data.number ++;
#else
      data.smallNumber ++;
    } else if (type == Types::BigNumber) {
      *data.number += 1;
#endif
    }
    return tmp;
  }

  Value operator++() {
    if (type == Types::Number) {
#ifdef USE_DOUBLE
      data.number ++;
#else
      data.smallNumber ++;
    } else if (type == Types::BigNumber) {
      *data.number += 1;
#endif
    }
    return this;
  }

  Value operator--(int) {
    Value tmp = this;
    if (type == Types::Number) {
#ifdef USE_DOUBLE
      data.number --;
#else
      data.smallNumber --;
    } else if (type == Types::BigNumber) {
      *data.number -= 1;
#endif
    }
    return tmp;
  }

  Value operator--() {
    if (type == Types::Number) {
#ifdef USE_DOUBLE
      data.number --;
#else
      data.smallNumber --;
    } else if (type == Types::BigNumber) {
      *data.number -= 1;
#endif
    }
    return this;
  }

  void pow(const Value& other) {
    modify_linked()
    if (type == Types::Number && other.type == Types::Number) {
#ifdef USE_DOUBLE
      data.number = ::pow(data.number, other.data.number);
#else
      data.smallNumber = ::pow(data.smallNumber, other.data.smallNumber);
    } else if (type == Types::BigNumber && IS_NUM(other)) {
#ifndef USE_BIG_NUMBER
      mpf_pow_ui(data.number->get_mpf_t(), data.number->get_mpf_t(), (long) other);
#else
			*data.number = data.number->pow((long) other);
#endif
#endif
    }
  }
};


#ifndef USE_NOSTD_MAP
inline size_t HashFunction::operator() (const Value& v) const {
  return (std::hash<std::string>() (v.toString())) ^
          (std::hash<int>()((int) v.getType()));
}
#else
inline Pair& Pair::operator= (const Pair& p) {
  this->key = p.key;
  this->value = p.value;
  return *this;
}
#endif

#ifdef USE_ARDUINO_STRING
inline int compareValue(const void *cmp1, const void *cmp2) {
  Value* a = *((Value **) cmp1);
  Value* b = *((Value **) cmp2);
  return a->toString().compareTo(b->toString());
}

inline int compareValueNumeric(const void *cmp1, const void *cmp2) {
  Value* a = *((Value **) cmp1);
  Value* b = *((Value **) cmp2);
  return (int) (*a - *b);
}
#endif
#undef modify_linked
#undef _release_value
#endif // VALUE_H