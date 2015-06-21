//xvcolM.cc
#include "matemat/matrices/src/vcolM.h"

int main(void)
  {
    m_double i(2,2);
    i= identidad(m_double(2,2));
    vcolM m(2,i);
    std::cout << "m= " << m << std::endl;
    std::cout << "total filas= " << m.GetTotalFilas() << std::endl;
    std::cout << "total columnas " << m.GetTotalCols() << std::endl;
    m*=i;
    std::cout << "m= " << m << std::endl;
    vcolM m1(2,i);
    m_double a(3,1,7.0);
    m_double b(2,1,3.0);
    m1(1)= a;
    m1(2)= b;
    std::cout << "m1= " << m1 << std::endl;
    vcolM m3(m1);
    m3*=2.0;
    std::cout << "m1*2= " << m3 << std::endl;
    m_double c(1,1,1.0);
    vcolM m2(1,c);
    std::cout << "m2= " << m2 << std::endl;
    std::cout << "m1*m2= " << m2*m1 << std::endl;
    return 0;
  }
