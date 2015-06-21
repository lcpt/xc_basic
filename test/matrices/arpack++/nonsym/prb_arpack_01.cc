//prb_arpack_01.cc

#include <iostream>
#include "matemat/matrices/src/matdispZ.h"
#include "arpack++/arbnsmat.h"
#include "arpack++/arbgnsym.h"

typedef matdispZ<double> msp_double;


ARbdNonSymMatrix<double, double> creaARbdNonSymMatrix(const msp_double &m,double *v)
  {
    const size_t ndiagL= m.ndiagL();
    const size_t ndiagU= m.ndiagU();
    const size_t ncols= m.GetFilas();
    const size_t sizeCol= (ndiagL+ndiagU+1);
    const size_t sizeV= sizeCol*ncols;
    v= new double[sizeV];
    for(register size_t j= 0;j<sizeV;j++)
      v[j]= 0.0;
    
    m.FillVectorBanda(v);
    std::cout << "ndiagL= " << ndiagL << std::endl;
    std::cout << "ndiagU= " << ndiagU << std::endl;
    std::cout << "sizeCol= " << sizeCol << std::endl;
    std::cout << "sizeV= " << sizeV << std::endl;
    std::cout << "vector= ";
    for(size_t j= 0;j<sizeV;j++)
      std::cout << v[j] << ' ';
    std::cout << std::endl;
    return ARbdNonSymMatrix<double, double>(m.getNumCols(),ndiagL,ndiagU,v);
  }


void test_01(void)
  {
    msp_double A(6,6);
    A(1,1)= 4.0;A(1,2)= 3.0;
    A(2,1)= 1.0;A(2,2)= 6.0;A(2,3)= 2.0;
    A(3,1)= -1.0;A(3,2)= 2.0;A(3,3)= 1.0;A(3,4)= 4.0;
    A(4,3)= 3.0;A(4,4)= -2.0;A(4,5)= -1.0;
    A(5,4)= 1.0;A(5,5)= 3.0;A(5,6)= 1.0;
    A(6,4)= 4.0;A(6,5)= 2.0;A(6,6)= 5.0;
    std::cout << A << std::endl;
    const size_t ndiagL= A.ndiagL();
    const size_t ndiagU= A.ndiagU();
    const size_t ncols= A.GetFilas();
    const size_t sizeCol= (ndiagL+ndiagU+1);
    const size_t sizeV= sizeCol*ncols;
    std::cout << "ndiagL= " << ndiagL << std::endl;
    std::cout << "ndiagU= " << ndiagU << std::endl;
    std::cout << "sizeCol= " << sizeCol << std::endl;
    std::cout << "sizeV= " << sizeV << std::endl;
    double *nzval= new double[sizeV];
    A.FillVectorBanda(nzval);
    for(size_t j= 0;j<sizeV;j++)
      std::cout << nzval[j] << ' ';
    std::cout << std::endl;
    const double v[]= {0.0,4.0,1.0,-1.0,3.0,6.0,2.0,0.0,2,1,3,0,4,-2,1,4,-1,3,2,0,1,5,0,0};
    for(size_t j= 0;j<sizeV;j++)
      assert(nzval[j]==v[j]);
    delete [] nzval;
  }
void test_02(void)
  {
    msp_double M(6,6);
    M(1,1)= 19.5;
    M(2,2)= 19.5;
    M(3,3)= 1.95e-09;
    M(4,4)= 9.75;
    M(5,5)= 9.75;
    M(6,6)= 9.75e-10;
    std::cout << M << std::endl;
    const size_t ndiagL= M.ndiagL();
    const size_t ndiagU= M.ndiagU();
    const size_t ncols= M.GetFilas();
    const size_t sizeCol= (ndiagL+ndiagU+1);
    const size_t sizeV= sizeCol*ncols;
    std::cout << "ndiagL= " << ndiagL << std::endl;
    std::cout << "ndiagU= " << ndiagU << std::endl;
    std::cout << "sizeCol= " << sizeCol << std::endl;
    std::cout << "sizeV= " << sizeV << std::endl;
    double *nzval= new double[sizeV];
    M.FillVectorBanda(nzval);
    for(size_t j= 0;j<sizeV;j++)
      std::cout << nzval[j] << ' ';
    std::cout << std::endl;
    const double v[]= {19.5,19.5,1.95e-09,9.75,9.75,9.75e-10};
    for(size_t j= 0;j<sizeV;j++)
      assert(nzval[j]==v[j]);
    delete [] nzval;
  }

void test_03()
  {
    msp_double k(4,4);
    k(1,1)= 5.0; k(1,2)= -4; k(1,3)= 1.0;
    k(2,1)= -4; k(2,2)= 6; k(2,3)= -4; k(2,4)= 1;
    k(3,1)= 1;k(3,2)= -4; k(3,3)= 6; k(3,4)= -4;
    k(4,2)= 1; k(4,3)= -4; k(4,4)= 5;
    std::cout << "k= " << k << std::endl;
    msp_double m(4,4);
    m(1,1)= 2.0;
    m(2,2)= 2.0;
    m(3,3)= 1.0;
    m(4,4)= 1.0;
    std::cout << "m= " << m << std::endl;

    double *vk= NULL;
    ARbdNonSymMatrix<double, double> ak=  creaARbdNonSymMatrix(k,vk);
    std::cout << "Num. filas ak: " << ak.nrows() << std::endl;
    std::cout << "Num. columnas ak: " << ak.ncols() << std::endl;
    double *vm= NULL;
    ARbdNonSymMatrix<double, double> am=  creaARbdNonSymMatrix(m,vm);
    std::cout << "Num. filas am: " << am.nrows() << std::endl;
    std::cout << "Num. columnas am: " << am.ncols() << std::endl;

    ARluNonSymGenEig<double> dprob(2, ak, am); //Los autovalores más grandes.
    //ARluNonSymGenEig<double> dprob(2, ak, am,0.0); //Los autovalores más proximos a 0.0 (los más pequeños).

    // Finding eigenvalues and eigenvectors.
    dprob.FindEigenvectors();

    // Printing solution.
    const size_t nconv= dprob.ConvergedEigenvalues();
    const size_t n= dprob.GetN();
    std::vector<double> EigVal(nconv,0.0);
    for(size_t i=0; i<nconv; i++)
      EigVal[i]= dprob.EigenvalueReal(i);

    std::vector<std::vector<double> > EigVec(nconv,std::vector<double>(n,0.0));
    for(size_t i=0; i<nconv; i++)
      for(size_t j=0; j<n;j++)
        EigVec[i][j]= dprob.RawEigenvector(i)[j];

    std::cout << "Autovalores: " << std::endl;
    for(size_t i= 0;i<nconv;i++)
      std::cout << "Eigenvalue[" << (i+1) << "]= " << EigVal[i] << std::endl;
    std::cout << "Autovectores: " << std::endl;
    for(size_t i= 0;i<nconv;i++)
      {
        std::cout << "Eigenvector[" << (i+1) << "]= ";
        const std::vector<double> &eig= EigVec[i];
        for(size_t j=0; j<n;j++)
	  std::cout << eig[j] << ' ';
	std::cout << std::endl;
      }
  }
int main(void)
  {
    //test_01();
    //test_02();
    //test_03();
    msp_double k(30,30);
k(1,1)= 2e+10; k(2,1)= -1.93249e-06; k(4,1)= -1e+10; k(5,1)= 9.66247e-07; k(6,1)= -2.5e+08; k(1,2)= -2.86195e-06; k(2,2)= 2e+10; k(4,2)= 1.43098e-06; k(5,2)= -1e+10; k(6,2)= 4.33013e+08; k(3,3)= 6.66667e+07; k(4,3)= 2.5e+08; k(5,3)= -4.33013e+08; k(6,3)= 1.66667e+07; k(1,4)= -1e+10; k(2,4)= 9.66247e-07; k(3,4)= 2.5e+08; k(4,4)= 2e+10; k(5,4)= 4.16301e-07; k(6,4)= 1.49012e-07; k(7,4)= -1e+10; k(8,4)= -1.38255e-06; k(9,4)= -2.5e+08; k(1,5)= 1.43098e-06; k(2,5)= -1e+10; k(3,5)= -4.33013e+08; k(4,5)= -4.47035e-07; k(5,5)= 2e+10; k(6,5)= -2.38419e-07; k(7,5)= -9.83942e-07; k(8,5)= -1e+10; k(9,5)= 4.33013e+08; k(1,6)= -2.5e+08; k(2,6)= 4.33013e+08; k(3,6)= 1.66667e+07; k(4,6)= 1.49012e-07; k(5,6)= -2.38419e-07; k(6,6)= 6.66667e+07; k(7,6)= 2.5e+08; k(8,6)= -4.33013e+08; k(9,6)= 1.66667e+07; k(4,7)= -1e+10; k(5,7)= -1.38255e-06; k(6,7)= 2.5e+08; k(7,7)= 2e+10; k(8,7)= -1.17812e-06; k(9,7)= -2.38419e-07; k(10,7)= -1e+10; k(11,7)= 2.56067e-06; k(12,7)= -2.5e+08; k(4,8)= -9.83942e-07; k(5,8)= -1e+10; k(6,8)= -4.33013e+08; k(7,8)= -1.3737e-06; k(8,8)= 2e+10; k(9,8)= 2.98023e-07; k(10,8)= 2.35764e-06; k(11,8)= -1e+10; k(12,8)= 4.33013e+08; k(4,9)= -2.5e+08; k(5,9)= 4.33013e+08; k(6,9)= 1.66667e+07; k(7,9)= -2.38419e-07; k(8,9)= 2.98023e-07; k(9,9)= 6.66667e+07; k(10,9)= 2.5e+08; k(11,9)= -4.33013e+08; k(12,9)= 1.66667e+07; k(7,10)= -1e+10; k(8,10)= 2.56067e-06; k(9,10)= 2.5e+08; k(10,10)= 2e+10; k(11,10)= -1.07242e-06; k(12,10)= 3.27826e-07; k(13,10)= -1e+10; k(14,10)= -1.48825e-06; k(15,10)= -2.5e+08; k(7,11)= 2.35764e-06; k(8,11)= -1e+10; k(9,11)= -4.33013e+08; k(10,11)= -7.57631e-07; k(11,11)= 2e+10; k(12,11)= -2.98023e-07; k(13,11)= -1.60001e-06; k(14,11)= -1e+10; k(15,11)= 4.33013e+08; k(7,12)= -2.5e+08; k(8,12)= 4.33013e+08; k(9,12)= 1.66667e+07; k(10,12)= 3.27826e-07; k(11,12)= -2.98023e-07; k(12,12)= 6.66667e+07; k(13,12)= 2.5e+08; k(14,12)= -4.33013e+08; k(15,12)= 1.66667e+07; k(10,13)= -1e+10; k(11,13)= -1.48825e-06; k(12,13)= 2.5e+08; k(13,13)= 2e+10; k(14,13)= 4.06988e-06; k(15,13)= -5.96046e-08; k(19,13)= -1e+10; k(20,13)= -2.58163e-06; k(21,13)= -2.5e+08; k(10,14)= -1.60001e-06; k(11,14)= -1e+10; k(12,14)= -4.33013e+08; k(13,14)= 3.93717e-06; k(14,14)= 2e+10; k(15,14)= -1.78814e-07; k(19,14)= -2.33715e-06; k(20,14)= -1e+10; k(21,14)= 4.33013e+08; k(10,15)= -2.5e+08; k(11,15)= 4.33013e+08; k(12,15)= 1.66667e+07; k(13,15)= -5.96046e-08; k(14,15)= -1.78814e-07; k(15,15)= 6.66667e+07; k(19,15)= 2.5e+08; k(20,15)= -4.33013e+08; k(21,15)= 1.66667e+07; k(16,16)= 1e+10; k(17,16)= -2.56067e-06; k(18,16)= 2.5e+08; k(22,16)= -1e+10; k(23,16)= 2.56067e-06; k(24,16)= 2.5e+08; k(16,17)= -2.35764e-06; k(17,17)= 1e+10; k(18,17)= -4.33013e+08; k(22,17)= 2.35764e-06; k(23,17)= -1e+10; k(24,17)= -4.33013e+08; k(16,18)= 2.5e+08; k(17,18)= -4.33013e+08; k(18,18)= 3.33333e+07; k(22,18)= -2.5e+08; k(23,18)= 4.33013e+08; k(24,18)= 1.66667e+07; k(13,19)= -1e+10; k(14,19)= -2.58163e-06; k(15,19)= 2.5e+08; k(19,19)= 2e+10; k(20,19)= 2.09548e-08; k(21,19)= -2.68221e-07; k(25,19)= -1e+10; k(26,19)= 2.56067e-06; k(27,19)= -2.5e+08; k(13,20)= -2.33715e-06; k(14,20)= -1e+10; k(15,20)= -4.33013e+08; k(19,20)= -2.04891e-08; k(20,20)= 2e+10; k(21,20)= 4.76837e-07; k(25,20)= 2.35764e-06; k(26,20)= -1e+10; k(27,20)= 4.33013e+08; k(13,21)= -2.5e+08; k(14,21)= 4.33013e+08; k(15,21)= 1.66667e+07; k(19,21)= -2.68221e-07; k(20,21)= 4.76837e-07; k(21,21)= 6.66667e+07; k(25,21)= 2.5e+08; k(26,21)= -4.33013e+08; k(27,21)= 1.66667e+07; k(16,22)= -1e+10; k(17,22)= 2.56067e-06; k(18,22)= -2.5e+08; k(22,22)= 2e+10; k(23,22)= 2.4219e-06; k(24,22)= -6.55651e-07; k(28,22)= -1e+10; k(29,22)= -4.98258e-06; k(30,22)= 2.5e+08; k(16,23)= 2.35764e-06; k(17,23)= -1e+10; k(18,23)= 4.33013e+08; k(22,23)= 2.33296e-06; k(23,23)= 2e+10; k(24,23)= 5.36442e-07; k(28,23)= -4.69061e-06; k(29,23)= -1e+10; k(30,23)= -4.33013e+08; k(16,24)= 2.5e+08; k(17,24)= -4.33013e+08; k(18,24)= 1.66667e+07; k(22,24)= -6.55651e-07; k(23,24)= 5.36442e-07; k(24,24)= 6.66667e+07; k(28,24)= -2.5e+08; k(29,24)= 4.33013e+08; k(30,24)= 1.66667e+07; k(19,25)= -1e+10; k(20,25)= 2.56067e-06; k(21,25)= 2.5e+08; k(25,25)= 2e+10; k(26,25)= -5.12134e-06; k(28,25)= -1e+10; k(29,25)= 2.56067e-06; k(30,25)= -2.5e+08; k(19,26)= 2.35764e-06; k(20,26)= -1e+10; k(21,26)= -4.33013e+08; k(25,26)= -4.71529e-06; k(26,26)= 2e+10; k(28,26)= 2.35764e-06; k(29,26)= -1e+10; k(30,26)= 4.33013e+08; k(19,27)= -2.5e+08; k(20,27)= 4.33013e+08; k(21,27)= 1.66667e+07; k(27,27)= 6.66667e+07; k(28,27)= 2.5e+08; k(29,27)= -4.33013e+08; k(30,27)= 1.66667e+07; k(22,28)= -1e+10; k(23,28)= -4.98258e-06; k(24,28)= -2.5e+08; k(25,28)= -1e+10; k(26,28)= 2.56067e-06; k(27,28)= 2.5e+08; k(28,28)= 2e+10; k(29,28)= 2.4219e-06; k(30,28)= 6.55651e-07; k(22,29)= -4.69061e-06; k(23,29)= -1e+10; k(24,29)= 4.33013e+08; k(25,29)= 2.35764e-06; k(26,29)= -1e+10; k(27,29)= -4.33013e+08; k(28,29)= 2.33296e-06; k(29,29)= 2e+10; k(30,29)= -5.36442e-07; k(22,30)= 2.5e+08; k(23,30)= -4.33013e+08; k(24,30)= 1.66667e+07; k(25,30)= -2.5e+08; k(26,30)= 4.33013e+08; k(27,30)= 1.66667e+07; k(28,30)= 6.55651e-07; k(29,30)= -5.36442e-07; k(30,30)= 6.66667e+07;
//std::cout << "k= " << k << std::endl;
    msp_double m(30,30);
m(1,1)= 3.9; m(2,2)= 3.9; m(3,3)= 3.9e-10; m(4,4)= 3.9; m(5,5)= 3.9; m(6,6)= 3.9e-10; m(7,7)= 3.9; m(8,8)= 3.9; m(9,9)= 3.9e-10; m(10,10)= 3.9; m(11,11)= 3.9; m(12,12)= 3.9e-10; m(13,13)= 3.9; m(14,14)= 3.9; m(15,15)= 3.9e-10; m(16,16)= 1.95; m(17,17)= 1.95; m(18,18)= 1.95e-10; m(19,19)= 3.9; m(20,20)= 3.9; m(21,21)= 3.9e-10; m(22,22)= 3.9; m(23,23)= 3.9; m(24,24)= 3.9e-10; m(25,25)= 3.9; m(26,26)= 3.9; m(27,27)= 3.9e-10; m(28,28)= 3.9; m(29,29)= 3.9; m(30,30)= 3.9e-10; 
//std::cout << "m= " << m << std::endl;

    double *vk= NULL;
    ARbdNonSymMatrix<double, double> ak=  creaARbdNonSymMatrix(k,vk);
    std::cout << "Num. filas ak: " << ak.nrows() << std::endl;
    std::cout << "Num. columnas ak: " << ak.ncols() << std::endl;
    double *vm= NULL;
    ARbdNonSymMatrix<double, double> am=  creaARbdNonSymMatrix(m,vm);
    std::cout << "Num. filas am: " << am.nrows() << std::endl;
    std::cout << "Num. columnas am: " << am.ncols() << std::endl;

    ARluNonSymGenEig<double> dprob(2, ak, am); //Los autovalores más grandes.
    //ARluNonSymGenEig<double> dprob(2, ak, am,0.0); //Los autovalores más proximos a 0.0 (los más pequeños).

    // Finding eigenvalues and eigenvectors.
    dprob.FindEigenvectors();

    // Printing solution.
    const size_t nconv= dprob.ConvergedEigenvalues();
    const size_t n= dprob.GetN();
    std::vector<double> EigVal(nconv,0.0);
    for(size_t i=0; i<nconv; i++)
      EigVal[i]= dprob.EigenvalueReal(i);

    std::vector<std::vector<double> > EigVec(nconv,std::vector<double>(n,0.0));
    for(size_t i=0; i<nconv; i++)
      for(size_t j=0; j<n;j++)
        EigVec[i][j]= dprob.RawEigenvector(i)[j];

    std::cout << "Autovalores: " << std::endl;
    for(size_t i= 0;i<nconv;i++)
      std::cout << "Eigenvalue[" << (i+1) << "]= " << EigVal[i] << std::endl;
    std::cout << "Autovectores: " << std::endl;
    for(size_t i= 0;i<nconv;i++)
      {
        std::cout << "Eigenvector[" << (i+1) << "]= ";
        const std::vector<double> &eig= EigVec[i];
        for(size_t j=0; j<n;j++)
	  std::cout << eig[j] << ' ';
	std::cout << std::endl;
      }
 }
