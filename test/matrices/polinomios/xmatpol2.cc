//xmatpol2.cc
#include "matemat/funciones/src/polinomios/Polinomio.h"
#include "matemat/matrices/src/matrizZ.h"

typedef matrizZ<Polinomio> mZ_pol;

int main(void)
  {
    mZ_pol _Ni(2,3);
    std::cout << _Ni <<std::endl;
    return 0;
  }
