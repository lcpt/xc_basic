#include "matemat/matrices/src/vectorZ.h"

typedef vectorZ<size_t> v_szt;

int main(void)
  {
    const size_t a[]= {3,3,3};
    std::cout << sizeof(a) << std::endl;
    std::cout << sizeof(a[0]) << std::endl;
    std::cout << sizeof(a)/sizeof(a[0]) << std::endl;
    v_szt v(sizeof(a)/sizeof(a[0]),a);
    std::cout << v << std::endl;
    return 0;
  }
