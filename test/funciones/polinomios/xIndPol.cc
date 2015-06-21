//xIndPol.cpp
#include "matemat/funciones/src/polinomios/IndPol.h"

int main(void)
  {
    IndPol v1(3);
    IndPol v2(3);
    v1[0]= 3;v1[1]= 0;v1[2]= 0;
    v2[0]= 0;v2[1]= 0;v2[2]= 1;
    std::cout << v2 << " < " << v1 << "= " << (v2<v1) << std::endl;
    std::cout << v1 << " == " << v2 << "= " << (v1==v2) << std::endl;
    std::cout << v2 << " > " << v1 << "= " << (v2>v1) << std::endl;
    std::cout << v2 << " < " << v2 << "= " << (v2<v2) << std::endl;
    v1[0]= 1;v1[1]= 0;v1[2]= 0;
    std::cout << v1 << " < " << v2 << "= " << (v1<v2) << std::endl;
    return 0;
  }
