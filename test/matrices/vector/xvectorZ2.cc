//xvectorz.cpp
#include "matemat/matrices/src/vectorZ.h"
#include <sstream>

int main(void)
  {
    vectorZ<double> v(2);
    v[0]= 1.0;
    std::cout << v << std::endl;
    vectorZ<double> v2(v);
    std::cout << v2 << std::endl;
    v2[1]=2.0;
    v= v2;
    std::cout << v << std::endl;
    std::cout << (v == v2) << std::endl; 
    std::cout << (v != v2) << std::endl;
    v.PutSuma(1,1.0);
    v.PutResta(1,2.0);
    v.PutProd(1,4.0);
    std::cout << v << std::endl;
    std::cout << v2 << std::endl;
    std::cout << v + v2 << std::endl;
    std::cout << v - v2 << std::endl;
    std::cout << 2.0 * v << std::endl;
    const std::string strd("[4.0,3.0,2.0,1.0]");
    std::istringstream str(strd);
    str >> v;
    std::cout << "v= " << v << std::endl;
    std::cout << "Left= " << v.Left(2) << std::endl;
    std::cout << "Right= " << v.Right(2) << std::endl;
    std::cout << "Mid= " << v.Mid(1,3) << std::endl;
    std::cout << "Cat= " << Cat(v,v) << std::endl;
    std::cout << "Menor 0= " << v.GetMenor(0) << std::endl;
    std::cout << "Menor 1= " << v.GetMenor(1) << std::endl;
    std::cout << "Menor 2= " << v.GetMenor(2) << std::endl;
    std::cout << "Menor 3= " << v.GetMenor(3) << std::endl;
    std::cout << v.Sumatorio(0,4) << std::endl;
    v.Con(0.0);
    std::cout << "v= " << v << std::endl;
    std::cout << "Sustituye: " << v.Sustituye(1,v2) << std::endl;
    return 1;
  }
