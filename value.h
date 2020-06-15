#ifndef __VALUE__H__
#define __VALUE__H__

#include <string.h>

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

		Value& operator+(Value other) {
			if (type + other.type == 0) {
				number += other.number;
			} else {
				toTxt();
				text += other.toString();
			}
			return *this;
		}
};

std::ostream &operator<<(std::ostream &s, Value *v) {
	return s << v->toString();
}

std::ostream &operator<<(std::ostream &s, Value v) {
	return s << v.toString();
}

#endif
