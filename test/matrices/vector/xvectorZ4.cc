//xvectorz.cpp
#include "matemat/matrices/src/vectorZ.h"

int main(void)
  {
    vectorZ<double> v(2);
    v[0]= 1.1;
    std::cout << v << std::endl;
    vectorZ<double> v2(v);
    std::cout << v2 << std::endl;
    std::cout << "Iguales: " << (v2 == v) << std::endl;
    return 1;
  }
