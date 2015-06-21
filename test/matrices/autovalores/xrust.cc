//xrust.cc
#include "matemat/matrices/src/autovalores/descomp.h"
#include "matemat/matrices/src/autovalores/compos.h"
#include "matemat/matrices/src/autovalores/rust.h"

int main(void)
  {
    double epsi=1e-3;
    matrizZ<double> a;
    std::cout << "Introduzca la matriz: ";
    std::cin >> a;
    std::cout << "Matriz introducida: " << a << std::endl;
    rust(a,epsi);
    std::cout << epsi << std::endl;
    std::cout << a << std::endl;
    return 0;
  }
