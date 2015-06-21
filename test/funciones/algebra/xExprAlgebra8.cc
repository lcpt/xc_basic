//xExprAlgebra8.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

void muestra(const ExprAlgebra &expr)
  {
    std::cout << expr << std::endl;
    std::cout << expr.Eval() << std::endl;
  }

int main(void)
  {
    ExprAlgebra expr(".53==.53");
    muestra(expr);
    expr= std::string("1<2");
    muestra(expr);
    expr=std::string("1<=1");
    muestra(expr);
    expr=std::string("2>1");
    muestra(expr);
    expr=std::string("1>=1");
    muestra(expr);
    expr=std::string("2 != 1");
    muestra(expr);

    return 1;
  }
