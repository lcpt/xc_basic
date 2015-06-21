//xbusquedabruta.cc

#include "streams/src/imanip.h"

int main(void)
  {
    busca_fin(std::cin,"modelo");
    while(std::cin.ipfx(0))
      std::cout << (char) std::cin.get();
    std::cout << std::endl;
    return 0;
  }
