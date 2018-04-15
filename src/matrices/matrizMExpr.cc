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
//MatrizM.cc

#include "matrizMExpr.h"
#include "matrizM.h"

void matrizMExpr::eval(void)
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        (*this)(i,j).eval();
  }
void matrizMExpr::eval(const char *palabra,const ExprAlgebra &a)
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        (*this)(i,j).eval(palabra,a);
  }
void matrizMExpr::eval(const char *palabra,const double &d)
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        (*this)(i,j).eval(palabra,d);
  }

matrizMExpr matrizMExpr::Eval(void) const
  {
    matrizMExpr retval(*this);
    retval.eval();
    return retval;
  }
matrizMExpr matrizMExpr::Eval(const char *palabra,const ExprAlgebra &a)
  {
    matrizMExpr retval(*this);
    retval.eval(palabra,a);
    return retval;
  }
matrizMExpr matrizMExpr::Eval(const char *palabra,const double &d)
  {
    matrizMExpr retval(*this);
    retval.eval(palabra,d);
    return retval;
  }
bool matrizMExpr::Evaluable(void) const
  {
    for(size_type i=1;i<=n_rows;i++)
      for(size_type j=1;j<=n_columns;j++)
        if(!(*this)(i,j).Evaluable())
          return false;
    return true;
  }
matrizM matrizMExpr::ToNum(void) const
  {
    matrizM retval(n_rows,n_columns);
    if(!Evaluable())
      std::cerr << "matrizExpr::ToNum: no se pudo evaluar la matriz de expresiones" << std::endl;
    else
      for(size_type i=1;i<=n_rows;i++)
        for(size_type j=1;j<=n_columns;j++)
          retval(i,j)= (*this)(i,j).ToNum();
    return retval;
  }
matrizM matrizMExpr::ToNum(const char *palabra,const double &d) const
  {
    matrizM retval(n_rows,n_columns);
    if(!Evaluable())
      std::cerr << "matrizExpr::ToNum: no se pudo evaluar la matriz de expresiones" << std::endl;
    else
      for(size_type i=1;i<=n_rows;i++)
        for(size_type j=1;j<=n_columns;j++)
          retval(i,j)= (*this)(i,j).ToNum(palabra,d);
    return retval;
  }
matrizMExpr &matrizMExpr::operator*=(const matrizExpr &m)
  {
    m_matriz_expr::operator*=(m);
    return *this;
  }
matrizMExpr &matrizMExpr::operator*=(const double &d)
  {
    size_type i,sz= size();      
    for(i= 0;i<sz;i++)
      (*this)[i]*=d;
    return *this;
  }
matrizMExpr &matrizMExpr::operator*=(const matrizMExpr &m)
  {
    m_matriz_expr::operator*=(m);
    return *this;
  }
matrizMExpr operator*(const matrizMExpr &m1,const matrizMExpr &m2)
  {
    matrizMExpr producto(m1);
    producto*=(m2);
    return producto;
  }

matrizMExpr operator+(const matrizMExpr &m1,const matrizMExpr &m2)
  {
    matrizMExpr retval(m1);
    retval+=m2;
    return retval;
  }

matrizMExpr operator-(const matrizMExpr &m1,const matrizMExpr &m2)
  {
    matrizMExpr retval(m1);
    retval-=m2;
    return retval;
  }

matrizMExpr operator*(const matrizMExpr &m,const double &d)
  {
    matrizMExpr retval(m);
    retval*=d;
    return retval;
  }

matrizMExpr operator*(const matrizExpr &m,const matrizMExpr &mM)
  {
    matrizMExpr retval(mM); retval*=m;
    return retval;
  }

