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

#include "matrizM.h"
#include "vcolM.h"

//! @brief Constructor.
//!
//! @param dim: Dimensiones de rows and columns (matriz cuadrada).
matrizM::matrizM(const std::vector<size_t> &dim)
  :  m_matriz(dim) {}

//! @brief Constructor.
//!
//! @param dim_rows: row dimension.
//! @param dim_columns: column dimension.
matrizM::matrizM(const std::vector<size_t> &dim_rows,const std::vector<size_t> &dim_columns)
  :  m_matriz(dim_rows,dim_columns) {}

//! @brief Operador *=
matrizM &matrizM::operator*=(const m_double &m)
  {
    m_matriz::operator*=(m);
    return *this;
  }

//! @brief Operador *=
matrizM &matrizM::operator*=(const double &d)
  {
    m_matriz::operator*=(d);
    return *this;
  }
//! @brief Operador *=
matrizM &matrizM::operator*=(const matrizM &m)
  {
    m_matriz::operator*=(m);
    return *this;
  }

//! @brief Operador *=
matrizM &matrizM::operator*=(const vcolM &v)
  {
    m_matriz::operator*=((const matrizM &)v);
    return *this;
  }

matrizM operator*(const matrizM &m1,const matrizM &m2)
  {
    matrizM producto(m1);
    producto*=m2;
    return producto;
  }

//! @brief Return the transposed matrix.
matrizM matrizM::GetTrn(void) const
  {
    matrizM m= *this;
    m.Trn();
    return m;
  }

//! @brief Convierte en una matriz de bloques, la que is being passed as parameter.
//!
//! @param dim_rows: row dimension.
//! @param dim_columns: column dimension.
//! @param a: Matriz que se convertirá a bloques.
matrizM a_cajas(const std::vector<size_t> dim_rows,const std::vector<size_t> dim_columns,const m_double &a)
  {
    matrizM retval(dim_rows,dim_columns);
    //Comprobamos dimensiones.
    assert(retval.getTotalNumberOfRows()==a.getNumberOfRows()); 
    assert(retval.getTotalNumberOfColumns()==a.getNumberOfColumns());

    const size_t n_rows= retval.getNumberOfRows();
    const size_t n_columns= retval.getNumberOfColumns();

    size_t row_box= 1; //Row where the box starts.
    size_t col_caja= 1; //Column where the box starts.
    for(size_t i=1;i<=n_rows;i++)
      {
        col_caja= 1;
        const size_t fbloque= dim_rows[i-1]; //Rows of i-th row.
        for(size_t j=1;j<=n_columns;j++)
          {
            const size_t cbloque= dim_columns[j-1]; //Columns of j-th column.
            retval(i,j)= a.GetCaja(row_box,col_caja,row_box+fbloque-1,col_caja+cbloque-1);
            col_caja+=cbloque; //Next column.
          }
        row_box+=fbloque; //Next row.
      }
    return retval;
  }

//! @brief Convierte en una matriz de bloques, la que is being passed as parameter.
//!
//! @param muestra: Matriz de muestra cuyos bloques sirven para definir la nueva.
//! @param a: Matriz que se convertirá a bloques.
matrizM a_cajas(const matrizM &muestra,const m_double &a)
  {
    const std::vector<size_t> dim_rows= muestra.get_dim_rows();
    const std::vector<size_t> dim_columns= muestra.get_dim_columns();
    return a_cajas(dim_rows,dim_columns,a);
  }




