//xel_decomp.cc

#include <iostream>
#include "xc_basic/src/texto/en_letra.h"

int main(void)
  {
    dq_sint lista= el_decomp(1234567);
    for(dq_sint::const_iterator i=lista.begin(); i!=lista.end();i++)
      std::cout << *i << std::endl;
    return 0;
  }
