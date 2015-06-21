//xmatdisp.cpp
#include <iostream>
#include "matemat/funciones/src/estadisticas/combinatoria.h"

int main(void)
  {
    for(unsigned int i=1;i<=64;i++)
      std::cout << i << "!= " << factorial(i) << std::endl;
    return 0;
  }
