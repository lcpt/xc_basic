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
//matrizM.h
//Matriz de matrices (o por cajas)
#ifndef MATRIZM_H
#define MATRIZM_H

#include "m_double.h"
#include "mat_por_cajas.h"

typedef MatPorCajas<m_double> m_matriz;

class vcolM;

class matrizM: public m_matriz
  {
  public:
    //! Constructor.
    matrizM(void) : m_matriz(1,1) {}
    //! Constructor.
    matrizM(size_type n_rows,size_type cols) : m_matriz(n_rows,cols) {}
    //! Constructor.
    matrizM(size_type n_rows,size_type cols,const m_double &val)
      : m_matriz(n_rows,cols,val) {}
    matrizM(const std::vector<size_t> &dim);
    matrizM(const std::vector<size_t> &dim_rows,const std::vector<size_t> &dim_cols);
    //! Constructor de copia.
    matrizM(const matrizM &otra) : m_matriz(otra) {}
    matrizM& operator=(const matrizM &m)
      {
	m_matriz::operator=(m);
        return *this;
      }
    matrizM GetTrn(void) const;
    matrizM &operator*=(const m_double &m);
    matrizM &operator*=(const double &d);
    matrizM &operator*=(const matrizM &m);
    matrizM &operator*=(const vcolM &M);
    friend matrizM operator+(const matrizM &m1,const matrizM &m2)
      {
        matrizM retval(m1);
        retval+=m2;
        return retval;
      }
    friend matrizM operator-(const matrizM &m1,const matrizM &m2)
      {
        matrizM retval(m1);
        retval-=m2;
        return retval;
      }
    friend matrizM operator*(const matrizM &m,const double &d)
      {
        matrizM retval(m);
        retval*=d;
        return retval;
      }
    friend matrizM operator*(const double &d,const matrizM &m)
      { return m*d; }
    friend matrizM operator*(const matrizM &m1,const matrizM &m2);
    friend matrizM operator*(const m_double &m,const matrizM &mM)
      {
        matrizM retval(mM); retval*=m;
        return retval;
      }
    friend matrizM operator*(const matrizM &mM, const m_double &m)
      { return m*mM; }
    friend matrizM a_cajas(const matrizM &muestra,const m_double &a);
    friend matrizM a_cajas(const std::vector<size_t> dim_n_rows,const std::vector<size_t> dim_n_columns,const m_double &a);
  };

#endif
