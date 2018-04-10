//symbol.cc
#include <iostream>
#include "xc_basic/src/text/latex.h"

int main(void)
  {
     for(int i= 0;i<256;i++)
       std::cout << i << ' ' << ltx_symbol(i) << std::endl << std::endl;
    return 0;
  }

