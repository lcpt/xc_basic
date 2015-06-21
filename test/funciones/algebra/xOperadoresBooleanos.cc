//xExprAlgebra8.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

void muestra(const ExprAlgebra &expr)
  {
    std::cout << expr << std::endl;
    std::cout << expr.Eval() << std::endl;
  }

int main(void)
  {
    ExprAlgebra expr=std::string("(1<2.2) and (4<8)");
    muestra(expr);
    expr=std::string("(1<2) and (8<4)");
    muestra(expr);
    expr=std::string("(1<2) or (8<4)");
    muestra(expr);
    expr=std::string("not(8<4)");
    muestra(expr);
    expr=std::string("(1<2) and not(8<4)");
    muestra(expr);
    expr=std::string("(1<2) or (4<8)");
    muestra(expr);
    return 1;
  }
