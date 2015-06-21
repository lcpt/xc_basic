//xNmbVars.cpp

#include <iostream>
#include "matemat/funciones/src/polinomios/NmbVars.h"

int main(void)
  {
    NmbVars n("xyzt");
    std::cout << n << std::endl;
    NmbVars n2(n);
    n2= PutVars("bzya");
    std::cout << n2 << std::endl;
    std::cout << "CambioVar: " << n.CambioVar(0,n2) << std::endl;
    return 1;
  }
