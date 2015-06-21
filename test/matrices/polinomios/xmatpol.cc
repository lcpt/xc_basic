//xmatpol.cpp
#include "matemat/funciones/src/polinomios/Polinomio.h"
#include "matemat/matrices/src/matrizZ.h"


typedef matrizZ<Polinomio> mZ_pol;

void fN(void)
  {
    mZ_pol _Ni(2,3);
    Polinomio N1;
    IndPol v1= N1.GetIndPol();
    v1[0]= 0; N1[v1]= 0.5;
    v1[0]= 1; N1[v1]= -0.5;
    _Ni(1,1)= N1;
    _Ni(2,2)= N1;
    _Ni(2,3)= N1;
    mZ_pol _N2= _Ni;
    std::cout << _N2 << std::endl;
  }
int main(void)
  {
    fN();
    return 0;
  }
