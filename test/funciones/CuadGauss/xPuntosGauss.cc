//xPuntosGauss.cc
#include "matemat/funciones/src/CuadGauss/PuntosGauss.h"

int main(void)
  {
    PuntosGauss pg(-1,1,1);
    std::cout << pg << std::endl;
    std::cout << PuntosGauss(-1,1,2) << std::endl;
    std::cout << PuntosGauss(-1,1,3) << std::endl;
    std::cout << PuntosGauss(-1,1,4) << std::endl;
    std::cout << PuntosGauss(-1,1,5) << std::endl;
    std::cout << PuntosGauss(-1,1,6) << std::endl;
    return 0;
  }
