#include <value.h>
#include <iostream>
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
}
