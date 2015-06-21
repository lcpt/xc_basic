//xFuncPorPuntosR2_T.cc

#include "matemat/funciones/src/func_por_puntos/FuncPorPuntosR2_T.h"

int main(void)
  {
    FuncPorPuntosR2_T<double> fp1(2,2);
    std::cout << fp1 << std::endl;
    const MatrizPos2d m(Pos2d(0.0,0.0),Pos2d(1.0,1.0),2);
    FuncPorPuntosR2_T<double> fp2(m,1.0);
    std::cout << fp2 << std::endl;
    const FuncPorPuntosR2_T<double>::m_valores d(m.getNumFilas(),m.getNumCols(),2.0);
    FuncPorPuntosR2_T<double> fp3(m,d);
    std::cout << fp3 << std::endl;
    return 0;
  }
