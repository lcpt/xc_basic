//xExprAlgebra6.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    ExprAlgebra expr("exp(x) - 3 * sqr(x)");
    double err;
    double cero= expr.RaizNewton(1e-5,100,"x",3.0,err);
    std::cout << "Cero de " << expr << ": " << cero << " Error: " << err << std::endl;
    return 1;
  }
