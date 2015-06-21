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
//Parametriza.h

#ifndef PARAMETRIZA_H
#define PARAMETRIZA_H

#include "xc_basic/src/matrices/m_double.h"

template<class F,size_t i>
class Parametriza
//Convierte una funcion de Rn en R
//en una función de R en R dejando fijos
//todos las variables de Rn menos el de
//índice i.
  {
    const F f_;
    const m_double &x;
  public:
    Parametriza(const F &f,const m_double &x0)
      : f_(f), x(x0) {}
    double operator()(const double &d) const
      {
        m_double nuevo(x);
        nuevo(i)= d;
        return f_(nuevo);
      }
  };

template <class F>
inline m_double ArgMatriz(const F &f,const m_double &m)
  {
    m_double retval(m.getNumFilas(),m.getNumCols());
    for(size_t k= 1; k <= m.getNumFilas(); k++)
      for(size_t l= 1; l <= m.getNumCols(); l++)
        retval(k,l)= f(m(k,l));
    return retval;
  }

#endif
