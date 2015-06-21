//test_biseccion
#include "matemat/funciones/src/raices/bisection.h"

inline double expr(const double &l,const double *params)
  { return sqr(l)-9.0; }

inline double expr2(const double &l,const double *params)
  { return sqr(l)-16; }

int main(void)
  {
    const double low= 0.0;
    const double high= 100.0;
    const double tol= 1e-8; //Error admitido.
    double root= 0;
    SolverBiseccion solver(low, high, tol);
    std::cout << "raiz= " << solver.Solve(root,NULL,expr) << std::endl;
    std::cout << "error: " << solver.GetError() << std::endl;
    std::cout << "raiz= " << solver.Solve(root,NULL,expr2) << std::endl;
    std::cout << "error: " << solver.GetError() << std::endl;
    return 0;
  }
