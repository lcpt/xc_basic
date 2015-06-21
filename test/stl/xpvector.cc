//xpvector.cpp
#include "../src/pvector.h"
#include <iostream>

int main(void)
  {
    int val[]= {0,1,2,3,4,5,6,7,8,9};
      {
        pvector<int> l(sizeof(val)/sizeof(val[0]));
        for (size_t i=0;i< sizeof(val)/sizeof(val[0]);i++)
          l[i]= &val[i];
        std::cout << l << std::endl;
        l.erase(l.begin(),l.end());
        std::cout << l << std::endl;
      }
    for (size_t i=0;i< sizeof(val)/sizeof(val[0]);i++)
      std::cout << val[i] << ' ';
    std::cout << std::endl;
    return 0;
  }
