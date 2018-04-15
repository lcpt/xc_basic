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
//m_func.h
//Matrices de funciones reales de n variables, esto es
//una función de Rn en Rn.

#ifndef M_FUNC_H
#define M_FUNC_H

#include "matrizZ.h"
#include "m_double.h"

template<class F>
class MatrizF: public matrizZ<F>
//Matriz de cosas que operan (::operator())
//sobre una matriz de doubles y
//returnn un double.
  {
  public:
    typedef matrizZ<F> matriz_f;
    typedef typename matriz_f::size_type size_type;

    MatrizF(void) : matriz_f(1,1) {}
    MatrizF(size_type n_rows,size_type cols) : matriz_f(n_rows,cols) {}
    MatrizF(const MatrizF<F> &otra) : matriz_f(otra) {}
    m_double Eval(const m_double &d) const;
  };

template <class F>
m_double MatrizF<F>::Eval(const m_double &d) const
  {
    size_t sz= this->size();
    m_double retval(sz,1);
    for(size_t i= 1; i<=sz; i++)
      retval(i)= ((*this)(i))(d);
    return retval;
  }

typedef double (*tipo_m_func)(const m_double &);
typedef MatrizF<tipo_m_func> m_func;

#endif
