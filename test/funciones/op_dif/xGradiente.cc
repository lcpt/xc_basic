//xGradiente.cc

#include "matemat/funciones/src/op_dif/op_dif.h"

double f(const m_double &x)
  { return x(2)*x(2)+x(1); }

int main(void)
  {
    m_double x(2,1);
    x(1)= 50.0;
    x(2)= 5.0;
    std::cout << gradiente_cdf(x,&f) << std::endl;
    return 0;
  }
