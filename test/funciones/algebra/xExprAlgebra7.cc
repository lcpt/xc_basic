//xExprAlgebra7.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    ExprAlgebra expr("exp(x) - 3 * sqr(x)");
    std::cout << expr << std::endl;
    std::cout << expr.Eval("x", 1) << std::endl;
    std::cout << expr.ToNum("x", 2) << std::endl;

    ExprAlgebra expr1(1.0);
    std::cout << expr1.ToNum() << std::endl;
    std::cout << expr1.ToNum("t",1.0) << std::endl;
    return 1;
  }
