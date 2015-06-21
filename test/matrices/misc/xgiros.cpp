//xGiros.cpp

#include <iostream.h>
#include "giros.h"
#include <math.h>

void main(void)
  {
    double rx= M_PI/4;
    double ry= M_PI/4;
    double rz= M_PI/4;
    matrizZ<double> gx(3,3);
    gx= GiroX(rx);
    matrizZ<double> gy(3,3);
    gy= GiroY(ry);
    matrizZ<double> gz(3,3);
    gz= GiroZ(rz);
    std::cout << "gx= " << gx << std::endl;
    std::cout << "gy= " << gy << std::endl;
    std::cout << "gz= " << gz << std::endl;
    matrizZ<double> gxyz(3,3);
    gxyz= GiroXYZ(rx,ry,rz);
    std::cout << "gxyz= " << gxyz << std::endl;
    std::cout << "gxyz2= " << gz * gy * gx << std::endl;
    std::cout << "dif= " << gxyz - (gz * gy * gx) << std::endl;
  }
