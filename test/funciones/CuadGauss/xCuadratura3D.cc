//xCuadratura2d.cc
#include "matemat/funciones/src/CuadGauss/Cuadratura3D.h"
#include "matemat/geom-2.0/src/pos_vec/Pos3d.h"
//#include "streams/src/nucleo/EntCmd.h"
//#include "streams/src/base/CmdStatus.hxx"

int main(void)
  {
    Cuadratura3D gs(Pos3d(-1.0,-1.0,-1.0),Pos3d(1.0,1.0,1.0),2,2,2);
    std::cout << gs << std::endl;
    std::cout << Cuadratura3D(Pos3d(-1.0,-1.0,-1.0),Pos3d(1.0,1.0,1.0),3,3,3) << std::endl;
    std::cout << Cuadratura3D(Pos3d(-1.0,-1.0,-1.0),Pos3d(1.0,1.0,1.0),4,4,4) << std::endl;
    std::cout << Cuadratura3D(Pos3d(-1.0,-1.0,-1.0),Pos3d(1.0,1.0,1.0),5,5,5) << std::endl;
    std::cout << Cuadratura3D(Pos3d(-1.0,-1.0,-1.0),Pos3d(1.0,1.0,1.0),6,6,5) << std::endl;
    return 0;
  }
