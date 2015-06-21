//xPol2.cc
#include "matemat/funciones/src/polinomios/Polinomio.h"

void InfoPol(Polinomio &p)
  {
    cout << "Grado (" << p << ") = " << p.Grado() << endl;    
    cout << "Dimension (" << p << ")= " << p.GetDim() << endl;
  }

int main(void)
  {
    Polinomio p("xy");
    IndPol v= p.GetIndPol();
    v[0]= 0; v[1]= 0; p[v]= 1.0;
    v[0]= 1; v[1]= 0; p[v]= 2.0;
    v[0]= 0; v[1]= 1; p[v]= -3.0;
    v[0]= 1; v[1]= 1; p[v]= 4.0;
    v[0]= 2; v[1]= 0; p[v]= 5.0;
    v[0]= 0; v[1]= 2; p[v]= -6.0;
    v[0]= 2; v[1]= 2; p[v]= 7.0;
    cout << "Polinomio: " << p;
    cout << "Variable 1: " << p.GetNmbVar(1) << endl;
    InfoPol(p);
    cout << "Dim: " << p.GetDim() << endl;
    cout << "Grado: " << p.Grado() << endl;
    cout << "Igualdad: " << (p == p) << endl;
    cout << "Suma con un real: " << p + 3.0 << endl;
    cout << "Suma p+p= " << p + p << endl;
    cout << "Producto p*p= " << p * p << endl;
    cout << "Parcial x: " << p.Parcial(1) << endl;
    cout << "Parcial y: " << p.Parcial(2) << endl;
    cout << "Primitiva x: " << p.Primitiva(1) << endl;
    cout << "Primitiva y: " << p.Primitiva(2) << endl;
    cout << "Eval(x=1.0)= " << p.Eval(1,1.0) << endl;
    vectorZ<double> x(2);
  }



