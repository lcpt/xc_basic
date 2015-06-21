//xMacaulay.cc

#include "matemat/funciones/src/macaulay/Macaulay.h"

int main(void)
  {
    Macaulay mc("x",1,3.0);
    std::cout << mc << std::endl;
    vZ_double v(2);
    for(int i= -10; i<= 10; i++)
      {
        std::cout << "Mc" << mc.Grado() 
             << '(' << i << ','
             << mc.GetA() << ")= " << mc(i) << std::endl;
      }
    Macaulay m2= mc*mc;
    std::cout << m2 << std::endl;
    std::cout << m2.Primitiva() << std::endl;
    std::cout << m2.Diferencial() << std::endl;
    std::cout << mc.Integral(-10.0,10.0) << std::endl;
    std::cout << mc.Derivada(7.0) << std::endl;
    std::cout << dot(mc,mc,-10,10) << std::endl;
    return 0;
  }
