#include <iostream>
#include "matemat/matrices/src/matsimZ.h"

typedef matsimZ<double> ms_double;

int main(void)
  {
    ms_double m1(1);
    m1(1,1)= 1;
    std::cout << m1 << std::endl;
    ms_double m2(m1);
    std::cout << m2 << std::endl;
    std::cout << "Iguales: " << (m1 == m2) << std::endl;
    return 0;
  }
