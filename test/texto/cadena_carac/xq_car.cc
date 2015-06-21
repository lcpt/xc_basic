#include <iostream>
#include "xc_basic/src/texto/cadena_carac.h"

int main(void)
  {
    std::string s= "xxxABCDxxx";
    std::cout << "Cadena original: '" << s << "'" << std::endl;
    s= q_car(s,'x');
    std::cout << "Quitamos el caracter 'x' a derecha e izquierda." << std::endl;
    std::cout << "Y queda: '" << s << "'" << std::endl;
    return 0;
  }
