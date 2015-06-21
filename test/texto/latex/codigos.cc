//codigos.cc
#include <iostream>
#include "xc_basic/src/streams/imanip.h"
#include "xc_basic/src/texto/latex.h"

int main(void)
  {
    char c;
    while(1)
      {
        c= char(std::cin.get());
        if(std::cin.eof()) break;
        std::cout << int(c) << " " << char(c) << std::endl;
      }
    return 0;
  }

