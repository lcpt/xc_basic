//xDiaSemana.cpp
//Prueba del objeto DiaSemana.

using namespace std;

#include "../src/diasemana.h"
#include <iostream>

int main(void)
  {
    DiaSemana dd;
    DiaSemana ult= dd.Ultimo();
    DiaSemana prim= dd.Primero();
    for (dd= prim;dd<ult;dd++)
      cout << dd << endl;
    cout << dd << endl;
    dd++;
    unsigned short int i;
    DiaSemana d2;
    for (i=1;i<29;i++)
      { 
        d2= dd - i;
        cout << dd << " - " << i << "= " << d2 << endl;
      }
    return 0;
  }
