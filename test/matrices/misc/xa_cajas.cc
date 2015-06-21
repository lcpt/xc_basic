//xmatrizM.cc
#include "matemat/matrices/src/matrizM.h"

int main(void)
  {
    m_double i(2,2);
    i= identidad(m_double(2,2));
    matrizM m(2,3,i);
    std::cout << "m= " << m << std::endl;
    std::cout << "total filas= " << m.GetTotalFilas() << std::endl;
    std::cout << "total columnas " << m.GetTotalCols() << std::endl;
    m_double m2= m.QuitaBloques();
    std::cout << "m2= " << m2 << std::endl;
    matrizM m3= a_cajas(m,m2);
    std::cout << "iguales: " << (m == m3) << std::endl;
    return 0;
  }
