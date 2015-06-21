//xPow.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    ExprAlgebra expr("abs(-7)");
    std::cout << "expr= " << expr << "= " << expr.ToNum()<< std::endl;
    return 0;
  }
