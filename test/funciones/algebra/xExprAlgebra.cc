//xExprAlgebra.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"
#include "texto/src/StrTok.hxx"

int main(void)
  {
    ExprAlgebra expr("(s1+x) * (-t) - (7 + 3)");
    std::cout << "expr= " << expr << std::endl;
    std::cout << "expr(t= -5)= " << expr("t",-5.0) << std::endl;
    expr.Asigna("s1",3.0);
    expr.Asigna("x",2.0);
    std::cout << "expr(s1=3,x=2,t= -5)= " << expr.Eval() << std::endl;
    ExprAlgebra expr2("a+b");
    std::cout << "expr2= " << expr2 << std::endl;
    ExprAlgebra expr3("d+e");
    expr2.Asigna("a",expr3);
    std::cout << "expr2= " << expr2 << std::endl;
    ExprAlgebra expr4("(s1+x) * (-t) - (7 + 3)");
    std::cout << "expr4= " << expr4 << std::endl;
    std::cout << "expr4= " << expr4.Eval() << std::endl;
    std::cout << "expr4 (GetValor)= " << expr4.ToNum() << std::endl;
    ExprAlgebra expr5("2.1e6");
    std::cout << "expr5 (GetValor)= " << expr5.ToNum() << std::endl;
    return 0;
  }
