//xdias_mes.cc
//Prueba del objeto mes.

#include "../src/mes.h"
#include <iostream>

int main(void)
  {
    Mes mm;
    Mes ult= mm.Ultimo();
    Mes prim= mm.Primero();
    int dias;
    for (mm= prim;mm<ult;mm++)
      {
         dias= mm.DiasAnteriores(0)+5;
         Mes b(dias,0);
         std::cout << "Mes: " << mm
              << " dias: " << dias
              << " Mes(dias): " << b << std::endl;
         if ( mm != b ) break;
      }
    dias= mm.DiasAnteriores(0)+5;
    Mes b(dias,0);
    std::cout << "Mes: " << mm
         << " dias: " << dias
         << " Mes(dias): " << b << std::endl;
    return 0;
  }
