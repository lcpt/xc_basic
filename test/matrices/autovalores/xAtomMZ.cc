//xsouriau.cc
#include "matemat/matrices/src/autovalores/souriau.h"

size_t i,j,n;
int main(void)
  { 
    std::cin >> n;
    matrizZ<double> a(n,n);
    for(i=1;i<=n;i++)
      for (j=1;j<=n;j++)
        std::cin >> a(i,j);
    vectorZ<double> t= souriau(a);
    std::cout << a << std::endl;
    std::cout << t << std::endl;
    return 0;
  }
