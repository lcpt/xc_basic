//xFuncPorPuntosR2_R.cc

#include "matemat/funciones/src/func_por_puntos/FuncPorPuntosR2_R.h"

int main(void)
  {
    FuncPorPuntosR2_R fp1(2,2);
    std::cout << fp1.PuntoMax() << std::endl;
    const MatrizPos2d m(Pos2d(0.0,0.0),Pos2d(1.0,1.0),2);
    FuncPorPuntosR2_R fp2(m,1.0);
    std::cout << fp2.PuntoMax() << std::endl;
    const FuncPorPuntosR2_R::m_valores d(m.getNumFilas(),m.getNumCols(),2.0);
    FuncPorPuntosR2_R fp3(m,d);
    std::cout << fp3.PuntoMax() << std::endl;
    return 0;
  }
