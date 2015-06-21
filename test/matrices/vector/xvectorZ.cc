//xvectorz.cpp
#include "matemat/matrices/src/vectorZ.h"

int main(void)
  {
    vectorZ<char> v(2);
    v[0]= 'a';
    std::cout << v << std::endl;
    vectorZ<char> v2(v);
    std::cout << v2 << std::endl;
    std::cout << "Iguales: " << (v2 == v) << std::endl;
    return 1;
  }
