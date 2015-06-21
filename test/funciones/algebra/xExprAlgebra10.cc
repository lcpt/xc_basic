//xExprAlgebra9.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    ExprAlgebra expr("(exp(2) - 3 * sqr(2))*sin(a)");
    std::cout << expr.GetFullString() << std::endl;
    ExprAlgebra expr2("2*(-3.14159265358979323846)");
    std::cout << expr2.GetFullString() << std::endl;
    return 1;
  }
