//xpolmio.cc
#include "matemat/funciones/src/polinomios/Polinomio.h"

int main(void)
  {
    Polinomio p("c");p[1]= -0.5; p[0]=0.5;
    Polinomio q("x");q[1]= 0.2; q[0]=1.0;
    std::cout << p << std::endl;
    std::cout << q << std::endl;
    q= q + 7.0;
    std::cout << q << std::endl;
    return 0;
  }
