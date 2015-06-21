//#include <conio.h>
#include <iostream>
#include "matemat/matrices/src/matrizZ.h"

int main(void)
  {
    //clrscr();
    matrizZ<double> m1(1,1);
    m1(1,1)= 1;
    std::cout << m1 << std::endl;
    std::cout << m1 + m1 - m1 << std::endl;
    std::cout << "dot: " << dot(m1,m1) << std::endl;
    std::cout << "GetFila: " << m1.GetFila(1) << std::endl;
    std::cout << "GetCol: " << m1.GetCol(1) << std::endl;
    std::cout << 3.141592 * m1 << std::endl;
    std::cout << 2*m1*m1*4 << std::endl;
    matrizZ<double> m2= m1;
    std::cout << (m1 == m2) << std::endl; 
    matrizZ<double> m3(4,4,0.0);
    m3(1,1)= 3.0;m3(1,2)= 1.0;m3(1,3)= 2.0;m3(1,4)= -1.0;
    m3(2,1)= 5.0;m3(2,2)= -2.0;m3(2,3)= -3.0;m3(2,4)= 1.0;
    m3(3,1)= 1.0;m3(3,2)= 3.0;m3(3,3)= 4.0;m3(3,4)= -2.0;
    m3(4,1)= 2.0;m3(4,2)= -1.0;m3(4,3)= 1.0;m3(4,4)= -3.0;
    std::cout.precision(4);
    matrizZ<double> m4(1,1,2.0);
    std::cout << m4 << std::endl;
    m3.SumaCaja(1,1,m4);
    std::cout << m3 << std::endl;
    return 0;
  }
