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
//vcolM.h

#ifndef VCOLM_H
#define VCOLM_H

#include "matrizM.h"


//! column vector de matrices.
class vcolM: public matrizM
  {
  private:
    inline m_double &operator()(const size_t &iRow,const size_t &)
      { return matrizM::operator()(iRow,1); }
    inline const m_double &operator()(const size_t &iRow,const size_t &) const
      { return matrizM::operator()(iRow,1); }
  public:
    //! Constructor por defecto.
    vcolM(const size_type &n_rows= 1)
      : matrizM(n_rows,1) {}
    //! Constructor.
    vcolM(const size_type &n_rows,const m_double &val)
      : matrizM(n_rows,1,val) {}
    vcolM(const std::vector<size_t> &dim);
    vcolM(const matrizM &m,const size_type &c);
    //! Constructor de copia.
    vcolM(const vcolM &otra) 
      : matrizM(otra) {}
    //! Operador asignacion.
    vcolM &operator=(const vcolM &m)
      {
	matrizM::operator=(m);
        return *this;
      }
    m_double &operator()(const size_t &iRow=1)
      { return matrizM::operator()(iRow,1); }
    const m_double &operator()(const size_t &iRow=1) const
      { return matrizM::operator()(iRow,1); }

    vcolM &operator*=(const m_double &m);
    vcolM &operator*=(const double &d);
    friend vcolM operator+(const vcolM &m1,const vcolM &m2);
    friend vcolM operator-(const vcolM &m1,const vcolM &m2);
    friend vcolM operator*(const vcolM &m,const double &d);
    friend vcolM operator*(const double &d,const vcolM &m)
      { return m*d; }
    friend vcolM operator*(const m_double &m,const vcolM &mM);
    friend vcolM operator*(const vcolM &mM, const m_double &m)
      { return m*mM; }
  };
#endif

vcolM operator*(const matrizM &a,const vcolM &b);
