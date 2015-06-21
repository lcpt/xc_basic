//xvectorz.cpp
#include <iostream>
#include "matemat/funciones/src/polinomios/funcion.h"

int main(void)
  {
    Funcion f;
    std::cout << f.GetNmbVar(1) << std::endl;
    Funcion f1("xyz");
    std::cout << f1.GetNmbVar(3) << std::endl;
    Funcion f2(f1);
    std::cout << f2.GetNmbVar(3) << std::endl;
    Funcion f3= f2;
    std::cout << f3.GetNmbVar(3) << std::endl;
    f1.CambioVar(0,NmbVars("abc"));
    std::cout << f1.GetVars() << std::endl;
    f2.CambioVar(1,NmbVars("abc"));
    std::cout << f2.GetVars() << std::endl;
    f3.CambioVar(2,NmbVars("abc"));
    std::cout << f3.GetVars() << std::endl;
    f3= f1 + f2;
    std::cout << f3.GetVars() << std::endl;
    return 1;
  }
