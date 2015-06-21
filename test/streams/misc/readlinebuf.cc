// A simple test program for the readlinebuf
#include "streams/src/readlinebuf.h"
#include <istream>

int main()
  {
    std::istream in( new readlinebuf( "\ncmd>") );
    char c;
    while(true)
      {
        in >> c;
	std::cout << c;
      }
  }
