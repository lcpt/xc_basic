//xpuntero.cpp
#include "../src/puntero.h"
#include <iostream>

int main(void)
  {
    int val[]= {0,1,2,3,4,5,6,7,8,9};
    puntero<int> p;
    p= &val[1];
    std::cout << *p << std::endl;
    return 0;
  }
