#ifndef __VALUE__H__
#define __VALUE__H__

#include <BigNumber.h>

#ifdef USE_UTILS
namespace Utils {
  const char* replace(const char *replaceOn, const char *from, const char *to) {
    char *result;
    int i = 0, c = 0;
    int tolen = strlen(to);
    int fromlen = strlen(from);
    for (; replaceOn[i] != '\0'; i++) {
      if (strstr(&replaceOn[i], from) == &replaceOn[i]) {
        c++;
        i += fromlen - 1;
      }
    }
    result = (char*)malloc(i + c * (tolen - fromlen) + 1);
    i = 0;
    while (*replaceOn) {
      if (strstr(replaceOn, from) == replaceOn) {
        strcpy(&result[i], to);
        i += tolen;
        replaceOn += fromlen;
      } else {
        result[i++] = *replaceOn++;
      }
    }
    result[i] = 0;
    return result;
  }

  char* repeat(char* a, int c) {
    char tmp[strlen(a)] = {};
    strcpy(tmp, a);
    for (c--; c > 0; c--) {
      strcat(a, tmp);
    }
    return a;
  }

  char* append(char* a, char* b) {
    char tmp[strlen(a)] = {};
    strcpy(tmp, a);
    return strcat(tmp, b);
}

  bool isEQ(const char* a, const char* b) {
    int64_t i = 0;
    while (a[i] != 0) {
      if (b[i] == 0) {
        return false;
      }
      if (a[i] != b[i]) return false;
      i++;
    }
    return true;
  }
}
#endif

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

    char* toString() {
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

    char* getString() {
      return data.string;
    }

    void toNum() {
      if(!type) return;
      type = 0;
      this->data.number = BigNumber(data.string);
      this->data.string = 0;
    }

    void toTxt() {
      if(type) return;
      type = 1;
      this->data.string = this->data.number.toString();
      this->data.number = 0;
    }
};

#ifdef STD_STREAMS
std::ostream &operator<<(std::ostream &s, Value &v) {
    return s << v.toString();
}
#endif

#endif
