//Primera prueba de la biblioteca STL;

#include <vector>
#include <iterator>
#include <iostream>

int main(void)
  {
    std::vector<int> a(5,4);
    std::vector<int> b(5);
    a[0]= 1;
    b=a;
    copy(a.begin(),a.end(),std::ostream_iterator<int>(std::cout));
    std::cout << std::endl;
    std::cout << a[0] << " , " << b[0] << std::endl;
    std::cout << a[1] << " , " << b[1] << std::endl;
    return 0;
  } 
