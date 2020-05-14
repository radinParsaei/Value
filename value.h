#ifndef __VALUE__H__
#define __VALUE__H__

#include <string.h>

#ifdef USE_GMP_LIB
#define NUMBER mpf_class
#include <gmpxx.h>
#include <string.h>
#include <sstream>
#include <iomanip>
using namespace std;
#ifndef USE_UTILS
#endif
#ifdef USE_UTILS
#define NUMBER_TO_STRING Utils::stringDuplicate(mpf_class_to_string(this->data.number).c_str());
#else
#define NUMBER_TO_STRING stringDuplicate(mpf_class_to_string(this->data.number).c_str());
#endif
#else
#include <BigNumber.h>
#define NUMBER BigNumber
#define NUMBER_TO_STRING data.number.toString()
#endif

#ifdef USE_UTILS
namespace Utils {
	inline size_t stringLength(const char* str) {
		size_t s = 0;
		while (str[s++] != '\0');
		return s;
	}

	inline int find(const char* data, const char* tofind) {
		size_t c = 0;
		size_t len = 0;
		while (data[c] != 0) {
			if (data[c] == tofind[len]) {
				len++;
				if (len == strlen(tofind)) {
					return c - len + 1;
				}
			} else {
				len = 0;
			}
			c++;
		}
		return -1;
	}

	inline char* stringDuplicate(const char* data) {
		size_t len = strlen(data) + 1;
    void *dup = malloc(len);
    return (char*)memcpy(dup, data, len);
	}

	inline const char* reverse(const char* in) {
#ifdef USE_UTILS
			char* data = Utils::stringDuplicate(in);
#else
			char* data = stringDuplicate(in);
#endif
		size_t len = strlen(data) - 1;
		for (size_t i = 0; i < len / 2 + 1; i++) {
			char tmp = *(data + len - i);
			*(data + len - i) = *(data + i);
			*(data + i) = tmp;
		}
		return data;
	}

	inline const char* replace(const char *replaceOn, const char *from, const char *to) {
		char *result;
		int i = 0, c = 0;
		int tolen = stringLength(to);
		int fromlen = stringLength(from);
		for (; replaceOn[i] != '\0'; i++) {
			if (strstr(&replaceOn[i], from) == &replaceOn[i]) {
				c++;
				i += fromlen - 1;
			}
		}
		result = new char[i + c * (tolen - fromlen) + 1];
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

	inline const char* append(const char* a, const char* b) {
		size_t alen = strlen(a);
		size_t len = alen + strlen(b) + 1;
		char* dup = new char[len];
		memcpy(dup, a, alen);
		size_t tmp = 0;
		while (alen < len) {
			dup[alen] = *(b + tmp++);
			alen++;
		}
		return dup;
	}

	inline const char* repeat(const char* a, int c) {
		size_t alen = strlen(a);
		size_t len = alen * c + 1;
		char* dup = new char[len];
		for (size_t i = 0; i < c; i++) memcpy(dup + i * alen, a, alen);
		dup[len - 1] = 0;
		return dup;
	}

	inline bool isEQ(const char* a, const char* b) {
		size_t i = 0;
		while (a[i] != 0) {
			if (b[i] == 0) {
				return false;
			}
			if (a[i] != b[i]) return false;
			i++;
		}
		return a[i] == 0 && b[i] == 0;
	}

	inline char* substring(const char* on, size_t len, size_t from = 0) {
		char* sub = (char*)malloc(len + 1);
		for (size_t i = 0; i < len; i++) {
			sub[i] = *(on + from + i);
		}
		sub[len] = 0;
		return sub;
	}

	const char* rtrim(const char* s) {
		size_t i = 0;
		while (s[++i] == ' ' || s[i] == '\t' || s[i] == '\n');
		s = substring(s, stringLength(s) - i, i);
		return s;
	}

	const char* ltrim(const char* s) {
		size_t i = stringLength(s) - 1;
		while (s[--i] == ' ' || s[i] == '\t' || s[i] == '\n');
		s = substring(s, i + 1);
		return s;
	}

	const char* trim(const char* s) {
		const char* a = rtrim(s);
		void* pA = (void*)a;
		a = ltrim(a);
		free(pA);
		return a;
	}
}
#endif

class Value {
	char* stringDuplicate(const char* data) {
		size_t len = strlen(data) + 1;
    char *dup = new char[len];
    return (char*)memcpy(dup, data, len);
	}
	bool type = 0;
	struct {
#ifdef USE_GMP_LIB
		mpf_class number = 0;
#else
		BigNumber number = 0;
#endif
		char* string = 0;
	} data;
#ifdef USE_GMP_LIB
	string mpf_class_to_string(mpf_class data) {
		ostringstream s;
		int scale = 0;
		mpf_class tmp = data;
		while (tmp.get_d() != tmp.get_si()) {
			tmp /= 10;
			scale++;
		}
	  s << fixed << setprecision(scale) << data;
	  string str = s.str();
	  int i = str.size();
	  while(str[--i] == '0')str.pop_back();
	  if(str[str.size() - 1] == '.')str.pop_back();
		return str;
	}
#endif
	public:
		Value() { type = 0; }
		Value(
#ifdef USE_GMP_LIB
			mpf_class
#else
			BigNumber
#endif
			data) {
			type = 0;
			this->data.number = data;
			this->data.string = 0;
		}

		Value(double data) {
			type = 0;
			this->data.number = data;
			this->data.string = 0;
		}

		Value(int data) {
			type = 0;
			this->data.number = data;
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
			*this =
#ifdef USE_GMP_LIB
			mpf_class
#else
			BigNumber
#endif
			(res);
		}

		Value(long data) {
			this->data.number = NUMBER(data);
		}

		Value(const char* data) {
			type = 1;
#ifdef USE_UTILS
			this->data.string = Utils::stringDuplicate(data);
#else
			this->data.string = stringDuplicate(data);
#endif
			this->data.number = 0;
		}

		Value& operator=(const char* data) {
			type = 1;
#ifdef USE_UTILS
			this->data.string = Utils::stringDuplicate(data);
#else
			this->data.string = stringDuplicate(data);
#endif
			this->data.number = 0;
			return *this;
		}

		Value& operator=(Value other) {
			type = other.type;
			this->data = other.data;
			return *this;
		}
		Value& operator=(
#ifdef USE_GMP_LIB
			mpf_class
#else
			BigNumber
#endif
			other) {
			type = 0;
			this->data.number = other;
			this->data.string = 0;
			return *this;
		}

		char* toString() {
			if(type){
				return data.string;
			} else {
				return NUMBER_TO_STRING;
			}
		}

		bool getType() {
			return type;
		}

#ifdef USE_GMP_LIB
		mpf_class
#else
		BigNumber
#endif
		getNumber() {
			return data.number;
		}

		char* getString() {
			return data.string;
		}

		void toNum() {
			if(!type) return;
			type = 0;
			this->data.number =
#ifdef USE_GMP_LIB
			mpf_class(data.string);
#else
			BigNumber(data.string);
#endif
			this->data.string = 0;
		}

		void toTxt() {
			if(type) return;
			type = 1;
			this->data.string = NUMBER_TO_STRING;
			this->data.number = 0;
		}

		long getLong() {
#ifdef USE_GMP_LIB
			return this->data.number.get_si();
#else
			return this->data.number.toLong();
#endif
		}

		double getDouble() {
#ifdef USE_GMP_LIB
			return this->data.number.get_d();
#else
			return this->data.number.toDouble();
#endif
		}
};

#ifdef STD_STREAMS
std::ostream &operator<<(std::ostream &s, Value &v) {
		return s << v.toString();
}
#endif

#endif
