//xPruebaMemoria.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"
#include "matemat/funciones/src/algebra/Lexico.h"
#include "matemat/funciones/src/algebra/TablaLiterales.h"

const size_t numCiclos= 1000000;
const std::string str= "(s1+x) * (-t) - (7 + 3)";
int func(void)
  {
    std::cout << "Tamaño de la tabla de literales antes: " << ExprAlgebra::Lex().getTablaLiterales().size() << std::endl;
    ExprAlgebra expr0("2+2");
    std::cout << expr0 << std::endl;
    std::cout << expr0.ToNum() << std::endl;
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
    ExprAlgebra expr5(str);
    std::cout << "Tamaño de la tabla de literales después: " << ExprAlgebra::Lex().getTablaLiterales().size() << std::endl;
    return 1;
  }

int main(void)
  {
    func();
    std::cout << "Tamaño de la tabla de literales final fase 1: " << ExprAlgebra::Lex().getTablaLiterales().size() << std::endl;
    func();
    std::cout << "Tamaño de la tabla de literales final fase 2: " << ExprAlgebra::Lex().getTablaLiterales().size() << std::endl;
    for(size_t i= 0;i<numCiclos;i++)
      ExprAlgebra expr(str);
    return 1;
  }
