//xPol6.cc
#include "matemat/funciones/src/polinomios/Polinomio.h"



int main(void)
  {
    Polinomio uno= Polinomio::neutro_producto();
    std::cout << "neutro producto: " << uno << std::endl;
    Polinomio p1;
    IndPol v1= p1.GetIndPol();
    v1[0]= 0; p1[v1]= 1.0;
    v1[0]= 1; p1[v1]= -2.0;
    v1[0]= 2; p1[v1]= 3.0;
    std::cout << "p1= " << p1 << std::endl;
    std::cout << "p1 * neutro_producto(): " << p1*uno << std::endl;
    Polinomio cero= Polinomio::neutro_suma();
    std::cout << "neutro suma: " << cero << std::endl;
    std::cout << "p1 * neutro_suma(): " << p1*cero << std::endl;
    return 0;
  }




