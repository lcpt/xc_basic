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
//mat_por_cajas.h

#ifndef MAT_POR_CAJAS_H
#define MAT_POR_CAJAS_H

#include "matrizZ.h"

//! @brief Matriz por cajas.
//! E es la clase de las cajas.
template <class TCAJA>
class MatPorCajas: public matrizZ<TCAJA>
  {
  protected:
    size_t get_filas_fila(const size_t &f) const;
    size_t get_cols_col(const size_t f) const;
    std::vector<size_t> get_dim_filas(void) const;
    std::vector<size_t> get_dim_cols(void) const;
    
  public:
    typedef matrizZ<TCAJA> m_cajas;
    typedef typename m_cajas::size_type size_type;
    typedef typename TCAJA::value_type value_type;
    typedef typename m_cajas::iterator iterator;
    typedef typename m_cajas::const_iterator const_iterator;

    MatPorCajas(void);
    MatPorCajas(size_type filas,size_type cols);
    MatPorCajas(size_type filas,size_type cols,const TCAJA &val);
    MatPorCajas(const std::vector<size_t> &);
    MatPorCajas(const std::vector<size_t> &,const std::vector<size_t> &);
    MatPorCajas(const MatPorCajas &otra);
    MatPorCajas& operator=(const MatPorCajas &m);

    size_t GetTotalFilas(void) const;
    size_t GetTotalCols(void) const;
    void Con(const value_type &d);
    MatPorCajas<TCAJA> &Trn(void);
    MatPorCajas<TCAJA> GetTrn(void) const;
    TCAJA QuitaBloques(void);
    MatPorCajas<TCAJA> &operator*=(const TCAJA &m);
    MatPorCajas<TCAJA> &operator*=(const value_type &d);
    MatPorCajas<TCAJA> &operator*=(const MatPorCajas<TCAJA> &m);
  };

//! @brief  Constructor.
template <class TCAJA>
MatPorCajas<TCAJA>::MatPorCajas(void)
  : m_cajas(1,1) {}

//! @brief  Constructor.
template <class TCAJA>
MatPorCajas<TCAJA>::MatPorCajas(size_type filas,size_type cols)
  : m_cajas(filas,cols) {}

//! @brief  Constructor.
template <class TCAJA>
MatPorCajas<TCAJA>::MatPorCajas(size_type filas,size_type cols,const TCAJA &val)
  : m_cajas(filas,cols,val) {}

//! @brief Constructor.
//!
//! @param dim: Dimensiones de filas y columnas (matriz cuadrada).
template <class TCAJA>
MatPorCajas<TCAJA>::MatPorCajas(const std::vector<size_t> &dim)
  :  m_cajas(dim.size(),dim.size())
  {
    for(register size_t i=1;i<=this->fls;i++)
      for(register size_t j=1;j<=this->cls;j++)
        (*this)(i,j)= m_double(dim[i-1],dim[j-1]);
  }

//! @brief Constructor.
//!
//! @param dim_fls: Dimensiones de las filas.
//! @param dim_cls: Dimensiones de las columnas.
template <class TCAJA>
MatPorCajas<TCAJA>::MatPorCajas(const std::vector<size_t> &dim_filas,const std::vector<size_t> &dim_cols)
  :  m_cajas(dim_filas.size(),dim_cols.size())
  {
    for(register size_t i=1;i<=this->fls;i++)
      for(register size_t j=1;j<=this->cls;j++)
        (*this)(i,j)= m_double(dim_filas[i-1],dim_cols[j-1]);
  }

//! Constructor de copia.
template <class TCAJA>
MatPorCajas<TCAJA>::MatPorCajas(const MatPorCajas &otra)
  : m_cajas(otra) {}

//! @brief Operador asignación.
template <class TCAJA>
MatPorCajas<TCAJA> &MatPorCajas<TCAJA>::operator=(const MatPorCajas<TCAJA> &m)
  {
    m_cajas::operator=(m);
    return *this;
  }


//! @brief Devuelve el número de filas de las matrices de la fila f.
template <class TCAJA>
size_t MatPorCajas<TCAJA>::get_filas_fila(const size_t &f) const
  {
    size_t retval= 0;
    const size_t ncls= this->getNumCols();
    for(register size_t j=1;j<=ncls;j++)
      {
        const size_t nfls_j= (*this)(f,j).getNumFilas();
        if(nfls_j>retval) retval= nfls_j;
      }
    return retval;
  }

//! @brief Devuelve el número de columnas de las matrices de la columna c.
template <class TCAJA>
size_t MatPorCajas<TCAJA>::get_cols_col(const size_t c) const
  {
    size_t retval= 0;
    const size_t nfls= this->getNumFilas();
    for(register size_t i=1;i<=nfls;i++)
      {
        const size_t ncls_i= (*this)(i,c).getNumFilas();
        if(ncls_i>retval) retval= ncls_i;
      }
    return retval;
  }

//! @brief Devuelve el número de filas de cada fila.
template <class TCAJA>
std::vector<size_t> MatPorCajas<TCAJA>::get_dim_filas(void) const
  {
    const size_t nfls= this->getNumFilas();
    std::vector<size_t> retval(nfls,0);
    for(register size_t i=1;i<=nfls;i++)
      retval[i-1]= get_filas_fila(i);
    return retval;
  }
//! @brief Devuelve el número de filas de cada columna.
template <class TCAJA>
std::vector<size_t> MatPorCajas<TCAJA>::get_dim_cols(void) const
  {
    const size_t ncls= this->getNumCols();
    std::vector<size_t> retval(ncls,0);
    for(register size_t j=1;j<=ncls;j++)
      retval[j-1]= get_cols_col(j);
    return retval;
  }
//! @brief Devuelve la suma de las filas de las filas de cajas.
template <class TCAJA>
size_t MatPorCajas<TCAJA>::GetTotalFilas(void) const
  {
    std::vector<size_t> dim_filas= get_dim_filas();
    size_t tot_filas= dim_filas[0];
    for(register size_t i=1;i<this->fls;i++)
      tot_filas+= dim_filas[i];
    return tot_filas;
  }
//! @brief Devuelve la suma de las columnas de las columnas de cajas.
template <class TCAJA>
size_t MatPorCajas<TCAJA>::GetTotalCols(void) const
  {
    std::vector<size_t> dim_cols= get_dim_cols();
    size_t tot_cols= dim_cols[0];
    for(register size_t j=1;j<=this->cls;j++)
      tot_cols+= dim_cols[j];
    return tot_cols;
  }

//! @brief Iguala todos los valores de todas las cajas al que se pasa como parámetro.
template <class TCAJA>
void MatPorCajas<TCAJA>::Con(const value_type &d)
  {
    for(iterator i= this->begin();i!=this->end();i++)
      (*i).Con(d);
  }
//! @brief Traspone la matriz.
template <class TCAJA>
MatPorCajas<TCAJA> &MatPorCajas<TCAJA>::Trn(void)
  {
    m_cajas::Trn();
    for(iterator i= this->begin();i!=this->end();i++)
      (*i).Trn();
    return *this;
  }
//! @brief Devuelve la traspuesta.
template <class TCAJA>
MatPorCajas<TCAJA> MatPorCajas<TCAJA>::GetTrn(void) const
  {
    MatPorCajas<TCAJA> m= *this;
    m.Trn();
    return m;
  }

//! Devuelve la matriz que tiene las mismas cajas que esta bloques.
//! Suponemos que el numero de filas de los bloques de una fila es el mismo
//! y que el numero de columnas de los bloque de una columna es el mismo.
template <class TCAJA>
TCAJA MatPorCajas<TCAJA>::QuitaBloques(void)
  {
    TCAJA retval(GetTotalFilas(),GetTotalCols());
    size_t f= 1,c= 1;
    TCAJA caja;
    for(size_t i= 1;i<=this->getNumFilas();i++)
      {
        for(size_t j= 1;j<=this->getNumCols();j++)
          {
            caja= (*this)(i,j);
            retval.PutCaja(f,c,caja);
            c+= caja.getNumCols();
	      }
        f+= caja.getNumFilas();
        c= 1;
      }
    return retval;
  }
//! @brief Operador *=
template <class TCAJA>
MatPorCajas<TCAJA> &MatPorCajas<TCAJA>::operator*=(const TCAJA &m)
  {
    this->Prod(m);
    return *this;
  }
//! @brief Operador *=
template <class TCAJA>
MatPorCajas<TCAJA> &MatPorCajas<TCAJA>::operator*=(const value_type &d)
  {
    size_type i,sz= this->size();      
    for(i= 0;i<sz;i++)
      (*this)[i]*=d;
    return *this;
  }
//! @brief Operador *=
template <class TCAJA>
MatPorCajas<TCAJA> &MatPorCajas<TCAJA>::operator*=(const MatPorCajas<TCAJA> &m)
  {
    m_cajas::operator*=(m);
    return *this;
  }

#endif
