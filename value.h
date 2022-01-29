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

enum class Types { Null = 0, True, False, Number, Text, Array };

class Value {
public:
  union {
    NUMBER* number;
    TEXT* text;
    ARRAY* array;
  } data;
  Types type = Types::Null;
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
    }
  }
  Value () { type = Types::Null; }
  Value (Types t) {
    if (t == Types::Array) {
      data.array = new ARRAY();
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
  ~Value () { freeUnusedMemory(); } // free unused pointers when the object is destructing
  void operator= (Value v) {
    freeUnusedMemory();
    data = v.data;
    type = v.type;
    clone();
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
  TEXT toString() {
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
    }
    return "";
  }
};

