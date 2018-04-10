#include <iostream>
#include "xc_basic/src/text/tab_cod.h"

int main(void)
  {
    std::string str;
    getline(std::cin,str);
    std::cout << pc8TOlatin1(str) << std::endl;
    return 0;
  }
