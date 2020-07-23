#ifndef __VALUE__H__
#define __VALUE__H__

#ifdef VALUE_MULTI_TYPE_SUPPORT
enum STATES { null = -1, VALUE_TYPE_NUMBER, VALUE_TYPE_TEXT, True, False };
#else
#define VALUE_TYPE_NUMBER 0
#define VALUE_TYPE_TEXT 1
#endif

#include <string.h>
#include <sstream>

#ifdef USE_GMP_LIB
#define NUMBER mpf_class
#include <gmpxx.h>
#include <string.h>
#include <sstream>
#include <iomanip>
#define NUMBER_TO_STRING mpf_class_to_string(this->number).c_str();
#else
#include <BigNumber.h>
#define NUMBER BigNumber
#define NUMBER_TO_STRING number.toString()
#endif

class Value {
#ifdef VALUE_MULTI_TYPE_SUPPORT
	STATES type;
#else
	bool type;
#endif
	NUMBER number;
	std::string text;
#ifdef USE_GMP_LIB
	std::string mpf_class_to_string(mpf_class data) {
		std::ostringstream s;
		int scale = 0;
		mpf_class tmp = data;
		while (tmp.get_d() != tmp.get_si()) {
			tmp /= 10;
			scale++;
		}
	  s << std::fixed << std::setprecision(scale) << data;
	  std::string str = s.str();
	  int i = str.size();
	  if (scale) {
			while(str[--i] == '0')str.pop_back();
		  if(str[str.size() - 1] == '.')str.pop_back();
		}
		return str;
	}
#endif
	public:
#ifdef VALUE_MULTI_TYPE_SUPPORT
		Value() { type = null; }
		Value(STATES a) {
			if (a == null || a == True || a == False) {
				type = a;
			} else {
				*this = (int)a;
			}
		}
		Value(bool a) {
			type = a? True:False;
		}
		bool getBool() {
#ifdef VALUE_MULTI_TYPE_SUPPORT
			return type == True? true:false;
#else
			return number != 0? true:false;
#endif
		}
#else
		Value() { type = VALUE_TYPE_NUMBER; }
#endif
		Value(NUMBER data) {
			type = VALUE_TYPE_NUMBER;
			this->number = data;
			this->text = "";
		}

		Value(char c) {
			type = VALUE_TYPE_TEXT;
			text = c;
			number = 0;
		}

		Value(double data) {
			type = VALUE_TYPE_NUMBER;
			this->number = data;
			this->text = "";
		}

		Value(int data) {
			type = VALUE_TYPE_NUMBER;
			this->number = data;
			this->text = "";
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
			*this = NUMBER(res);
		}

		Value(long data) {
			this->number = NUMBER(data);
		}

		Value(const char* data) {
			type = VALUE_TYPE_TEXT;
			this->text = data;
			this->number = 0;
		}

		Value(std::string data) {
			type = VALUE_TYPE_TEXT;
			this->text = data;
			this->number = 0;
		}

		Value(Value* other) {
			type = other->type;
			this->text = other->text;
			this->number = other->number;
		}

		Value& operator=(const char* data) {
			type = VALUE_TYPE_TEXT;
			this->text = data;
			this->number = 0;
			return *this;
		}

		Value& operator=(std::string data) {
			type = VALUE_TYPE_TEXT;
			this->text = data;
			this->number = 0;
			return *this;
		}

		Value& operator=(Value other) {
			type = other.type;
			this->text = other.text;
			this->number = other.number;
			return *this;
		}
		Value& operator=(Value* other) {
			type = other->type;
			this->text = other->text;
			this->number = other->number;
			return *this;
		}
		Value& operator=(NUMBER n) {
			type = VALUE_TYPE_NUMBER;
			this->number = n;
			this->text = "";
			return *this;
		}

		Value& operator=(int i) {
			type = VALUE_TYPE_NUMBER;
			this->number = i;
			this->text = "";
			return *this;
		}

		std::string toString() {
			if(type == VALUE_TYPE_TEXT){
				return text;
			} else
#ifdef VALUE_MULTI_TYPE_SUPPORT
			if (type == VALUE_TYPE_NUMBER)
#endif
			 {
				return NUMBER_TO_STRING;
			}
#ifdef VALUE_MULTI_TYPE_SUPPORT
			else if (type == null) {
				return "null";
			} else if (type == True) {
				return "True";
			} else {
				return "False";
			}
#endif
		}

#ifdef VALUE_MULTI_TYPE_SUPPORT
		STATES
#else
		bool
#endif
		getType() {
			return type;
		}

		NUMBER getNumber() {
			return number;
		}

		std::string getString() {
			return text;
		}

		Value canNum() {
			if (type == VALUE_TYPE_NUMBER) {
				return true;
			}
#ifdef VALUE_MULTI_TYPE_SUPPORT
			else if (type == True || type == False || type == null) {
				return true;
			}
#endif
			else {
				for (char c : text) {
					if (!isdigit(c)) return false;
				}
				return true;
			}
		}

		Value& toNum() {
			if(type == VALUE_TYPE_NUMBER) return *this;
#ifdef VALUE_MULTI_TYPE_SUPPORT
			if (type == False || type == null) {
				type = VALUE_TYPE_NUMBER;
				number = 0;
				return *this;
			} else if (type == True) {
				type = VALUE_TYPE_NUMBER;
				number = 1;
				return *this;
			}
#endif
			type = VALUE_TYPE_NUMBER;
#ifdef USE_GMP_LIB
			size_t i = 0;
			for(; text[i] != 0; i++){
				if(!isdigit(text[i]) && text[i] != '-' && text[i] != '.') {
					this->number = 0;
					this->text = "";
					return *this;
				}
			}
#endif
			this->number = NUMBER(text.c_str());
			this->text = "";
			return *this;
		}

		Value& toTxt() {
			if(type) return *this;
			type = VALUE_TYPE_TEXT;
			this->text = NUMBER_TO_STRING;
			this->number = 0;
			return *this;
		}

		long getLong() {
#ifdef USE_GMP_LIB
			return this->number.get_si();
#else
			return this->number.toLong();
#endif
		}

		double getDouble() {
#ifdef USE_GMP_LIB
			return this->number.get_d();
#else
			return this->number.toDouble();
#endif
		}

		Value replace(Value from, Value to) {
			toTxt();
			if(from.toString().empty())
        return text;
    	size_t c = 0;
    	while((c = text.find(from.toString(), c)) != std::string::npos) {
      	text.replace(c, from.toString().length(), to.toString());
  			c += to.toString().length();
    	}
			return text;
		}

		Value find(Value tofind) {
			Value tmp = tofind;
			tmp.toNum();
			Value tmp2 = this;
			tmp2.toTxt();
			return NUMBER((long)tmp2.getString().find(tmp.getLong()));
		}

		Value toUpper() {
			if (type == VALUE_TYPE_NUMBER) {
				toTxt();
				return *this;
			}
			for (size_t c = 0; c < text.size(); c++) {
				if (text[c] > 96 && text[c] < 123) {
					text[c] -= 32;
				}
			}
			return *this;
		}

		Value toLower() {
			if (type == VALUE_TYPE_NUMBER) {
				toTxt();
				return *this;
			}
			for (size_t c = 0; c < text.size(); c++) {
				if (text[c] < 91 && text[c] > 64) {
					text[c] += 32;
				}
			}
			return *this;
		}

		Value trimLeft() {
			if (type == VALUE_TYPE_NUMBER) {
				toTxt();
				return *this;
			}
			size_t i = 0;
			while (text[i] == '\n' || text[i] == '\t' || text[i] == ' ') i++;
			text = text.substr(i);
			return *this;
		}

		Value trimRight() {
			if (type == VALUE_TYPE_NUMBER) {
				toTxt();
				return *this;
			}
			size_t i = text.size() - 1;
			while (text[i] == '\n' || text[i] == '\t' || text[i] == ' ') i--;
			text = text.substr(0, i + 1);
			return *this;
		}

		Value trim() {
			if (type == VALUE_TYPE_NUMBER) {
				toTxt();
				return *this;
			}
			trimRight();
			trimLeft();
			return *this;
		}

		Value codePointAt(Value l) {
			Value tmp = l;
			tmp.toNum();
			return text[tmp.getLong()];
		}

		Value endsWith(Value data) {
			return text.find(data.toString()) == text.size() - data.toString().size();
		}

		Value startsWith(Value data) {
			return text.find(data.toString()) == 0;
		}

		Value& operator+=(Value other) {
			if ((type || other.type) == 0) {
				number += other.number;
			} else {
				toTxt();
				text += other.toString();
			}
			return *this;
		}

		Value operator+(Value other) {
			Value tmp = this;
			tmp += other;
			return tmp;
		}

		Value operator++(int) {
			Value tmp = this;
			operator+=(1);
			return tmp;
		}

		Value operator++() {
			operator+=(1);
			return this;
		}

		Value& operator-=(Value other) {
			if ((type || other.type) == 0) {
				number -= other.number;
			} else {
				toTxt();
				replace(other.toString(), "");
			}
			return *this;
		}

		Value operator-(Value other) {
			Value tmp = this;
			tmp -= other;
			return tmp;
		}

		Value operator--(int) {
			Value tmp = this;
			operator-=(1);
			return tmp;
		}

		Value operator--() {
			operator-=(1);
			return this;
		}

		Value& operator*=(Value other) {
			if ((type || other.type) == 0) {
				number *= other.number;
			} else if (type == VALUE_TYPE_NUMBER && other.type == VALUE_TYPE_TEXT) {
		    std::ostringstream os;
		    for(NUMBER i = 0; i < number; i++) os << other.text;
		    text = os.str();
				type = VALUE_TYPE_TEXT;
				number = 0;
			} else if (type == VALUE_TYPE_TEXT && other.type == VALUE_TYPE_NUMBER) {
		    std::ostringstream os;
		    for(NUMBER i = 0; i < other.number; i++) os << text;
		    text = os.str();
				type = VALUE_TYPE_TEXT;
				number = 0;
			} else {
				toNum();
				std::ostringstream os;
		    for(NUMBER i = 0; i < number; i++) os << other.text;
		    text = os.str();
				type = VALUE_TYPE_TEXT;
				number = 0;
			}
			return *this;
		}

		Value operator*(Value other) {
			Value tmp = this;
			tmp *= other;
			return tmp;
		}

		Value& operator/=(Value other) {
			if ((type || other.type) == 0) {
				number /= other.number;
			} else {
				toNum();
				Value tmp = other;
				tmp.toNum();
				number /= tmp.number;
			}
			return *this;
		}

		Value operator/(Value other) {
			Value tmp = this;
			tmp /= other;
			return tmp;
		}

		Value& operator%=(Value other) {
			if ((type || other.type) == 0) {
#ifdef USE_GMP_LIB
				number = number.get_si() % other.number.get_si();
#else
				number %= other.number;
#endif
			} else {
				toNum();
				Value tmp = other;
				tmp.toNum();
#ifdef USE_GMP_LIB
				number = number.get_si() % tmp.number.get_si();
#else
				number %= tmp.number;
#endif
			}
			return *this;
		}

		Value operator%(Value other) {
			Value tmp = this;
			tmp %= other;
			return tmp;
		}

		Value operator[](Value i) {
			Value tmp = this;
			tmp.toTxt();
			if (tmp.text.size() > i.getLong()) {
				return Value(tmp.text[i.getLong()]);
			}
			return Value(-1);
		}

		Value operator[](int i) {
			Value tmp = this;
			tmp.toTxt();
			if (tmp.text.size() > i) {
				return Value(tmp.text[i]);
			}
			return Value(-1);
		}

		bool operator==(Value other) {
			return strictEquals(other);
		}

		bool operator!=(Value other) {
			return !strictEquals(other);
		}

		bool equals(Value other) {
#ifdef VALUE_MULTI_TYPE_SUPPORT
			return toString() == other.toString() || (type == VALUE_TYPE_NUMBER && (other.getBool() == this->number)) || ((type == True || type == False) && (getBool() == other.number));
#else
			return toString() == other.toString();
#endif
		}

		bool strictEquals(Value other) {
			return type == other.type && equals(other);
		}

		Value substring(Value other) {
			if (type == VALUE_TYPE_NUMBER) toTxt();
			return text = text.substr(other.getLong());
		}

		Value substring(Value v1, Value v2) {
			if (type == VALUE_TYPE_NUMBER) toTxt();
			return text = text.substr(v1.getLong(), v2.getLong() - v1.getLong());
		}

		Value reverse() {
			if (type == VALUE_TYPE_NUMBER) {
				toTxt();
			}
			if (text == "") {
				return *this;
			}
			size_t len = text.size() - 1;
			for (size_t i = 0; i < (len / 2) + 1; i++) {
				char tmp = text[len - i];
				text[len - i] = text[i];
				text[i] = tmp;
			}
			return *this;
		}

		Value operator&&(Value other) {
#ifdef VALUE_MULTI_TYPE_SUPPORT
			return getBool() && other.getBool();
#else
			return number != 0 && other.number != 0;
#endif
		}

		Value operator||(Value other) {
#ifdef VALUE_MULTI_TYPE_SUPPORT
			return getBool() || other.getBool();
#else
			return number != 0 || other.number != 0;
#endif
		}

		Value operator!() {
#ifdef VALUE_MULTI_TYPE_SUPPORT
			if ((type != null && (type == VALUE_TYPE_NUMBER && number != 0)) || type == True) {
				return false;
			}
			return true;
#else
			return number == 0;
#endif
		}

		Value operator~() {
			Value tmp = this;
			tmp.toNum();
			return (int)~tmp.getLong();
		}

		Value operator&(Value other) {
			Value tmp = this;
			tmp.toNum();
			Value tmp2 = other;
			tmp2.toNum();
			return tmp.getLong() & tmp2.getLong();
		}

		Value operator|(Value other) {
			Value tmp = this;
			tmp.toNum();
			Value tmp2 = other;
			tmp2.toNum();
			return tmp.getLong() | tmp2.getLong();
		}

		Value operator^(Value other) {
			Value tmp = this;
			tmp.toNum();
			Value tmp2 = other;
			tmp2.toNum();
			return tmp.getLong() ^ tmp2.getLong();
		}

		Value operator-() {
			Value tmp = this;
			tmp.toNum();
#ifdef USE_GMP_LIB
			return mpf_class(NUMBER(-1) * tmp.getNumber());
#else
			return NUMBER(-1) * tmp.getNumber();
#endif
		}

		Value operator<<(Value other) {
			Value tmp = this;
			tmp.toNum();
			Value tmp2 = other;
			tmp2.toNum();
#ifdef USE_GMP_LIB
			return tmp.getLong() << tmp2.getLong();
#else
			return tmp.getNumber() << tmp2.getNumber();
#endif
		}

		Value operator>>(Value other) {
			Value tmp = this;
			tmp.toNum();
			Value tmp2 = other;
			tmp2.toNum();
#ifdef USE_GMP_LIB
			return tmp.getLong() >> tmp2.getLong();
#else
			return tmp.getNumber() >> tmp2.getNumber();
#endif
		}

#ifdef NUMBER_COMPARISON_OPERATORS
		Value operator<(Value other) {
			if (other.type == VALUE_TYPE_TEXT || type == VALUE_TYPE_TEXT) return false;
			return number < other.number;
		}
		Value operator>(Value other) {
			if (other.type == VALUE_TYPE_TEXT || type == VALUE_TYPE_TEXT) return false;
			return number > other.number;
		}
		Value operator<=(Value other) {
			if (other.type == VALUE_TYPE_TEXT || type == VALUE_TYPE_TEXT) return false;
			return number <= other.number;
		}
		Value operator>=(Value other) {
			if (other.type == VALUE_TYPE_TEXT || type == VALUE_TYPE_TEXT) return false;
			return number >= other.number;
		}
#endif
};

inline std::ostream &operator<<(std::ostream &s, Value *v) {
	return s << v->toString();
}

inline std::ostream &operator<<(std::ostream &s, Value v) {
	return s << v.toString();
}

inline std::istream& operator>>(std::istream &in, Value &val) {
  std::string a;
  in >> a;
  val = a;
  return in;
}

#endif
