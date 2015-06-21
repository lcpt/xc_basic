//xParcialFd.cc

#include "matemat/funciones/src/op_dif/op_dif.h"

double f(const m_double &x)
  { return x(2)*x(2); }

int main(void)
  {
    m_double x(2,1);
    x(1)= 0.0;
    x(2)= 5.0;
    std::cout << parcial_fd(x,&f,2) << std::endl;
    std::cout << parcial_cdf(x,&f,2) << std::endl;
    std::cout << parcial_fd(x,&f,2) - parcial_cdf(x,&f,2) << std::endl;
    return 0;
  }
