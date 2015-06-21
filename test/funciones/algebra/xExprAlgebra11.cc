//xExprAlgebra9.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    ExprAlgebra expr("sin(3.141592/4)");
    std::cout << expr << std::endl;
    std::cout << expr.Eval() << std::endl;
    ExprAlgebra expr2("max(2,sin(3.141592/4))");
    std::cout << expr2 << std::endl;
    std::cout << expr2.Eval() << std::endl;
    ExprAlgebra expr3("min(max(2,sin(3.141592/4)),0.5)");
    std::cout << expr3 << std::endl;
    std::cout << expr3.Eval() << std::endl;
    return 1;
  }
