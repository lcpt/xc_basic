//xBisiesto.cc
//Prueba del objeto mes.

#include "tiempo/src/agno.h"
#include <iostream>

int main(void)
  {
    int bisiestos= 0;
    Agno a;
    for(a=ORIGEN;a<=1997;a++)
      {
         std::cout << "Agno: " << a
              << " bisiesto: " << a.Bisiesto()
              << " num. bisiestos: " << bisiestos
	      << " num. bisiestos: " << a.GetNumBisiestos() << std::endl;
         if (bisiestos != a.GetNumBisiestos()) break;
         if(a.Bisiesto()) bisiestos++;
      }
    return 1;
  }
