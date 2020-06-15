#ifndef __VALUE__H__
#define __VALUE__H__

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
	bool type;
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
		Value() { type = 0; }
		Value(NUMBER data) {
			type = 0;
			this->number = data;
			this->text = "";
		}

		Value(double data) {
			type = 0;
			this->number = data;
			this->text = "";
		}

		Value(int data) {
			type = 0;
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
			type = 1;
			this->text = data;
			this->number = 0;
		}

		Value(std::string data) {
			type = 1;
			this->text = data;
			this->number = 0;
		}

		Value(Value* other) {
			type = other->type;
			this->text = other->text;
			this->number = other->number;
		}

		Value& operator=(const char* data) {
			type = 1;
			this->text = data;
			this->number = 0;
			return *this;
		}

		Value& operator=(std::string data) {
			type = 1;
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
			type = 0;
			this->number = n;
			this->text = "";
			return *this;
		}

		Value& operator=(int i) {
			type = 0;
			this->number = i;
			this->text = "";
			return *this;
		}

		std::string toString() {
			if(type){
				return text;
			} else {
				return NUMBER_TO_STRING;
			}
		}

		bool getType() {
			return type;
		}

		NUMBER getNumber() {
			return number;
		}

		std::string getString() {
			return text;
		}

		void toNum() {
			if(!type) return;
			type = 0;
#ifdef USE_GMP_LIB
			size_t i = 0;
			for(; text[i] != 0; i++){
				if(!isdigit(text[i])){
					this->number = 0;
					this->text = "";
					return ;
				}
			}
#endif
			this->number = NUMBER(text.c_str());
			this->text = "";
		}

		void toTxt() {
			if(type) return;
			type = 1;
			this->text = NUMBER_TO_STRING;
			this->number = 0;
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
			if (!type) {
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
			if (!type) {
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
			if (!type) {
				toTxt();
				return *this;
			}
			size_t i = 0;
			while (text[i] == '\n' || text[i] == '\t' || text[i] == ' ') i++;
			text = text.substr(i);
			return *this;
		}

		Value trimRight() {
			if (!type) {
				toTxt();
				return *this;
			}
			size_t i = text.size() - 1;
			while (text[i] == '\n' || text[i] == '\t' || text[i] == ' ') i--;
			text = text.substr(0, i + 1);
			return *this;
		}

		Value trim() {
			if (!type) {
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
			} else if (type == 0 && other.type == 1) {
		    std::ostringstream os;
		    for(NUMBER i = 0; i < number; i++) os << other.text;
		    text = os.str();
				type = 1;
				number = 0;
			} else if (type == 1 && other.type == 0) {
		    std::ostringstream os;
		    for(NUMBER i = 0; i < other.number; i++) os << text;
		    text = os.str();
				type = 1;
				number = 0;
			} else {
				toNum();
				std::ostringstream os;
		    for(NUMBER i = 0; i < number; i++) os << other.text;
		    text = os.str();
				type = 1;
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
};

std::ostream &operator<<(std::ostream &s, Value *v) {
	return s << v->toString();
}

std::ostream &operator<<(std::ostream &s, Value v) {
	return s << v.toString();
}

#endif
