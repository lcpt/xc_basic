//acentos.cc
#include <iostream>
#include "xc_basic/src/streams/imanip.h"
#include "xc_basic/src/text/tab_cod.h"

int main(void)
  {
    char c;
    do
      {
        c= char(std::cin.get());
        if(std::cin.eof()) break;
        std::cout << int(c) << " " << c << " " << iso8859_1TOlatex(c) << std::endl;
      }
    while(!std::cin.eof());
    return 0;
  }

