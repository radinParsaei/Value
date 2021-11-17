#define PTR_TO_STRING
#define VALUE_MULTI_TYPE_SUPPORT
#include <value.h>
#include <iostream>
using namespace std;

TEXT ptrToString(long ptr) {
    if (ptr == 0) return "null";
    return Value(ptr).toString();
}

void freeUnusedPointer(long a) {
    cout << "Freeing " << a << endl;
}

int main() {
  Value a = (void*)0;
  cout << a + 10 << endl;
}
