//xMes.cpp
//Prueba del objeto mes.

#include "../src/mes.h"
#include <iostream>

int main(void)
  {
    Mes mm;
    Mes ult= mm.Ultimo();
    Mes prim= mm.Primero();
    for (mm= prim;mm<ult;mm++)
      std::cout << mm << std::endl;
    std::cout << mm << std::endl;
    mm++;
    unsigned short int i;
    Mes m2;
    for (i=1;i<36;i++)
      { 
        m2= mm - i;
        std::cout << mm << " - " << i << "= " << m2 << std::endl;
      }
    for (mm= prim;mm<ult;mm++)
      std::cout << "mes: " << mm
           << " dias mes:" << mm.Dias(0)
           << " dias anteriores:" << mm.DiasAnteriores(0) << std::endl;
    std::cout << "mes: " << mm
         << " dias mes:" << mm.Dias(0)
         << " dias anteriores:" << mm.DiasAnteriores(0) << std::endl;
    return 0;
  }
