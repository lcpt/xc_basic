//xPol4.cc
#include "matemat/funciones/src/polinomios/Polinomio.h"

int main(void)
  {
    Polinomio p("xy");
    IndPol v= p.GetIndPol();
    v[0]= 0; v[1]= 0; p[v]= 1.0;
    v[0]= 1; v[1]= 0; p[v]= 1.0;
    v[0]= 0; v[1]= 1; p[v]= 1.0;
    v[0]= 1; v[1]= 1; p[v]= 1.0;
    v[0]= 2; v[1]= 0; p[v]= 1.0;
    v[0]= 0; v[1]= 2; p[v]= 1.0;
    Polinomio p1("a");
    IndPol v1= p1.GetIndPol();
    v1[0]= 0; p1[v1]= 1.0;
    v1[0]= 1; p1[v1]= 1.0;
    std::cout << "p1= " << p1 << std::endl;
    std::cout << "p= "<< p << std::endl;
    Polinomio q= p.Eval(1,p1);
    std::cout << "p(x=p1)= " << q << std::endl;
    std::cout << "Variables: " << q.GetVars() << std::endl;
    std::cout << "Grados: " << q.Grados() << std::endl;
    std::cout << "Grado: " << q.Grado() << std::endl;
    q= p.Eval(2,p1);
    std::cout << "p(y=p1)= " << q << std::endl;
    std::cout << "Variables: " << q.GetVars() << std::endl;
    std::cout << "Grados: " << q.Grados() << std::endl;
    std::cout << "Grado: " << q.Grado() << std::endl;
    q= p.Eval(1,p1);
    q= q.Eval(1,p1);
    std::cout << "p(x=p1,y=p1)= " << q << std::endl;
    std::cout << "Variables: " << q.GetVars() << std::endl;
    std::cout << "Grados: " << q.Grados() << std::endl;
    std::cout << "Grado: " << q.Grado() << std::endl;
    return 1;
  }



