//xExprAlgebra.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"
#include "matemat/funciones/src/algebra/ConjVariables.h"


int main(void)
  {
    ExprAlgebra expr("(s1+x) * (-t) - (7 + 3)");
    std::cout << "expr= " << expr << " Variables: " << expr.Variables() << std::endl;
    ExprAlgebra expr2(expr("t",-5.0));
    std::cout << "expr(t= -5)= " << expr2 << " Variables: " << expr2.Variables() << std::endl;
    expr.Asigna("s1",3.0);
    expr.Asigna("x",2.0);
    std::cout << "expr(s1=3,x=2,t= -5)= " << expr.Eval()<< " Variables: " << expr.Variables() << " Num. variables: " << expr.NumVariables() << std::endl;
    return 1;
  }
