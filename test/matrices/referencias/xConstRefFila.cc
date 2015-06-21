//xConstRefFila.cc

#include <iostream>
#include "matemat/matrices/src/matrizZ.h"

int main(void)
  {
    matrizZ<double> m3(4,4,0.0);
    m3(1,1)= 3.0;m3(1,2)= 1.0;m3(1,3)= 2.0;m3(1,4)= -1.0;
    m3(2,1)= 5.0;m3(2,2)= -2.0;m3(2,3)= -3.0;m3(2,4)= 1.0;
    m3(3,1)= 1.0;m3(3,2)= 3.0;m3(3,3)= 4.0;m3(3,4)= -2.0;
    m3(4,1)= 2.0;m3(4,2)= -1.0;m3(4,3)= 1.0;m3(4,4)= -3.0;
    std::cout.precision(4);

    m3.Print(std::cout);
    std::cout << std::endl;
    matrizZ<double>::const_ref_fila f3= m3.GetConstRefFila(3);
    std::cout << "f3= " << f3 << std::endl;
    std::cout << "f3(2)= " << f3(2) << std::endl;
    matrizZ<double>::const_ref_fila f4= m3.GetConstRefFila(4,1,3);
    std::cout << f4 << std::endl;
    std::cout << "f4= " << f4 << std::endl;
    std::cout << "f4(1)= " << f4(1) << std::endl;

    matrizZ<double>::const_ref_fila ff4= m3.GetConstRefFila(4,RangoIndice(1,3));
    std::cout << ff4 << std::endl;
    std::cout << "ff4= " << ff4 << std::endl;
    std::cout << "ff4(1)= " << ff4(1) << std::endl;
    std::cout << "rango filas ff4: " << ff4.RangoFilas() << std::endl;
    std::cout << "rango columnas ff4: " << ff4.RangoCols() << std::endl;
    return 0;
  }
