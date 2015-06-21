//xExprAlgebra1.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"

int main(void)
  {
    ExprAlgebra expr0("3*Fmac0(x,2)");
    std::cout << "expr0= " << expr0 << std::endl;
    for(int i=-3;i<=5;i++)
      {
        const double x= double(i);
	std::cout << " expr0(" << x << ")= " << expr0.Eval("x",x).ToNum() << std::endl;
      }
    ExprAlgebra expr1("-1/4*Fmac2(x,-1)");
    std::cout << "expr1= " << expr1 << std::endl;
    for(int i=-3;i<=5;i++)
      {
        const double x= double(i);
	std::cout << " expr1(" << x << ")= " << expr1.Eval("x",x).ToNum() << std::endl;
      }
    return 0;
  }
