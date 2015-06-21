//xcholsky.cc
#include "matemat/matrices/src/solvers/cholsky.h"

int main(void)
  {
    matsimZ<double> a;
    matrizZ<double> b,x;
    int reg;
    std::cin >> a;
    std::cout << "a= " << a << std::endl;
    std::cin >> b;
    std::cout << "b= " << b << std::endl;
    x= cholesky(a,b,reg);
    if (!reg)
      std::cout << " matriz singular" << std::endl;
    else
      std::cout << "b/a= " << x << std::endl;
    return 0;
  }
