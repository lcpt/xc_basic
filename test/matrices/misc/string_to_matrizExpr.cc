//xmatriz.cpp
#include <iostream>
#include "matemat/matrices/src/matrizExpr.h"

int main(void)
  {
    matrizExpr m1(1,2);
    const std::string str= "[['1.0','2.0*x']['2.0','1.0+y']['3.0','1.0']]";
    m1= string_to_matrizExpr(str);
    std::cout << m1 << std::endl;
    return 0;
  }
