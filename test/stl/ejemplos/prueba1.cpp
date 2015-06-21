//Primera prueba de la biblioteca STL;

#include "vector.h"

int main(void)
  {
    vector<int> a(5);
    vector<int> b(5);
    a[0]= 1;
    b=a;
    cout << a[0] << " , " << b[0] << endl;
    return 0;
  } 
