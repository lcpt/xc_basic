//xpvect.cpp
#include "matemat/matrices/src/m_double.h"

int main(void)
  {
    m_double i_(3,1);
    i_(1,1)= 1;
    m_double j_(3,1);
    j_(2,1)= 1;
    m_double k_(3,1);
    k_(3,1)= 1;
    std::cout << (i_ ^ j_) << std::endl;
    std::cout << (j_ ^ k_) << std::endl;
    std::cout << (k_ ^ i_) << std::endl;
    m_double a(3,1,0.0);
    m_double b(3,1,0.0);
    std::cout << (a ^ b) << std::endl;
    return 0;
  }
