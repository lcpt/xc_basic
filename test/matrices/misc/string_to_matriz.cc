//xmatriz.cpp
#include <iostream>
#include "matemat/matrices/src/m_double.h"

int main(void)
  {
    m_double m1(1,2);
    const std::string str= "[[1.0,2.0][2.0,1.0][3.0,1.0]]";
    m1= string_to_m_double(str);
    std::cout << m1 << std::endl;
    return 0;
  }
