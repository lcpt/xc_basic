#include <unistd.h>
#include <iostream>
#include "streams/src/ifdbuf.h"
    
int main(int argc)
  {
    ifdstreambuf fds(0);
    std::istream is(&fds);
    std::cout << is.rdbuf();
    return 0;
  }        

