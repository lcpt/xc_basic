//----------------------------------------------------------------------------
//  biblioteca bibXCLcmd; bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//MatrizIndices.cc

#include "MatrizIndices.h"

MatrizIndices::MatrizIndices(const MatrizIndices &mp, size_t &f1, size_t &c1, size_t &f2, size_t &c2)
 : m_indices(mp,f1,c1,f2,c2) {}

MatrizIndices::MatrizIndices(const size_t &f,const size_t &c,const VIndices &p)
  : m_indices(f,c,p) {}

//! @brief Constructor.
//! @param posiciones: Matriz en la que cada fila i es un vector [j,k] que indica la
//! posición (fila= j, columna= k) del vector que ocupa la fila i en indices, es decir
//! (*this)(j,k)= indices[i].
//! @param indices: Matriz cuyas filas son los vectores de índices que se colocan en
//! esta matriz.
MatrizIndices::MatrizIndices(const m_int &posiciones,const std::vector<VIndices> &indices)
  : m_indices(posiciones.maximo_de_columna(1),posiciones.maximo_de_columna(2)) 
  {
    const size_t sz= indices.size();
    const size_t nf= posiciones.maximo_de_columna(1);
    const size_t nc= posiciones.maximo_de_columna(2);
    if(sz>=nf*nc)
      {
        for(size_t i=1;i<sz;i++)
          {
            const size_t j= posiciones(i,1);
            const size_t k= posiciones(i,2);
            (*this)(j,k)= indices[i-1];
          }
      }
    else
      std::cerr << "MatrizIndices; se paso una matriz con un"
                << " número de filas (" << sz << ") insuficiente"
		<< " (<" << nf*nc << ").\n";
  }
//! @brief Devuelve la fila cuyos índices se pasa como parámetro.
MatrizIndices MatrizIndices::GetCaja(size_t f1, size_t c1, size_t f2, size_t c2) const
  { return MatrizIndices(*this,f1,c1,f2,c2); }

//! @brief Devuelve la columna cuyo índice se pasa como parámetro.
MatrizIndices MatrizIndices::GetFila(size_t fila) const
  { return GetCaja(fila,1,fila,this->cls); }

//! @brief Devuelve la fila cuyo índice se pasa como parámetro.
MatrizIndices MatrizIndices::GetCol(size_t col) const
  { return GetCaja(1,col,this->fls,col); }

//! @brief Agrega a todos los miembros el valor
//! que se pasa como parámetro.
void MatrizIndices::Offset(const VIndices &vi)
  {
    const size_t nFilas= getNumFilas();
    const size_t nCols= getNumCols();
    for(size_t i= 1;i<= nFilas;i++)
      for(size_t j= 1;j<= nCols;j++)
	(*this)(i,j)+= vi;
  }
