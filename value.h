#ifndef __VALUE__H__
#define __VALUE__H__

#include <BigNumber.h>

class Value {
  bool type;
  struct {
    BigNumber number;
    char* string;
  } data;
  public:
    Value();
    Value(BigNumber data){
      type = 0;
      this->data.number = data;
      this->data.string = 0;
    }

    Value(double data) {
      type = 0;
      this->data.number = BigNumber(data);
      this->data.string = 0;
    }

    Value(int data) {
      type = 0;
      this->data.number = BigNumber(data);
      this->data.string = 0;
    }

    Value(long long data) {
      char res[25] = {'0', '0', '0', '0', '0', '0',
                        '0', '0', '0', '0', '0', '0',
                        '0', '0', '0', '0', '0', '0',
                        '0', '0', '0', '0', '0', '0'
                        , '0'};
      if (data < 0) { res[0] = '-'; data = -data; }
      uint8_t c = 0;
      while(data != 0) {
        res[23 - c++] = (data % 10) + '0';
        data /= 10;
      }
      res[24] = 0;
      *this = BigNumber(res);
    }

    Value(const char* data) {
      type = 1;
      this->data.string = strdup(data);
      this->data.number = 0;
    }

    Value& operator=(Value other) {
      type = other.type;
      this->data = other.data;
      return *this;
    }
    Value& operator=(BigNumber other) {
      type = 0;
      this->data.number = other;
      this->data.string = 0;
      return *this;
    }

    const char* toString() {
      if(type){
        return data.string;
      } else {
        return data.number.toString();
      }
    }

    bool getType() {
      return type;
    }

    BigNumber getNumber() {
      return data.number;
    }

    const char* getString() {
      return data.string;
    }
};

#ifdef STD_STREAMS
std::ostream &operator<<(std::ostream &s, Value &v) {
    return s << v.toString();
}
#endif

#endif
