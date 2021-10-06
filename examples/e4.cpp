#define NUMBER_COMPARISON_OPERATORS
#include <value.h>
#include <iostream>
using namespace std;

void freeUnusedPointer(long) {}

int main() {
  Value a;
  cin >> a; //read a text
  a.toNum(); //convert a to number
  if (a < 10) {
    cout << "< 10" << endl;
  } else {
    cout << ">= 10" << endl;
  }
}
