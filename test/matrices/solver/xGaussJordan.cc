//xGaussJordan.cpp
#include "matemat/matrices/src/matrizZ.h"
#include "matemat/matrices/src/solvers/gauss.h"

int main(void)
  {
    //Ejemplo tomado de la pag 365 del libro de Puy.
    matrizZ<double> a(4,4,0.0);
    a(1,1)= 3.0;a(1,2)= 1.0;a(1,3)= 2.0;a(1,4)= -1.0;
    a(2,1)= 5.0;a(2,2)= -2.0;a(2,3)= -3.0;a(2,4)= 1.0;
    a(3,1)= 1.0;a(3,2)= 3.0;a(3,3)= 4.0;a(3,4)= -2.0;
    a(4,1)= 2.0;a(4,2)= -1.0;a(4,3)= 1.0;a(4,4)= -3.0;
    matrizZ<double> b(4,1,0.0);
    b(1,1)= 5;b(2,1)=2;b(3,1)=3;b(4,1)=1;
    std::cout << a << std::endl;
    std::cout << b << std::endl;
    std::cout << b/a << std::endl;
    return 0;
  }
