//xgauss.cc
#include "matemat/matrices/src/solvers/gauss.h"

int main(void)
  {
    matrizZ<double> a,b,x;
    int reg;
    std::cin >> a;
    std::cout << "a= " << a << std::endl;
    x= gauss_invierte(a,reg);
    if (!reg)
      std::cout << " matriz singular" << std::endl;
    else
      {
        std::cout << "comprobamos: " << std::endl;
        matrizZ<double> c=a*x;
        std::cout << "a*x= " << c << std::endl;
      }
    return 0;
  }
