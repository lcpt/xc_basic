//xBisiesto.cc
//Prueba del objeto Agno.

#include "../src/agno.h"
#include <iostream>

int main(void)
  {
    unsigned long dias= 0;
    Agno a;
    for(a=ORIGEN;a<=1997;a++)
      {
         dias= a.DiasAnteriores()+5;
         Agno b(dias);
	 std::cout << "Agno: " << a
              << " dias: " << dias
		   << " Agno dias: " << b << std::endl;
         if ( a != b ) break;
      }
    return 0;
  }
