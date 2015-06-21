//potiter.cc

#include "matemat/matrices/src/matrizZ.h"
#include "matemat/matrices/src/solvers/potiter.h"
#include "matemat/matrices/src/solvers/gj_aux.h"

int main(void)
  {
    matrizZ<double> a;
    std::cin >> a;
    std::cout << a << std::endl;
    std::cout << valorp(a) << std::endl;
    return 0;
  }
