//xExprAlgebra1.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"
#include "matemat/funciones/src/algebra/LexAlgebra.h"

int main(void)
  {
    ExprAlgebra expr2("&1+&2");
    std::cout << "expr2= " << expr2 << std::endl;
    ExprAlgebra expr3("d+e");
    std::cout << "&a= " << expr3 << std::endl;
    expr2.Asigna("&1",expr3);
    std::cout << "expr2= " << expr2 << std::endl;
    expr2.LexA().Asigna("d",1);
    expr2.LexA().Asigna("e",-1);
    expr2.LexA().Asigna("&2",3.0);
    //expr2.LexA().PrintVariables(std::cout);
    std::cout << "result= " << expr2.ToNum() << std::endl;
    return 0;
  }
