//xConstRefCol.cc

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
    matrizZ<double>::const_ref_col c3= m3.GetConstRefCol(3);
    std::cout << "c3= " << c3 << std::endl;
    std::cout << "c3(2)= " << c3(2) << std::endl;
    matrizZ<double>::const_ref_col c4= m3.GetConstRefCol(4,1,3);
    std::cout << "c4= " << c4 << std::endl;
    std::cout << "c4(3)= " << c4(3) << std::endl;

    matrizZ<double>::const_ref_col cc4= m3.GetConstRefCol(RangoIndice(1,3),4);
    std::cout << "cc4= " << cc4 << std::endl;
    std::cout << "cc4(3)= " << cc4(3) << std::endl;
    std::cout << "rango filas cc4: " << cc4.RangoFilas() << std::endl;
    std::cout << "rango columnas cc4: " << cc4.RangoCols() << std::endl;
    return 0;
  }
