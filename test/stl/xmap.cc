//xmap.cpp
#include <iostream>
#include <string>
#include <map>

int main(void)
  {
    std::map < std::string,int,std::less<std::string> > m;
    m["Juan"]= 1;
    m["Alfonso"]= 3;
    m["Manuel"]= 5;
    m["Santiago"]= 7;
    std::cout << "Valor de Santiago= " << m["Santiago"] << std::endl;
    return 1;
  }
