#include <g++/std/typeinfo.h>
#include <iostream.h>
int main(void)
  {
    int a;
    cout << (typeid(7) == typeid(a)) << endl;
    return 0;
  }
