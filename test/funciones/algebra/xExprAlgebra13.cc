//xExprAlgebra3.cc
#include "matemat/funciones/src/algebra/ExprAlgebra.h"
#include "matemat/funciones/src/algebra/ConjVariables.h"
#include "tiempo/src/timer.h"
#include "boost/lexical_cast.hpp"

double expr_cpp(const double &s1,const double &x,const double &t)
  { return ((s1+x) * (-t) - (7 + 3)); }

const size_t numCiclos= 100000;

int main(void)
  {

    Timer t(true);
    t.start();
    for(size_t i= 0;i<numCiclos;i++)
      ExprAlgebra expr("(s1+x) * (-t) - (7 + 3)");
    t.stop();
    std::cout << "test A: cada operación de construcción tardó: " << double(t.getTime().getUserTime())/numCiclos << ' ' << t.getTime().getUnidades() << std::endl;


    t.reset();
    t.start();
    for(size_t i= 0;i<numCiclos;i++)
      ExprAlgebra expr("15.0");
    t.stop();
    std::cout << "test B: cada operación de construcción tardó: " << double(t.getTime().getUserTime())/numCiclos << ' ' << t.getTime().getUnidades() << std::endl;

    t.reset();
    t.start();
    for(size_t i= 0;i<numCiclos;i++)
      { ExprAlgebra(15.0); }
    t.stop();
    std::cout << "test C: cada operación de construcción tardó: " << double(t.getTime().getUserTime())/numCiclos << ' ' << t.getTime().getUnidades() << std::endl;


    ExprAlgebra expr("(s1+x) * (-t) - (7 + 3)");
    std::cout << expr << std::endl;
    expr.Asigna("t",-5.0);
    expr.Asigna("s1",3.0);
    expr.Asigna("x",2.0);
    //std::cout << "expr(s1=3,x=2,t= -5)= " << expr.ToNum()<< " Variables: " << expr.Variables() << " Num. variables: " << expr.NumVariables() << std::endl;
    double tmp= 0.0;
    t.reset();
    t.start();
    for(size_t i= 0;i<numCiclos;i++)
      tmp= expr.ToNum();
    t.stop();
    std::cout << "Cada operación tardó: " << double(t.getTime().getUserTime())/numCiclos << std::endl;
    std::cout << "tmp= " << tmp << std::endl;

    t.reset();
    t.start();
    for(size_t i= 0;i<numCiclos;i++)
      tmp= expr_cpp(3.0,2.0,-5.0);
    t.stop();
    std::cout << "Cada operación tardó: " << double(t.getTime().getUserTime())/numCiclos << std::endl;
    std::cout << "tmp= " << tmp << std::endl;

    ExprAlgebra expr2= expr.ToNum();
    t.reset();
    t.start();
    for(size_t i= 0;i<numCiclos;i++)
      tmp= expr2.ToNum();
    t.stop();
    std::cout << "Cada operación tardó: " << double(t.getTime().getUserTime())/numCiclos << std::endl;
    std::cout << "tmp= " << tmp << std::endl;

    
    return 1;
  }
