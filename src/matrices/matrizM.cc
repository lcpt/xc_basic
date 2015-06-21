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
//! @param dim: Dimensiones de filas y columnas (matriz cuadrada).
matrizM::matrizM(const std::vector<size_t> &dim)
  :  m_matriz(dim) {}

//! @brief Constructor.
//!
//! @param dim_fls: Dimensiones de las filas.
//! @param dim_cls: Dimensiones de las columnas.
matrizM::matrizM(const std::vector<size_t> &dim_filas,const std::vector<size_t> &dim_cols)
  :  m_matriz(dim_filas,dim_cols) {}

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

//! @brief Devuelve la traspuesta.
matrizM matrizM::GetTrn(void) const
  {
    matrizM m= *this;
    m.Trn();
    return m;
  }

//! @brief Convierte en una matriz de bloques, la que se pasa como parámetro.
//!
//! @param dim_fls: Dimensiones de las filas.
//! @param dim_cls: Dimensiones de las columnas.
//! @param a: Matriz que se convertirá a bloques.
matrizM a_cajas(const std::vector<size_t> dim_fls,const std::vector<size_t> dim_cls,const m_double &a)
  {
    matrizM retval(dim_fls,dim_cls);
    //Comprobamos dimensiones.
    assert(retval.GetTotalFilas()==a.getNumFilas()); 
    assert(retval.GetTotalCols()==a.getNumCols());

    const size_t fls= retval.getNumFilas();
    const size_t cls= retval.getNumCols();

    size_t fila_caja= 1; //Fila donde empieza la caja.
    size_t col_caja= 1; //Columna donde empieza la caja.
    for(size_t i=1;i<=fls;i++)
      {
        col_caja= 1;
        const size_t fbloque= dim_fls[i-1]; //Filas de la fila i.
        for(size_t j=1;j<=cls;j++)
          {
            const size_t cbloque= dim_cls[j-1]; //Columnas de la columna j.
            retval(i,j)= a.GetCaja(fila_caja,col_caja,fila_caja+fbloque-1,col_caja+cbloque-1);
            col_caja+=cbloque; //Avanza las columnas.
          }
        fila_caja+=fbloque; //Avanza las filas.
      }
    return retval;
  }

//! @brief Convierte en una matriz de bloques, la que se pasa como parámetro.
//!
//! @param muestra: Matriz de muestra cuyos bloques sirven para definir la nueva.
//! @param a: Matriz que se convertirá a bloques.
matrizM a_cajas(const matrizM &muestra,const m_double &a)
  {
    const std::vector<size_t> dim_fls= muestra.get_dim_filas();
    const std::vector<size_t> dim_cls= muestra.get_dim_cols();
    return a_cajas(dim_fls,dim_cls,a);
  }




