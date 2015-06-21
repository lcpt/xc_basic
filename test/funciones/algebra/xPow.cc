//xPow.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"
#include "texto/src/StrTok.hxx"

int main(void)
  {
    ExprAlgebra expr=std::string("2^2");
    std::cout << "expr= " << expr << "= " << expr.ToNum()<< std::endl;
    return 0;
  }
