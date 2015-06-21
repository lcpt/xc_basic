//xlu.cpp
#include "matemat/matrices/src/matrizZ.h"
#include "matemat/matrices/src/solvers/solver_lu.h"

typedef matrizZ<long double> m_double;

int main(void)
  {
    m_double a(3,3,0.0);
    a(1,1)= 2.0;a(1,2)= 1.0;a(1,3)= -2.0;
    a(2,1)= 4.0;a(2,2)= 1.0;a(2,3)= -1.0;
    a(3,1)= 6.0;a(3,2)= -1.0;a(3,3)= 6.0;
    std::cout << a << std::endl;
    std::cout << "Determinante: " << a.GetDet() << std::endl;
    m_double A(a);
    SolverLU<m_double,m_double> solver;
    solver.PutMatriz(a);
    solver.Decomp();
    std::cout << a << std::endl;
    m_double b(3,1,0.0);
    b(1)= 4.0; 
    b(2)= 7.0;
    b(3)= 8.0;
    m_double x= solver.BackSub(b);
    std::cout << x << std::endl;
    std::cout << "Comprueba: " << A*x-b << std::endl;
    return 0;
  }
