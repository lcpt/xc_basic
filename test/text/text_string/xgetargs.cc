#include <iostream>
#include "xc_basic/src/text/text_string.h"
#include "xc_basic/src/text/en_letra.h"


int main(void)
  {
    std::string str("prueba(1,2,3,4,a,b,c.1,d,2))");
    std::cout << "Tiene paréntesis izdo.: " << bool2str(has_char(str,'(')) << std::endl;
    const std::deque<std::string> args= getargs(str);
    for(std::deque<std::string>::const_iterator beg=args.begin(); beg!=args.end();++beg)
      std::cout << *beg << "\n";
    return 0;
  }
