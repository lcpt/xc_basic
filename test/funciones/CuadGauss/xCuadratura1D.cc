//xCuadratura1d.cc
#include "matemat/funciones/src/CuadGauss/Cuadratura1D.h"

int main(void)
  {
    Cuadratura1D pg(-1,1,1);
    std::cout << pg << std::endl;
    std::cout << Cuadratura1D(-1,1,2) << std::endl;
    std::cout << Cuadratura1D(-1,1,3) << std::endl;
    std::cout << Cuadratura1D(-1,1,4) << std::endl;
    std::cout << Cuadratura1D(-1,1,5) << std::endl;
    std::cout << Cuadratura1D(-1,1,6) << std::endl;
    return 0;
  }
