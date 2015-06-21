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
//matrizExpr.h
//Matriz de expresiones

#ifndef MATRIZEXPR_H
#define MATRIZEXPR_H

#include "matrizZ.h"
#include "xc_basic/src/funciones/algebra/ExprAlgebra.h"
#include "m_double.h"

typedef matrizZ<ExprAlgebra> expr_matriz;

class matrizExpr: public expr_matriz
  {
    friend class matrizMExpr;

    void eval(void);
    void eval(const char *palabra,const ExprAlgebra &a);
    void eval(const char *palabra,const double &d);
  public:
    matrizExpr(size_type filas=1,size_type cols=1,const ExprAlgebra &val=ExprAlgebra())
      : expr_matriz(filas,cols,val) {}
    matrizExpr(const matrizExpr &otra) : expr_matriz(otra) {}
    matrizExpr &operator=(const matrizExpr &m)
      {
	expr_matriz::operator=(m);
        return *this;
      }
    explicit matrizExpr(const m_double &m);
    static LexAlgebra &LexA(void)
      { return ExprAlgebra::LexA(); }
    matrizExpr &Trn(void);
    matrizExpr GetTrn(void) const;
    bool Evaluable(void) const;
    matrizExpr Eval(void) const;
    matrizExpr Eval(const char *palabra,const ExprAlgebra &a);
    matrizExpr Eval(const char *palabra,const double &d);
    m_double ToNum(void) const;
    m_double ToNum(const char *palabra,const double &d) const;
    matrizExpr &operator*=(const double &d);
    matrizExpr &operator*=(const matrizExpr &m);
    friend matrizExpr operator*(const matrizExpr &m1,const matrizExpr &m2);
    friend matrizExpr operator*(const m_double &m,const matrizExpr &mE)
      {
        matrizExpr retval= mE*matrizExpr(m);
        return retval;
      }
    friend matrizExpr operator*(const matrizExpr &mE, const m_double &m)
      { return m*mE; }
  };


matrizExpr string_to_matrizExpr(const std::string &str);

#endif
