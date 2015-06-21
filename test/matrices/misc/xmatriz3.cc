//#include <conio.h>
#include <iostream>
#include "matemat/matrices/src/matrizZ.h"
#include "tiempo/src/timer.h"

int main(void)
  {
    matrizZ<double> m3(4,4,0.0);
    m3(1,1)= 3.0;m3(1,2)= 1.0;m3(1,3)= 2.0;m3(1,4)= -1.0;
    m3(2,1)= 5.0;m3(2,2)= -2.0;m3(2,3)= -3.0;m3(2,4)= 1.0;
    m3(3,1)= 1.0;m3(3,2)= 3.0;m3(3,3)= 4.0;m3(3,4)= -2.0;
    m3(4,1)= 2.0;m3(4,2)= -1.0;m3(4,3)= 1.0;m3(4,4)= -3.0;
    std::cout.precision(4);
    matrizZ<double> m4(4,4,0.0);
    const size_t num_sumas= 100000;
    Timer t;
    t.start();
    for(register size_t i=1;i<=num_sumas;i++)
      m4= m4+(m3*m3);
    TimeElapsed elap= t.stop();
    std::cout << "resueltas: " << num_sumas << " sumas, en "
             << elap.getRealTime() << ' ' << elap.getUnidades() << std::endl;
    std::cout << m4 << std::endl;
    return 0;
  }
