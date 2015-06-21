//Primera prueba de la biblioteca STL;

#include <vector>
#include <iterator>
#include <iostream>

int main(void)
  {
    std::vector<double> a(5,4.0);
    std::vector<double> b(5,3.0);
    a[0]= 1.0;
    b=a;
    std::cout << "Suma de dos vectores: " << std::endl;
    transform(a.begin(),a.end(),b.begin(),a.begin(),std::plus<double>());
    copy(a.begin(),a.end(),std::ostream_iterator<double>(std::cout));
    std::cout << std::endl;
    std::cout << a[0] << " , " << b[0] << std::endl;
    std::cout << a[1] << " , " << b[1] << std::endl;
    return 0;
  } 
