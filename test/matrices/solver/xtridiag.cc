//xtridiag.cc
#include "matemat/matrices/src/solvers/tridiag.h"

int main(void)
  {
    matridZ<double> a;
    matrizZ<double> b,x;
    int reg;
    std::cin >> a;
    std::cout << "a= " << a << std::endl;
    std::cin >> b;
    std::cout << "b= " << b << std::endl;
    x= tridiag(a,b,reg);
    if (!reg)
      std::cout << " matriz singular o mEtodo inadecuado." << std::endl;
    else
      std::cout << "b/a= " << x << std::endl;
    return 0;
  }
