//test_biseccion

#include "matemat/funciones/src/func_por_puntos/FuncPorPuntosR_R.h"

int main(void)
  {
    FuncPorPuntosR_R fp;
    fp.Inserta(0,0);
    fp.Inserta(1,1);
    fp.Inserta(2,1.5);
    fp.Inserta(5,-1);
    std::cout << fp << std::endl;
    for(int i=-10;i<=10;i++)
      {
        const double x= i;
        std::cout << "f(" << x << ")= " << fp(x) << std::endl;
      }
    std::cout << "MAXIMO:" << fp.Max() << std::endl;
    std::cout << "MINIMO:" << fp.Min() << std::endl;
    
    std::cout << "CEROS:" << std::endl;
    dq_double ceros= fp.Ceros();
    for(dq_double::const_iterator i= ceros.begin();i!=ceros.end();i++)
      std::cout << *i << std::endl;
    return 0;
  }
