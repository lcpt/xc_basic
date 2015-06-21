//xExprAlgebra1.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"
#include "matemat/funciones/src/algebra/integ_num.h"

int main(void)
  {
    ExprAlgebra expr0=std::string("exp(x)-3*x^2");
    std::cout << "expr0= " << expr0 << std::endl;
    ExprAlgebra integ_trap= expr0.IntegTrapecio(Intervalo1D("x",0.0,1.0,100));
    std::cout << "integ_trap= " << integ_trap.ToNum() << std::endl;
    ExprAlgebra integ_simpson= expr0.IntegSimpson(Intervalo1D("x",0.0,1.0,100));
    std::cout << "integ_simpson= " << integ_simpson.ToNum() << std::endl;
    return 0;
  }
