#include <iostream>
#include "xc_basic/src/texto/cadena_carac.h"
#include "xc_basic/src/texto/en_letra.h"


int main(void)
  {
    std::cout << isNumber("-w") << "\n";
    const std::string str("-3.6277"); 
    std::cout << isNumber(str) << "\n";
    return 0;
  }
