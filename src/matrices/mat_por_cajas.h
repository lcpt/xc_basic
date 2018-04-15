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
    size_t get_rows_row(const size_t &f) const;
    size_t get_columns_column(const size_t f) const;
    std::vector<size_t> get_dim_rows(void) const;
    std::vector<size_t> get_dim_columns(void) const;
    
  public:
    typedef matrizZ<TCAJA> m_cajas;
    typedef typename m_cajas::size_type size_type;
    typedef typename TCAJA::value_type value_type;
    typedef typename m_cajas::iterator iterator;
    typedef typename m_cajas::const_iterator const_iterator;

    MatPorCajas(void);
    MatPorCajas(size_type n_rows,size_type n_columns);
    MatPorCajas(size_type n_rows,size_type n_columns,const TCAJA &val);
    MatPorCajas(const std::vector<size_t> &);
    MatPorCajas(const std::vector<size_t> &,const std::vector<size_t> &);
    MatPorCajas(const MatPorCajas &otra);
    MatPorCajas& operator=(const MatPorCajas &m);

    size_t getTotalNumberOfRows(void) const;
    size_t getTotalNumberOfColumns(void) const;
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
MatPorCajas<TCAJA>::MatPorCajas(size_type n_rows,size_type n_columns)
  : m_cajas(n_rows,n_columns) {}

//! @brief  Constructor.
template <class TCAJA>
MatPorCajas<TCAJA>::MatPorCajas(size_type n_rows,size_type n_columns,const TCAJA &val)
  : m_cajas(n_rows,n_columns,val) {}

//! @brief Constructor.
//!
//! @param dim: Dimensiones de rows and columns (matriz cuadrada).
template <class TCAJA>
MatPorCajas<TCAJA>::MatPorCajas(const std::vector<size_t> &dim)
  :  m_cajas(dim.size(),dim.size())
  {
    for(register size_t i=1;i<=this->n_rows;i++)
      for(register size_t j=1;j<=this->n_columns;j++)
        (*this)(i,j)= m_double(dim[i-1],dim[j-1]);
  }

//! @brief Constructor.
//!
//! @param dim_rows: row dimension.
//! @param dim_columns: column dimension.
template <class TCAJA>
MatPorCajas<TCAJA>::MatPorCajas(const std::vector<size_t> &dim_rows,const std::vector<size_t> &dim_columns)
  :  m_cajas(dim_rows.size(),dim_columns.size())
  {
    for(register size_t i=1;i<=this->n_rows;i++)
      for(register size_t j=1;j<=this->n_columns;j++)
        (*this)(i,j)= m_double(dim_rows[i-1],dim_columns[j-1]);
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


//! @brief Return el number of rows of the iRow-th row matrices.
template <class TCAJA>
size_t MatPorCajas<TCAJA>::get_rows_row(const size_t &iRow) const
  {
    size_t retval= 0;
    const size_t ncls= this->getNumberOfColumns();
    for(register size_t j=1;j<=ncls;j++)
      {
        const size_t nrows_j= (*this)(iRow,j).getNumberOfRows();
        if(nrows_j>retval) retval= nrows_j;
      }
    return retval;
  }

//! @brief Return el number of columns de las matrices de the column c.
template <class TCAJA>
size_t MatPorCajas<TCAJA>::get_columns_column(const size_t c) const
  {
    size_t retval= 0;
    const size_t nrows= this->getNumberOfRows();
    for(register size_t i=1;i<=nrows;i++)
      {
        const size_t ncls_i= (*this)(i,c).getNumberOfRows();
        if(ncls_i>retval) retval= ncls_i;
      }
    return retval;
  }

//! @brief Return el number of rows of each row.
template <class TCAJA>
std::vector<size_t> MatPorCajas<TCAJA>::get_dim_rows(void) const
  {
    const size_t nrows= this->getNumberOfRows();
    std::vector<size_t> retval(nrows,0);
    for(register size_t i=1;i<=nrows;i++)
      retval[i-1]= get_rows_row(i);
    return retval;
  }
//! @brief Return el number of rows of each column.
template <class TCAJA>
std::vector<size_t> MatPorCajas<TCAJA>::get_dim_columns(void) const
  {
    const size_t ncls= this->getNumberOfColumns();
    std::vector<size_t> retval(ncls,0);
    for(register size_t j=1;j<=ncls;j++)
      retval[j-1]= get_columns_column(j);
    return retval;
  }
//! @brief Return the sum of the rows of boxes.
template <class TCAJA>
size_t MatPorCajas<TCAJA>::getTotalNumberOfRows(void) const
  {
    std::vector<size_t> dim_rows= get_dim_rows();
    size_t tot_rows= dim_rows[0];
    for(register size_t i=1;i<this->n_rows;i++)
      tot_rows+= dim_rows[i];
    return tot_rows;
  }
//! @brief Return the sum of the columns of boxes.
template <class TCAJA>
size_t MatPorCajas<TCAJA>::getTotalNumberOfColumns(void) const
  {
    std::vector<size_t> dim_columns= get_dim_columns();
    size_t tot_n_columns= dim_columns[0];
    for(register size_t j=1;j<=this->n_columns;j++)
      tot_n_columns+= dim_columns[j];
    return tot_n_columns;
  }

//! @brief Iguala todos los valores de todas las cajas to the argument.
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
//! @brief Return la traspuesta.
template <class TCAJA>
MatPorCajas<TCAJA> MatPorCajas<TCAJA>::GetTrn(void) const
  {
    MatPorCajas<TCAJA> m= *this;
    m.Trn();
    return m;
  }

//! @brief Return the matrix that has the same number of boxes that this has
//! blocks. We suppose that the number of rows of the blocks of a row is the
//! same and that the number of columns of the blocks are also the same.
template <class TCAJA>
TCAJA MatPorCajas<TCAJA>::QuitaBloques(void)
  {
    TCAJA retval(getTotalNumberOfRows(),getTotalNumberOfColumns());
    size_t f= 1,c= 1;
    TCAJA caja;
    for(size_t i= 1;i<=this->getNumberOfRows();i++)
      {
        for(size_t j= 1;j<=this->getNumberOfColumns();j++)
          {
            caja= (*this)(i,j);
            retval.PutCaja(f,c,caja);
            c+= caja.getNumberOfColumns();
	      }
        f+= caja.getNumberOfRows();
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
