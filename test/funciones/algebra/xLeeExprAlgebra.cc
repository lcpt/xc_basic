//xInfija.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    ExprAlgebra expr; //"(s1+x) * (-t) - (7 + 3)";
    std::cin >> expr;
    std::cout << "expr= " << expr << std::endl;
    return 1;
  }
