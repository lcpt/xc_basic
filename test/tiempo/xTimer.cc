//xTimer.cc

using namespace std;
#include "tiempo/src/timer.h"
#include "matemat/matem.h"

int main(void)
  {
    Timer t;
    t.start();
    double a(1e3);
    size_t i(1);
    for(;i<=1000000;i++)
      {
        a= tan(sqrt(a));
      }
    t.stop();
    std::cout << "i= " << i << std::endl;
    std::cout << "La operación tardó: " << t.getTime() << std::endl;
    return 0;
  }
