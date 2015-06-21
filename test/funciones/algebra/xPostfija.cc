//xPostfija.cc
#include "matemat/funciones/src/algebra/ExprPostfija.h"

int main(void)
  {
    ExprPostfija expr;
    std::cin >> expr;
    std::cout << "expr= " << expr << std::endl;
    ExprPostfija expr2("(s1+x) * (-t) - (7 + 3)");
    std::cout << "expr2= " << expr2 << std::endl;
    return 1;
  }
