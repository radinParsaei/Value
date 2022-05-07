#include <iostream>
#include <value.h>

int main() {
	Value a = "Hello, World!!!";
	std::cout << a.toString() << std::endl;
	a = 10;
	std::cout << a.toString() << std::endl;
	a = Types::True;
	std::cout << a.toString() << std::endl;
	a = Types::False;
	std::cout << a.toString() << std::endl;
	a = Types::Null;
	std::cout << a.toString() << std::endl;
	a = Types::Array;
	a.append(10);
	a.append("Hello");
	std::cout << a.toString() << std::endl;
	a = Types::Map;
	a.put(Types::True, "True");
	a.put(Types::False, "False");
	a.put(Types::Null, "NULL");
	a.put(10, "ten");
	a.put("Hello", "World");
	std::cout << "Hello, " << a.get("Hello").toString() << "!!!" << std::endl;
	Value b = 10;
	Value c = 20;
	c += "Hello World"; // 20Hello World
	c -= " World"; // 20Hello
	b++;
	std::cout << (b + c).toString() << std::endl;
	Value arr = Types::Array;
	arr.append(10);
	arr.append("Hello");
	HashFunction hfn;
	std::cout << hfn(arr) << std::endl;
	arr.reverse();
	std::cout << hfn(arr) << std::endl;
}
