//xorto.cc
#include "matemat/matrices/src/matrizZ.h"
#include "matemat/matrices/src/solvers/orto.h"

int main(void)
  {
    matrizZ<double> a,b,x;
    int reg;
    std::cin >> a;
    std::cout << "a= " << a << std::endl;
    std::cin >> b;
    std::cout << "b= " << b << std::endl;
    x= ortoresol(a,b,reg);
    if (!reg)
      std::cout << " matriz singular" << std::endl;
    else
      std::cout << "b/a= " << x << std::endl;
    return 0;
  }

