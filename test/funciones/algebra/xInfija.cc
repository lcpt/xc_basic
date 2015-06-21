//xInfija.cc
#include "matemat/funciones/src/algebra/ExprInfija.h"
#include "matemat/funciones/src/algebra/LexAlgebra.h"

int main(void)
  {
    ExprInfija expr; //"(s1+x) * (-t) - (7 + 3)";
    std::cin >> expr;
    std::cout << "expr= " << expr << std::endl;
    //ExprInfija expr2(&lex_algebra,"(s1+x) * (-t) - (7 + 3)");
    //std::cout << "expr2= " << expr2 << std::endl;
    return 1;
  }
