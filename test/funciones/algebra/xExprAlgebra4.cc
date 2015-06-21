//xExprAlgebra4.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    const ExprAlgebra a1("t*0");
    std::cout << "a1= " << a1 << std::endl;
    std::cout << "a1.Colct(): " <<  a1.Colct() << std::endl;
    double d= a1.Colct().ToNum();
    std::cout << d << std::endl;
    return 1;
  }
