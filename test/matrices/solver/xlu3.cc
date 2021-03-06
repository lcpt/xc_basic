//xlu.cpp
#include "matemat/matrices/src/matsimZ.h"
#include "matemat/matrices/src/solvers/solver_lu.h"

typedef matsimZ<double> ms_double;
typedef matrizZ<double> m_double;

int main(void)
  {
    ms_double a(4,0.0);
    a(1,1)= 5.0;a(1,2)= -4.0;a(1,3)= 1.0;a(1,4)= 0.0;
    a(2,1)= -4.0;a(2,2)= 6.0;a(2,3)= -4.0;a(2,4)= 1.0;
    a(3,1)= 1.0;a(3,2)= -4.0;a(3,3)= 6.0;a(3,4)= -4.0;
    a(4,1)= 0.0;a(4,2)= 1.0;a(4,3)= -4.0;a(4,4)= 5.0;
    std::cout << a << std::endl;
    ms_double A(a);
    SolverSimLU<double,m_double> solver(1);
    solver.PutMatriz(a);
    solver.Decomp();
    std::cout << a << std::endl;
    m_double b(4,1,0.0);
    b(2,1)= 1.0;
    m_double x= solver.BackSub(b);
    std::cout << x << std::endl;
    std::cout << "Comprueba: " << A*x-b << std::endl;
    return 0;
  }
