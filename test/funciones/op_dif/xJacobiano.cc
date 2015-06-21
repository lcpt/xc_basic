//xJacobiano.cc

#include "matemat/funciones/src/op_dif/op_dif.h"
#include "matemat/matrices/src/m_func.h"

double f1(const m_double &x)
  { return x(2)*x(2)+x(1); }

double f2(const m_double &x)
  { return x(1)*x(1)+x(2); }

double f3(const m_double &x)
  { return x(1)*x(1)+x(2)*x(2); }

int main(void)
  {
    m_double x(3,1);
    m_func mf(3,1);
    mf(1)= &f1;
    mf(2)= &f2;
    mf(3)= &f3;
    x(1)= 5.0;
    x(2)= 5.0;
    std::cout << jacobiano_cdf(x,mf) << std::endl;
    std::cout << jacobiano_cdf(x,mf)*mf.Eval(x) << std::endl;
    return 0;
  }
