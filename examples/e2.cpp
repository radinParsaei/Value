#define VALUE_MULTI_TYPE_SUPPORT
#include <value.h>
#include <iostream>
using namespace std;

void freeUnusedPointer(long) {}

int main() {
  Value a = "Hello, World!!!";
  cout << a << endl;
  a = 10;
  cout << a << endl;
  a = True;
  cout << a << endl;
  cout << (a = False) << endl;
  a = null;
  cout << a << endl;
}
