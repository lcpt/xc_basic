//xCuadratura2d.cc
#include "matemat/funciones/src/CuadGauss/Cuadratura2D.h"

int main(void)
  {
    Cuadratura2D gs(-1.0,1.0,-1.0,1.0,2,2);
    std::cout << gs << std::endl;
    std::cout << Cuadratura2D(-1.0,1.0,-1.0,1.0,2,2) << std::endl;
    std::cout << Cuadratura2D(-1.0,1.0,-1.0,1.0,3,3) << std::endl;
    std::cout << Cuadratura2D(-1.0,1.0,-1.0,1.0,4,4) << std::endl;
    std::cout << Cuadratura2D(-1.0,1.0,-1.0,1.0,5,5) << std::endl;
    std::cout << Cuadratura2D(-1.0,1.0,-1.0,1.0,6,6) << std::endl;
    return 0;
  }
