//xcrout.cc
#include "matemat/matrices/src/solvers/crout.h"

int main(void)
  {
    matrizZ<double> a,b,x;
    int reg;
    std::cin >> a;
    std::cout << "a= " << a << std::endl;
    std::cin >> b;
    std::cout << "b= " << b << std::endl;
    x= crout(a,b,reg);
    if (!reg)
      std::cout << " matriz singular" << std::endl;
    else
      std::cout << "b/a= " << x << std::endl;
    return 0;
  }
