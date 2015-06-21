//xIndPol2.cpp
#include "matemat/funciones/src/polinomios/IndPol.h"

int main(void)
  {
    NmbVars n1("xyz");
    NmbVars n2("xz");
    IndPol v1(3);
    IndPol v2(2);
    v1[0]= 3;v1[1]= 0; v1[2]= 0;
    v2[0]= 0;v2[1]= 1;
    std::cout << "v2= " << v2 << std::endl;
    std::cout << "Conv v2: " << Conv(n1,n2,v2) << std::endl;
    return 0;
  }
