//xExprAlgebra9.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    ExprAlgebra expr("exp(2) - 3 * sqr(2)");
    std::cout << expr << std::endl;
    std::cout << expr.Eval() << std::endl;
    ExprAlgebra expr2("tonum(exp(2) - 3 * sqr(2))");
    std::cout << expr2 << std::endl;
    ExprAlgebra expr3("(exp(2) - tonum(3 * sqr(2)))");
    std::cout << expr3 << std::endl;
    std::cout << expr3.Eval() << std::endl;
    ExprAlgebra expr4("(exp(2) - tonum(3 * sqr(z)))");
    std::cout << expr4 << std::endl;
    std::cout << expr4.Eval() << std::endl;
    return 1;
  }
