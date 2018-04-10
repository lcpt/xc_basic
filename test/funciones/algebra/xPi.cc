//xExprAlgebra.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"
#include "text/src/StrTok.hxx"

int main(void)
  {
    ExprAlgebra expr=std::string("2*PI(1)");
    std::cout << "expr= " << expr << std::endl;
    std::cout << "expr (GetValor)= " << expr.ToNum() << std::endl;
    return 0;
  }
