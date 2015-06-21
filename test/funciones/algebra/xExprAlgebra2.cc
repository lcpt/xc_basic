//xExprAlgebra2.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
//     const ExprAlgebra a1("(s1+x) * (-t) - (7 + 3)");
//     cout << "a1=" << a1 << endl;
//     const ExprAlgebra a3= a1("x","y + z");
//     cout << "a1(x= y + z)=" << a3 << endl;
    const ExprAlgebra a1("x");
    std::cout << "a1=" << a1 << std::endl;
    const ExprAlgebra a3= a1(std::string("x"),std::string("y + z"));
    std::cout << "a1(x= y + z)= " << a3 << std::endl;
    return 1;
  }
