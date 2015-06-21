#include <iostream>
#include "matemat/matrices/src/matrizZ.h"

typedef matrizZ<double> m_double;

int main(void)
  {
    const float w[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    m_double m1(3,3,w,w+9);
    std::cout << m1 << std::endl;
    m_double m2(m1);
    std::cout << "Iguales: " << (m1 == m2) << std::endl;
    return 0;
  }
