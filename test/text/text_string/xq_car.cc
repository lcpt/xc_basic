#include <iostream>
#include "xc_basic/src/text/text_string.h"

int main(void)
  {
    std::string s= "xxxABCDxxx";
    std::cout << "Original string: '" << s << "'" << std::endl;
    s= q_car(s,'x');
    std::cout << "Erase character 'x' from left and right." << std::endl;
    std::cout << "And we get: '" << s << "'" << std::endl;
    return 0;
  }
