//xmatsim.cpp
#include <iostream>
#include "matemat/matrices/src/matsimZ.h"

typedef matsimZ<double> ms_double;

int main(void)
  {
    ms_double m1(1,1.0);
    std::cout << m1 << std::endl;
    std::cout << m1 + m1 - m1 << std::endl;
    std::cout << "GetFila: " << m1.GetFila(1) << std::endl;
    std::cout << "GetCol: " << m1.GetCol(1) << std::endl;
    std::cout << 3.141592 * m1 << std::endl;
    //std::cout << 2*m1*m1*4 << std::endl;
    ms_double m2= m1;
    std::cout << (m1 == m2) << std::endl;
    ms_double m3(4,0.0);
    m3(1,1)= 3.0;m3(1,2)= 1.0;m3(1,3)= 2.0;m3(1,4)= -1.0;
    m3(2,1)= 5.0;m3(2,2)= -2.0;m3(2,3)= -3.0;m3(2,4)= 1.0;
    m3(3,1)= 1.0;m3(3,2)= 3.0;m3(3,3)= 4.0;m3(3,4)= -2.0;
    m3(4,1)= 2.0;m3(4,2)= -1.0;m3(4,3)= 1.0;m3(4,4)= -3.0;
    std::cout.precision(4);
    std::cout << m3 << std::endl;
    std::cout << "Traza= " << m3.Traza() << std::endl;
    std::cout << "Menor(1,1)= " << m3.GetMenor(1,1) << std::endl;
    std::cout << "Menor(1,4)= " << m3.GetMenor(1,4) << std::endl;
    std::cout << "Menor(4,1)= " << m3.GetMenor(4,1) << std::endl;
    std::cout << "Menor(4,4)= " << m3.GetMenor(4,4) << std::endl;
    std::cout << "Menor(2,2)= " << m3.GetMenor(2,2) << std::endl;
    return 0;
  }
