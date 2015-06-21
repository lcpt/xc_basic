#include <iostream.h>
#include "vectorZ.h"


int main(void)
  {
/*
    short unsigned int c= 4;
    cout << c << std::endl;
*/
    vectorZ<short unsigned int> v(4);
    v.Con(3);
    cout << v << std::endl;
  }
