#include <iostream>
#include "matemat/matrices/src/op_tensor.h"
//Ejemplo de producto tensorial.
int main(void)
  {
    matrizZ<double> m(4,1,0.0);
    m(1,1)= 1.0;
    m(2,1)= 2.0;
    m(3,1)= 3.0;
    m(4,1)= 4.0;
    m.Trn();
    matrizZ<double> m2(m.GetTrn());
    std::cout << m << std::endl;
    std::cout << m2 << std::endl;
    std::cout << (m & m2) << std::endl;
    std::cout << (m2 & m) << std::endl;
    return 0;
  }
