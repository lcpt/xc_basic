//Primera prueba de la biblioteca STL;

#include <vector>
#include <iterator>
#include <iostream>

int main(void)
  {
    std::vector<char> a(5,'b');
    std::vector<char> b(5);
    a[0]= 'a';
    b=a;
    copy(a.begin(),a.end(),std::ostream_iterator<char>(std::cout));
    std::cout << std::endl;
    std::cout << a[0] << " , " << b[0] << std::endl;
    std::cout << a[1] << " , " << b[1] << std::endl;
    return 0;
  } 
