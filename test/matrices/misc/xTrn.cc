#include <iostream>
#include "../../src/matrizZ.h"

int main(void)
  {
    matrizZ<double> m(4,2,0.0);
    m(1,1)= 1.0;m(1,2)= 2.0;
    m(2,1)= 3.0;m(2,2)= 4.0;
    m(3,1)= 5.0;m(3,2)= 6.0;
    m(4,1)= 7.0;m(4,2)= 8.0;
    std::cout << m << std::endl;
    m.Trn();
    std::cout << m << std::endl;
    matrizZ<double> m2(m.GetTrn());
    std::cout << m2 << std::endl;
    matrizZ<double> m3= m2.GetTrn();
    std::cout << m3 << std::endl;
    matrizZ<double> m4;
    m4= m3.GetTrn();
    std::cout << m4 << std::endl;
    return 0;
  }
