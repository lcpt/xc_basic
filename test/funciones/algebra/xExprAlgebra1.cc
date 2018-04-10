//xExprAlgebra1.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"
#include "text/src/StrTok.hxx"

int main(void)
  {
    ExprAlgebra expr2("a+b");
    std::cout << "expr2= " << expr2 << std::endl;
    ExprAlgebra expr3("d+e");
    std::cout << "a= " << expr3 << std::endl;
    expr2.Asigna("a",expr3);
    std::cout << "expr2= " << expr2 << std::endl;
    return 0;
  }
