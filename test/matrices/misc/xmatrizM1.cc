//xmatrizM.cc
#include "matemat/matrices/src/matrizM.h"

int main(void)
  {
    m_double i= identidad(m_double(3,3));
    matrizM m(1,1,i);
    matrizM v(1,1,m_double(3,1,1.0));
    std::cout << "m= " << m << std::endl;
    std::cout << "v= " << v << std::endl;
    std::cout << "m*v= " << m*v << std::endl;
    return 0;
  }
