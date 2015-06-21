//xnum2str.cc

#include <iostream>
#include "xc_basic/src/texto/en_letra.h"

int main(void)
  {
    size_t d= 1015;
    std::string s= num2str(d);
    std::cout << "Entero: " << d << ' ' << s << std::endl;
    std::cout << "Coma flotante: " << 3.141592 << ' ' << num2str(3.141592,4) << std::endl;
    std::cout << "Coma flotante, parte entera: " << M_PI*1e12 << ' ' << ldbl2str_ip(M_PI*1e12) << std::endl;
    std::cout << "Coma flotante, parte fraccionaria: " << M_PI*1e12 << ' ' << ldbl2str_fp(M_PI*1e12) << std::endl;
    std::cout << "Booleano: " << true << ' ' << bool2str(true) << std::endl;
    return 0;
  }
