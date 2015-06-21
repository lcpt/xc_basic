//xsouriau.cc

#include "matemat/matrices/src/autovalores/souriau.h"

int main(void)
  {
    matrizZ<double> a;
    std::cin >> a;
    std::cout << a << std::endl;
    std::cout << souriau(a) << std::endl;
    return 0;
  }
