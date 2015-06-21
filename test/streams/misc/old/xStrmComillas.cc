//xStrComillas.cc

#include "streams/src/StrmComillas.h"

int main(void)
  {
    StrmComillas strm;
    strm= std::cin;
    int i;
    std::string str;
    StrComillas strc;
    float f;
    double d;
    strm >> i >> str >> strc >> f >> d;
    std::cout << i << std::endl;
    std::cout << str << std::endl;
    std::cout << strc << std::endl;
    std::cout << f << std::endl;
    std::cout << d << std::endl;
    return 0;
  }
