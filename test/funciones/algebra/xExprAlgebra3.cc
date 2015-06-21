//xExprAlgebra3.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    const ExprAlgebra a1("(s1+x) * (-t) - (7 + 3)");
    std::cout << "a1= " << a1 << std::endl;
    const ExprAlgebra a2("y+z+k");
    std::cout << "a2= " << a2 << std::endl;
    ExprAlgebra a3= a1 * a2;
    std::cout << "a3= " << -a3 << std::endl;
    return 1;
  }
