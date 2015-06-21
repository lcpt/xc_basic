//xmatpol4.cc
#include "matemat/funciones/src/polinomios/Polinomio.h"
#include "matemat/matrices/src/matrizPolmio.h"
#include "matemat/matrices/src/m_double.h"

void fN(void)
  {
    matrizPolmio _Ni(2,3);
    Polinomio cero;
    IndPol v1= cero.GetIndPol();
    v1[0]= 0; cero[v1]= 0.0;
    Polinomio N1;
    v1= N1.GetIndPol();
    v1[0]= 0; N1[v1]= 0.5;
    v1[0]= 1; N1[v1]= -0.5;
    _Ni(1,1)= N1;
    _Ni(2,2)= N1;
    _Ni(2,3)= N1;
    matrizPolmio _N2= _Ni;
    std::cout << "N2= " << _N2 <<std::endl;
    std::cout << "Eval(_N2,1,3.0)= " << Eval(_N2,1,3.0) << std::endl;
    Polinomio N2("y");
    v1= N1.GetIndPol();
    v1[0]= 0; N2[v1]= 0.5;
    v1[0]= 1; N2[v1]= -0.5;
    matrizPolmio N3_(1,1);
    N3_(1,1)= N2;
    std::cout << Eval(_N2,1,N2) << std::endl;
    std::cout << Eval(_N2,N3_) << std::endl;
    std::cout << Primitiva(_N2,1) << std::endl;
    std::cout << Diferencial(_N2,1) << std::endl;
    std::cout << Integral(_N2,1,0.0,1.0) << std::endl;
    Polinomio N4("y");
    std::cout << Integral(_N2,1,N4,N2) << std::endl;
    mZ_double _D(2,3);
    _D(1,1)= 1.0; _D(1,2)= 2.0; _D(1,3)= 3.0;
    _D(2,1)= 4.0; _D(2,2)= 5.0; _D(2,3)= 6.0;
    std::cout << _N2 + _D << std::endl;
    std::cout << _D - _N2 << std::endl;
  }
int main(void)
  {
    fN();
    return 0;
  }
