//xvectorz.cpp
#include "matemat/matrices/src/vectorZ.h"

int main(void)
  {
    vectorZ<int> v(2);
    v[0]= 'a';
    std::cout << v << std::endl;
    vectorZ<int> v2(v);
    std::cout << v2 << std::endl;
    std::cout << "Iguales: " << (v2 == v) << std::endl;
    return 1;
  }
