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
//matrizMExpr.h
//Matriz de matrices (o por cajas)
#ifndef MATRIZMEXPR_H
#define MATRIZMEXPR_H

#include "matrizExpr.h"
#include "mat_por_cajas.h"

class matrizM;
class ExprAlgebra;

typedef MatPorCajas<matrizExpr> m_matriz_expr;

class matrizMExpr: public m_matriz_expr
  {
    void eval(void);
    void eval(const char *palabra,const ExprAlgebra &a);
    void eval(const char *palabra,const double &d);
  public:
    matrizMExpr(void) : m_matriz_expr(1,1) {}
    matrizMExpr(size_type filas,size_type cols) : m_matriz_expr(filas,cols) {}
    matrizMExpr(size_type filas,size_type cols,const matrizExpr &val)
      : m_matriz_expr(filas,cols,val) {}
    matrizMExpr(const matrizMExpr &otra) : m_matriz_expr(otra) {}
    matrizMExpr& operator=(const matrizMExpr &m)
      {
	m_matriz_expr::operator=(m);
        return *this;
      }
    bool Evaluable(void) const;
    matrizMExpr Eval(void) const;
    matrizMExpr Eval(const char *palabra,const ExprAlgebra &a);
    matrizMExpr Eval(const char *palabra,const double &d);
    matrizM ToNum(void) const;
    matrizM ToNum(const char *palabra,const double &d) const;
    matrizMExpr &operator*=(const matrizExpr &m);
    matrizMExpr &operator*=(const double &d);
    matrizMExpr &operator*=(const matrizMExpr &m);

    friend matrizMExpr operator+(const matrizMExpr &m1,const matrizMExpr &m2);
    friend matrizMExpr operator-(const matrizMExpr &m1,const matrizMExpr &m2);
    friend matrizMExpr operator*(const matrizMExpr &m,const double &d)
;
    friend matrizMExpr operator*(const double &d,const matrizMExpr &m)
      { return m*d; }
    friend matrizMExpr operator*(const matrizMExpr &m1,const matrizMExpr &m2);
    friend matrizMExpr operator*(const matrizExpr &m,const matrizMExpr &mM);
    friend matrizMExpr operator*(const matrizMExpr &mM, const matrizExpr &m)
      { return m*mM; }
  };


#endif
