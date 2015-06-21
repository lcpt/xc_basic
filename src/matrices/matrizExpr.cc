//----------------------------------------------------------------------------
//  biblioteca xc_basic; funciones y clases de uso general
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//matrizExpr.cc

#include "matrizExpr.h"

matrizExpr::matrizExpr(const m_double &m)
  : expr_matriz(m.getNumFilas(),m.getNumCols())
  {
    for(size_type i=1;i<=fls;i++)
      for(size_type j=1;j<=cls;j++)
        (*this)(i,j)= ExprAlgebra(m(i,j));
  }
void matrizExpr::eval(void)
  {
    for(size_type i=1;i<=fls;i++)
      for(size_type j=1;j<=cls;j++)
        (*this)(i,j).eval();
  }
void matrizExpr::eval(const char *palabra,const ExprAlgebra &a)
  {
    for(size_type i=1;i<=fls;i++)
      for(size_type j=1;j<=cls;j++)
        (*this)(i,j).eval(palabra,a);
  }
void matrizExpr::eval(const char *palabra,const double &d)
  {
    for(size_type i=1;i<=fls;i++)
      for(size_type j=1;j<=cls;j++)
        (*this)(i,j).eval(palabra,d);
  }
matrizExpr matrizExpr::Eval(void) const
  {
    matrizExpr retval(*this);
    retval.eval();
    return retval;
  }
matrizExpr matrizExpr::Eval(const char *palabra,const ExprAlgebra &a)
  {
    matrizExpr retval(*this);
    retval.eval(palabra,a);
    return retval;
  }
matrizExpr matrizExpr::Eval(const char *palabra,const double &d)
  {
    matrizExpr retval(*this);
    retval.eval(palabra,d);
    return retval;
  }
bool matrizExpr::Evaluable(void) const
  {
    for(size_type i=1;i<=fls;i++)
      for(size_type j=1;j<=cls;j++)
        if(!(*this)(i,j).Evaluable())
          return false;
    return true;
  }
m_double matrizExpr::ToNum(void) const
  {
    m_double retval(fls,cls,0.0);
    if(!Evaluable())
      std::cerr << "matrizExpr::ToNum: no se pudo evaluar la matriz de expresiones" << std::endl;
    else
      for(size_type i=1;i<=fls;i++)
        for(size_type j=1;j<=cls;j++)
          retval(i,j)= (*this)(i,j).ToNum();
    return retval;  
  }
m_double matrizExpr::ToNum(const char *palabra,const double &d) const
  {
    m_double retval(fls,cls,0.0);
    if(!Evaluable())
      std::cerr << "matrizExpr::ToNum: no se pudo evaluar la matriz de expresiones" << std::endl;
    else
      for(size_type i=1;i<=fls;i++)
        for(size_type j=1;j<=cls;j++)
          retval(i,j)= (*this)(i,j).ToNum(palabra,d);
    return retval;  
  }
matrizExpr &matrizExpr::Trn(void)
  {
    expr_matriz::Trn();
    return *this;
  }
matrizExpr matrizExpr::GetTrn(void) const
  {
    matrizExpr m= *this;
    m.Trn();
    return m;
  }
matrizExpr &matrizExpr::operator*=(const double &d)
  {
    size_type i,sz= size();      
    for(i= 0;i<sz;i++)
      (*this)[i]*= ExprAlgebra(d);
    return *this;
  }
matrizExpr &matrizExpr::operator*=(const matrizExpr &m)
  {
    *this= (*this)*m;
    return *this;
  }
matrizExpr operator*(const matrizExpr &m1,const matrizExpr &m2)
  {
    matrizExpr producto(m1.fls,m2.cls);
    if (m1.cls != m2.fls)
      {
        std::cerr << "Matrices de dimensiones incompatibles en producto." << std::endl;
        std::cerr << "  m1= " << m1 << std::endl;
        std::cerr << "  m2= " << m2 << std::endl;
        return producto;
      }
    matrizExpr::size_type i=1,j=1;
    for (i= 1;i<=m1.fls;i++)
      for (j= 1;j<=m2.cls;j++)
        producto(i,j)= dot(m1.GetFila(i),m2.GetCol(j));
    return producto;
  }




//! @brief Convierte un string (por ejemplo: [[1.0+x^2,0.0][0.0,1.0-x]]) a una matriz.
matrizExpr string_to_matrizExpr(const std::string &str)
  {
    matrizExpr retval(1,1);
    retval.Input(str);
    return retval;
  }

