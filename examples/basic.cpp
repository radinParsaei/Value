#include <iostream>
#include <value.h>
using namespace std;

int main() {
	Value a = "Hello, World!!!";
	cout << a.toString() << endl;
	a = 10;
	cout << a.toString() << endl;
	a = Types::True;
	cout << a.toString() << endl;
	a = Types::False;
	cout << a.toString() << endl;
	a = Types::Null;
	cout << a.toString() << endl;
	a = Types::Array;
	a.append(10);
	a.append("Hello");
	cout << a.toString() << endl;
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
	std::cout << (b + c).toString() << std::endl;
}
