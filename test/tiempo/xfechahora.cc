//xFechaHora.cpp
//Prueba del objeto fecha.

#include "../src/fechahora.h"
#include <iostream>

int main(void)
  {
    FechaHora f(1,1,1996,10,30,28.2);
    std::cout << f << std::endl;
    std::cout << f+7 << std::endl;
    std::cout << f-7 << std::endl;
    std::cout << f+100 << std::endl;
    f.Time();
    std::cout << f << std::endl;
    f.LocalTime();
    std::cout << f << std::endl;
    f.TimeOfDay();
    std::cout << "Fecha GMT: " << f << std::endl;
    f.LocalTimeOfDay();
    std::cout << "Fecha local: " << f << std::endl;
    return 0;
  }
