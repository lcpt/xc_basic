//xConstRefCaja.cc

#include <iostream>
#include "matemat/matrices/src/matrizZ.h"
#include "matemat/matrices/src/RangoMatriz.h"

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
    matrizZ<double>::const_ref_caja sm= m3.GetConstRefCaja(2,2);
    std::cout << sm << std::endl;
    std::cout << sm(2,2) << std::endl;
    std::cout << std::endl;
    matrizZ<double>::const_ref_caja sm2= m3.GetConstRefCaja(1,3,4,3);
    sm2.Print(std::cout);
    std::cout << std::endl;
    const RangoIndice filas(1,4);
    const RangoIndice cols(3,3);
    matrizZ<double>::const_ref_caja sm3= m3.GetConstRefCaja(filas,cols);
    sm3.Print(std::cout);
    std::cout << std::endl;
    std::cout << "rango filas sm3: " << sm3.RangoFilas() << std::endl;
    std::cout << "rango columnas sm3: " << sm3.RangoCols() << std::endl;

    RangoMatriz rm(m3);
    std::cout << "rango matriz m3: " << rm << std::endl;
    return 0;
  }
