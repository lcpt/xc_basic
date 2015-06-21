//xGetToken.cc

#include "streams/src/imanip.h"

int main(void)
  {
    const char delim= '|';
    int i;
    std::string str;
    float f;
    double d;
    std::cin >> get_token(f,delim) >> get_token(str,delim) >> get_token(i,delim) >> get_token(d,delim);
    std::cout << "i= " << i << std::endl;
    std::cout << "str= " << str << std::endl;
    std::cout << "f= " << f << std::endl;
    std::cout << "d= " << d << std::endl;
    return 0;
  }
