#include <value.h>
#include <iostream>
using namespace std;

void freeUnusedPointer(long) {}

int main() {
	Value a = "Hello, World!!!";
	cout << a << endl;
	a = 10;
	cout << a << endl;
}
