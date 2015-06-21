//xmatriz.cpp
#include <iostream>
#include "matemat/matrices/src/matrizZ.h"

typedef matrizZ<double> m_double;

int main(void)
  {
    m_double m11(1,1,3.0);
    std::cout << m11.Abs2() << std::endl;
    m_double m13(1,3,3.0);
    std::cout << m13.Abs2() << std::endl;
    m_double m31(3,1,3.0);
    std::cout << m31.Abs2() << std::endl;
    m_double m23(2,3,3.0);
    std::cout << m23.Abs2() << std::endl;
    m_double m32(3,2,3.0);
    std::cout << m32.Abs2() << std::endl;
    m_double m33(3,3,3.0);
    std::cout << m33.Abs2() << std::endl;
    return 0;
  }
