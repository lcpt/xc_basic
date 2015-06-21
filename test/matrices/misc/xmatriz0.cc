#include <iostream>
#include "matemat/matrices/src/matrizZ.h"

typedef matrizZ<double> m_double;

int main(void)
  {
    m_double m1(1,1);
    m1(1,1)= 1;
    std::cout << m1 << std::endl;
    m_double m2(m1);
    std::cout << "Iguales: " << (m1 == m2) << std::endl;
    return 0;
  }
