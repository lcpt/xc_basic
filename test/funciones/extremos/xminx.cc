/* Output from p2c, the Pascal-to-C translator */
/* From input file "xminx.pas" */


#include "matemat/funciones/src/extremos/extremo.h"
#include "matemat/funciones/src/extremos/parametriza.h"
#include <iostream>
#include <cmath>

#define nmax            20


long i, n;
double a, b, dx;

//funciones a minimizar
inline double fln(const double &x)
  { return (x-1.5)*(x-1.5); }
double f(const m_double &x)
  { return 100*(x(2)-1.5)*(x(2)-1.5)+x(1); }

int main(void)
  {
    cin >> a >> b >> dx >> n;
    double x_min= extremo(a, b, true, &fln);
    std::cout << "El mínimo se produce en: " << x_min
         << " y es: " << fln(x_min) << std::endl;
    double x_max= extremo(a, b, false, &fln);
    std::cout << "El máximo se produce en: " << x_max
         << " y es: " << fln(x_max) << std::endl;
    m_double x(2,1);
    x(1)= 1.0;
    x(2)= 5.0;
    x(2)= extremo<typeof(&f),2>(a, b,true,&f,x);
    std::cout << "El mínimo se produce en: " << x(2)
         << " y es: " << f(x) << std::endl;
    x(2)= extremo<typeof(&f),2>(a, b,false,&f,x);
    std::cout << "El máximo se produce en: " << x(2)
         << " y es: " << f(x) << std::endl;
    return 0;
  }
