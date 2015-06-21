#include "tiempo/src/timer.h"
#include <iostream>
#include "matemat/matem.h"
#include <string>

int main(void)
  {
    Timer t;
    t.start();
    double a= 2.0;
    for(size_t i=1;i<=1000000;i++)
      {
        a+=double(i);
        std::cout << i << std::endl;
      }
    std::cout << "valor= " << a << std::endl;
    TimeElapsed elap= t.stop();
    const std::string &uds= elap.getUnidades();
    std::cout << "tiempo usuario = " << elap.getUserTime() << ' ' << uds << std::endl;
    std::cout << "tiempo sistema = " << elap.getSystemTime() << ' ' << uds << std::endl;
    std::cout << "tiempo real = " << elap.getRealTime() << ' ' << uds << std::endl;
    return 0;
  }
