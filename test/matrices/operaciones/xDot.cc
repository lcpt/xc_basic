#include <iostream>
#include "matemat/matrices/src/matrizZ.h"


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
    std::cout << dot(m,m2) << std::endl;
    return 0;
  }
