//xjordan.cc
#include "matemat/matrices/src/solvers/jordan.h"

int main(void)
  {
    matrizZ<double> a,b,x;
    int reg;
    std::cin >> a;
    std::cout << "a= " << a << std::endl;
    std::cin >> b;
    std::cout << "b= " << b << std::endl;
    x= jordan_const(a,b,reg);
    if (!reg)
      std::cout << " matriz singular" << std::endl;
    else
      {
        std::cout << "comprobamos: " << std::endl;
        std::cout << "b/a= " << x << std::endl;
        matrizZ<double> c=a*x;
        std::cout << "a*x= " << c << std::endl;
        std::cout << "a*x-b= " << c-b << std::endl;
      }
    return 0;
  }
