#include <iostream>
#include "xc_basic/src/text/en_letra.h"

int main(void)
  {
    //    unsigned long i= 101121001;
    //    cout << en_humano(i) << endl;
    //    cout << en_letra(i,false) << endl;
    long double d= 1000001015.4966; //001121.123;
    std::cout << en_humano(d,0) << std::endl;
    std::cout << en_letra(d,0,true) << std::endl;
//     cout << "en_humano(" << d << ")= " << en_humano(d) << endl;
//     cout << "en_letra(" << d << ")= " << en_letra(d,true) << endl;
    return 0;
  }
