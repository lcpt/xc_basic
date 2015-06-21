//xlu.cpp
#include "matemat/matrices/src/matrizZ.h"
#include "matemat/matrices/src/solvers/solver_lu.h"

typedef matrizZ<double> m_double;
typedef matrizZ<size_t> m_sizet;

int main(void)
  {
    m_double a(4,4,0.0);
    m_sizet p;
    a(1,1)= 3.0;a(1,2)= 1.0;a(1,3)= 2.0;a(1,4)= -1.0;
    a(2,1)= 5.0;a(2,2)= -2.0;a(2,3)= -3.0;a(2,4)= 1.0;
    a(3,1)= 1.0;a(3,2)= 3.0;a(3,3)= 4.0;a(3,4)= -2.0;
    a(4,1)= 2.0;a(4,2)= -1.0;a(4,3)= 1.0;a(4,4)= -3.0;
    std::cout << a << std::endl;
    SolverConstLU<m_double,m_double> solver(1);
    solver.PutMatriz(a);
    solver.Decomp();
    std::cout << a << std::endl;
    m_double b(4,1,0.0);
    b(2,1)= 1.0;
    m_double x= solver.BackSub(b);
    std::cout << x << std::endl;
    std::cout << "Comprueba: " << a*x-b << std::endl;
    return 0;
  }
