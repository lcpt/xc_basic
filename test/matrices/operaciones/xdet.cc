//xdet.cpp
#include <iostream>
#include "matemat/matrices/src/m_double.h"

int main(void)
  {
    m_double m3;
    std::cin >> m3;
    std::cout << "Matriz: " << std::endl << m3 << std::endl;
    std::cout << "Determinante lento: " << m3.GetDetLento() << std::endl;
    std::cout << "Determinante : " << m3.GetDet() << std::endl;
    return 0;
  }
