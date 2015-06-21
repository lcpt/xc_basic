//xpset.cpp
#include "../src/pset.h"
#include <iostream>

int main(void)
  {
    int val[]= {3,6,2,0,4,5,1,9,8,7};
      {
        pset<int> s;
        for (size_t i=0;i< sizeof(val)/sizeof(val[0]);i++)
          s.insert(s.end(),&val[i]);
        std::cout << s << std::endl;
        s.erase(s.begin(),s.end());
        std::cout << s << std::endl;
      }
    for (size_t i=0;i< sizeof(val)/sizeof(val[0]);i++)
      std::cout << val[i] << ' ';
    std::cout << std::endl;
    return 0;
  }
