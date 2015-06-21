//xExprTrigonometricas.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    ExprAlgebra expr=std::string("deg2rad(180)");
    std::cout << expr.Eval() << std::endl;
    expr=std::string("rad2deg(3.141592)");
    std::cout << expr.Eval() << std::endl;
    expr=std::string("sin(deg2rad(45))");
    std::cout << expr.Eval() << std::endl;
    expr=std::string("cos(deg2rad(45))");
    std::cout << expr.Eval() << std::endl;
    expr=std::string("tan(deg2rad(45))");
    std::cout << expr.Eval() << std::endl;
    expr=std::string("cosec(deg2rad(45))");
    std::cout << expr.Eval() << std::endl;
    expr=std::string("sec(deg2rad(45))");
    std::cout << expr.Eval() << std::endl;
    expr=std::string("cotg(deg2rad(45))");
    std::cout << expr.Eval() << std::endl;
    return 1;
  }
