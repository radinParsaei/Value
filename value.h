#ifdef USE_BIG_NUMBER
#include <BigNumber.h>
#define NUMBER BigNumber
#else
#include <gmpxx.h>
#include <sstream>
#include <iomanip>
#define NUMBER mpf_class
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

class Value;

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

enum class Types { Null = 0, True, False, Number, Text, Array, Map };

#ifndef MAX_FIXED_MAP_SIZE
#define MAX_FIXED_MAP_SIZE MAX_FIXED_ARRAY_SIZE
#endif

class Value {
private:
  Value(Value* v) {
    this->data = v->data;
    this->type = v->type;
    freeMemory = false;
  }
  union {
    NUMBER* number;
    TEXT* text;
    ARRAY* array;
#ifdef USE_NOSTD_MAP
    Array<Pair, MAX_FIXED_MAP_SIZE>* map;
#else
    std::unordered_map<Value, Value, HashFunction>* map;
#endif
  } data;
  Types type = Types::Null;
  bool freeMemory = true;
public:
  void clone() {
    if (type == Types::Text) {
      TEXT* t = new TEXT(*data.text);
      data.text = t;
    } else if (type == Types::Number) {
      NUMBER* t = new NUMBER(*data.number);
      data.number = t;
    } else if (type == Types::Array) {
      ARRAY* t = new ARRAY(*data.array);
      data.array = t;
    } else if (type == Types::Map) {
#ifndef USE_NOSTD_MAP
      std::unordered_map<Value, Value, HashFunction>* t = new std::unordered_map<Value, Value, HashFunction>();
      *t = *data.map;
      data.map = t;
#else
      ARRAY* t = new ARRAY(*data.array);
      data.array = t;
#endif
    }
  }

  // free unused pointers
  void freeUnusedMemory() {
    if (type == Types::Text && data.text != 0) {
      delete data.text;
      data.text = 0;
    } else if (type == Types::Number && data.number != 0) {
      delete data.number;
      data.number = 0;
    } else if (type == Types::Array && data.array != 0) {
      delete data.array;
      data.array = 0;
    } else if (type == Types::Map && data.map != 0) {
      delete data.map;
      data.map = 0;
    }
  }
  Value () { type = Types::Null; }
  Value (Types t) {
    if (t == Types::Array) {
      data.array = new ARRAY();
    } else if (t == Types::Map) {
#ifdef USE_NOSTD_MAP
      data.map = new Array<Pair, MAX_FIXED_MAP_SIZE>();
#else
      data.map = new std::unordered_map<Value, Value, HashFunction>();
#endif
    }
    type = t;
  }
  Value (NUMBER n) {
    this->data.number = &n;
    type = Types::Number;
  }
  Value (int n) {
    this->data.number = new NUMBER(n);
    type = Types::Number;
  }
  Value (double n) {
    this->data.number = new NUMBER(n);
    type = Types::Number;
  }
  Value (long n) {
    this->data.number = new NUMBER(n);
    type = Types::Number;
  }
  Value (TEXT s) {
    data.text = &s;
    type = Types::Text;
  }
  Value (const char* s) {
    data.text = new TEXT(s);
    type = Types::Text;
  }
  Value (const Value& v) {
    data = v.data;
    type = v.type;
    clone();
  }
  // free unused pointers when the object is destructing
  ~Value () {
    if (freeMemory) freeUnusedMemory();
  }

  void operator= (const Value& v) {
    freeUnusedMemory();
    data = v.data;
    type = v.type;
    clone();
  }

  void operator= (Types t) {
    freeUnusedMemory();
    if (t == Types::Array) {
      data.array = new ARRAY();
    } else if (t == Types::Map) {
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
    this->data.number = new NUMBER(n);
    type = Types::Number;
  }

  void operator= (long n) {
    freeUnusedMemory();
    this->data.number = new NUMBER(n);
    type = Types::Number;
  }

  void operator= (double n) {
    freeUnusedMemory();
    this->data.number = new NUMBER(n);
    type = Types::Number;
  }

  void operator= (TEXT t) {
    freeUnusedMemory();
    this->data.text = &t;
    type = Types::Text;
  }

  void operator= (const char* t) {
    freeUnusedMemory();
    this->data.text = new TEXT(t);
    type = Types::Text;
  }

  void append(Value v) {
#ifdef USE_ARDUINO_ARRAY
    Value* value = new Value();
    *value = v;
    data.array->push_back(value);
#else
    data.array->push_back(v);
#endif
  }

  Types getType() const {
    return type;
  }

  void put(Value k, Value v) {
    if (type == Types::Map) {
#ifndef USE_NOSTD_MAP
      auto it = data.map->find(k);
      if (it != data.map->end()) {
        it->second = v;
      } else {
        data.map->insert(std::pair<Value, Value>(k, v));
      }
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

  Value get(Value k) {
    if (type == Types::Map) {
#ifdef USE_NOSTD_MAP
      for (int i = 0; i < data.map->size(); i++) {
        if (*(*data.map)[i].key == k) {
          return (*data.map)[i].value;
        }
      }
      return Types::Null;
#else
      return (*data.map)[k];
#endif
    } else {
      return Types::Null;
    }
  }

  TEXT toString() const {
    if (type == Types::Number) {
#ifndef USE_ARDUINO_STRING
      std::ostringstream s;
      s << std::setprecision(500) << *data.number;
      return s.str();
#else
      return data.number->toString();
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
    if (other.type == type) {
      if (type == Types::Text) {
        return *data.text == *other.data.text;
      } else if (type == Types::Number) {
        return *data.number == *other.data.number;
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
      return *data.number > *other.data.number;
    }
    return false;
  }

  bool operator< (const Value& other) const {
    if (other.type == Types::Number && type == Types::Number) {
      return *data.number < *other.data.number;
    }
    return false;
  }

  Value operator! () const {
    if (type == Types::True) {
      return Types::False;
    } else if (type == Types::False) {
      return Types::True;
    }
    return Types::False;
  }

  double toDouble() const {
#ifndef USE_BIG_NUMBER
			return data.number->get_d();
#else
			return data.number->toDouble();
#endif
  }

  long toLong() const {
#ifndef USE_BIG_NUMBER
      return data.number->get_si();
#else
      return data.number->toLong();
#endif
  }

  long operator~ () const {
    if (type == Types::Number) {
      return ~toLong();
    }
    return 0;
  }

  inline operator int() {
    return toLong();
  }

  inline operator long() {
    return toLong();
  }

  inline operator double() {
    return toDouble();
  }

  inline bool equals(const Value& v) {
    return *this == v;
  }

  bool looksEqual(const Value& v) const {
    return toString() == v.toString();
  }

  Value operator+=(Value other) {
    if (type == Types::Number && other.type == Types::Number) {
      *data.number += *other.data.number;
    } else if (type == Types::Text || other.type == Types::Text) { // If either a or b is text
      auto temp = data;
      if (type == Types::Text)
        *data.text += other.toString();
      else
        data.text = new TEXT(toString() + other.toString());
      if (type == Types::Number) delete temp.number;
      if (type == Types::Array) delete temp.array;
      if (type == Types::Map) delete temp.map;
      type = Types::Text;
    }
    return this;
  }

  Value operator+(Value other) {
    Value v = *this;
    v += other;
    return v;
  }

  Value operator-=(Value other) {
    if (type == Types::Number && other.type == Types::Number) {
      *data.number -= *other.data.number;
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

  Value operator-(Value other) {
    Value v = *this;
    v -= other;
    return v;
  }

  Value operator*=(Value other) {
    if (type == Types::Number && other.type == Types::Number) {
      *data.number *= *other.data.number;
    } else if (type == Types::Text && other.type == Types::Number) {
#ifndef USE_ARDUINO_STRING
      std::ostringstream os;
      for (NUMBER i = 0; i < *other.data.number; i++) {
        os << toString();
      }
      *data.text = os.str();
#else
      String s;
      for (NUMBER i = 0; i < *other.data.number; i++) {
        s += toString();
      }
      *data.text = s;
#endif
    } else if (type == Types::Number && other.type == Types::Text) {
#ifndef USE_ARDUINO_STRING
      std::ostringstream os;
      for (NUMBER i = 0; i < *data.number; i++) {
        os << other.toString();
      }
      freeUnusedMemory();
      type = Types::Text;
      data.text = new TEXT(os.str());
#else
      String s;
      for (NUMBER i = 0; i < *data.number; i++) {
        s += other.toString();
      }
      freeUnusedMemory();
      type = Types::Text;
      data.text = new TEXT(s);
#endif
    }
    return this;
  }

  Value operator*(Value other) {
    Value v = *this;
    v *= other;
    return v;
  }

  Value operator/=(Value other) {
    if (type == Types::Number && other.type == Types::Number) {
      *data.number /= *other.data.number;
    } else {
      *this = 0;
    }
    return this;
  }

  Value operator/(Value other) {
    Value v = *this;
    v /= other;
    return v;
  }

  Value operator%=(Value other) {
    if (type == Types::Number && other.type == Types::Number) {
#ifndef USE_BIG_NUMBER
      *data.number = floor(*data.number);
      mpz_class t(toString().c_str());
      t %= *other.data.number;
      *data.number = t;
#else
      *data.number = *data.number % *other.data.number;
#endif
    } else {
      *this = 0;
    }
    return this;
  }

  Value operator%(Value other) {
    Value v = *this;
    v %= other;
    return v;
  }

  Value operator++(int) {
    Value tmp = this;
    if (type == Types::Number) {
      *data.number += 1;
    }
    return tmp;
  }

  Value operator++() {
    if (type == Types::Number) {
      *data.number += 1;
    }
    return this;
  }

  void pow(Value other) {
    if (type == Types::Number && other.type == Types::Number) {
#ifndef USE_BIG_NUMBER
      mpf_pow_ui(data.number->get_mpf_t(), data.number->get_mpf_t(), other.data.number->get_ui());
#else
			*data.number = data.number->pow(*other.data.number);
#endif
    }
  }
};


#ifndef USE_NOSTD_MAP
size_t HashFunction::operator() (const Value& v) const {
  return (std::hash<std::string>() (v.toString())) ^
          (std::hash<int>()((int) v.getType()));
}
#else
Pair& Pair::operator= (const Pair& p) {
  this->key = p.key;
  this->value = p.value;
  return *this;
}
#endif
