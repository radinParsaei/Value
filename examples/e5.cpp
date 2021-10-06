#include <iostream>
#include <value.h>

using namespace std;

void freeUnusedPointer(long) {}

int main() {
  Value a = "     HELLO WORLD";
  a.trimLeft();
  a.toLower();
  Value b = "H";
  a.replace("h", b);
  a.replace("world", "World");
  cout << a[0].toLower() + "ello" << endl << a.substring(0, 5).reverse() << endl;
  if (a.startsWith("Hello")) {
    cout << a << " is startsWith Hello" << endl;
  } else if (a.endsWith("World")) {
    cout << a << " is endsWith World" << endl;
  }
  cout << a[0] << " as a number is " << a.codePointAt(0) << endl;
  cout << Value("Hello     ").trim() << Value(" World   ").trimRight() << (Value("!") * 3) << endl;
  if (a.find("e") != -1) {
    cout << a << " has \'e\'\n";
  }
  cout << "enter a number: ";
  Value tmp;
  cin >> tmp;
  if (tmp.canNum()) {
    cout << "Hmm.. " << tmp << " is really a number\n";
  } else {
    cout << "Ooops.. " << tmp << " is not a number\n";
  }
}
