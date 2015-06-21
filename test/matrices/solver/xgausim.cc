//xgausim.cc
#include "matemat/matrices/src/solvers/gausim.h"

int main(void)
  {
    matrizZ<double> b,x;
    matsimZ<double> a;
    int reg;
    std::cin >> a;
    std::cout << "a= " << a << std::endl;
    std::cin >> b;
    std::cout << "b= " << b << std::endl;
    x= gausim(a,b,reg);
    if (!reg)
      std::cout << " matriz singular" << std::endl;
    else
      std::cout << "b/a= " << x << std::endl;
    return 0;
  }
