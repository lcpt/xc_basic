//xmatdisp.cpp
#include <iostream>
#include "matemat/funciones/src/estadisticas/combinatoria.h"

int main(void)
  {
    for(unsigned int m=2;m<=10;m++)
      {
        for(unsigned int n=0;n<=m;n++)
          std::cout << "  C(" << m << "," << n << ")= " << coeficiente_binomial(m,n);
        std::cout << std::endl;
      }
    return 0;
  }
