//xvectorz.cpp
#include "matemat/funciones/src/polinomios/Polinomio.h"

void InfoPol(Polinomio &p)
  {
    std::cout << "Grado (" << p << ") = " << p.Grado() << std::endl;    
    std::cout << "Dimension (" << p << ")= " << p.GetDim() << std::endl;
  }

int main(void)
  {
    Polinomio p1;
    IndPol v1= p1.GetIndPol();
    v1[0]= 0; p1[v1]= 1.0;
    v1[0]= 1; p1[v1]= -2.0;
    v1[0]= 2; p1[v1]= 3.0;
    Polinomio p2("yz");
    IndPol v2= p2.GetIndPol();
    v2[0]= 0; v2[1]= 0; p2[v2]= 1.0;
    v2[0]= 1; v2[1]= 0; p2[v2]= 2.0;
    v2[0]= 0; v2[1]= 1; p2[v2]= -3.0;
    v2[0]= 1; v2[1]= 1; p2[v2]= 4.0;
    v2[0]= 2; v2[1]= 0; p2[v2]= 5.0;
    v2[0]= 0; v2[1]= 2; p2[v2]= -6.0;
    v2[0]= 2; v2[1]= 2; p2[v2]= 7.0;
    InfoPol(p1);
    InfoPol(p2);
    std::cout << (p2 == p2) << std::endl;
    std::cout << p2 + 3.0 << std::endl;
    std::cout << "Suma p1 + p1= " << p1 + p1 << std::endl;
    std::cout << "Suma p2 + p2= " << p2 + p2 << std::endl;
    std::cout << "Suma p1 + p2= " << p1 + p2 << std::endl;
    std::cout << "Suma p2 + p1= " << p2 + p1 << std::endl;
    std::cout << "Producto p1 * p1: " << p1 * p1 << std::endl;
    std::cout << "Producto p2 * p2: " << p2 * p2 << std::endl;
    std::cout << "Producto p1 * p2: " << p1 * p2 << std::endl;
    std::cout << "Producto p2 * p1: " << p2 * p1 << std::endl;
    std::cout << "Parcial: " << p2.Parcial(1) << std::endl;
    return 0;
  }




