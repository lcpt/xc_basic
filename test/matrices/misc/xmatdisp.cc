//xmatdisp.cpp
#include <iostream>
#include "matemat/matrices/src/matdispZ.h"
#include "matemat/matrices/src/m_double.h"

typedef matdispZ<double> msp_double;

int main(void)
  {
    msp_double m3(4,4);
    m3(1,1)= 3.0;m3(1,2)= 1.0;m3(1,3)= 2.0;m3(1,4)= -1.0;
    m3(2,1)= 5.0;m3(2,2)= -2.0;m3(2,3)= -3.0;m3(2,4)= 1.0;
    m3(3,1)= 1.0;m3(3,2)= 3.0;m3(3,3)= 4.0;m3(3,4)= -2.0;
    m3(4,1)= 2.0;m3(4,2)= -1.0;m3(4,3)= 1.0;m3(4,4)= -3.0;
    std::cout << m3 << std::endl;
    std::cout << m3.GetFila(3) << std::endl;
    /*
    msp_double m4(m3);
    std::cout << m4.Traza() << std::endl;
    std::cout << m4.GetCompleta().Traza() << std::endl;
    std::cout << m4.GetMenor(1,1) << std::endl;
    msp_double m5(4,1);
    m5(1,1)= 1.0; m5(2,1)= 2.0;m5(3,1)= 3.0;
    msp_double m6(4,1);
    m6(1,1)= 3.0; m6(2,1)= 2.0;m6(3,1)= 1.0;
    std::cout << "m5= " << m5 << std::endl;
    std::cout << "m6= " << m6 << std::endl;
    std::cout << "Producto escalar: " << dot(m6.GetTrn(),m5) << std::endl;
    std::cout << "Producto: " << m3*m5 << std::endl;
    return 0;
    */
  }
