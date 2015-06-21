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
//MatrizIndices.h
//Matriz de puntos

#ifndef MATRIZINDICES_H
#define MATRIZINDICES_H

#include "xc_basic/src/matrices/matrizT.h"
#include "xc_basic/src/matrices/vectorZ.h"
#include "m_int.h"

typedef vectorZ<int> VIndices;

//! @ingroup GEOM
//
//! @brief Matriz de índices. Sirve para expresar una
//! serie de posiciones por sus índices, por ejemplo:
//! [[1,2][2,2][3,2]
//!  [1,1][2,1][2,3]]
class MatrizIndices: public MatrizT<VIndices,std::vector<VIndices > >
  {
  public:
    typedef MatrizT<VIndices,std::vector<VIndices > > m_indices;
  protected:
    MatrizIndices(const MatrizIndices &mp, size_t &f1, size_t &c1, size_t &f2, size_t &c2);
  public:
    MatrizIndices(const size_t &f=1,const size_t &c=1,const VIndices &p= VIndices());
    MatrizIndices(const m_int &,const std::vector<VIndices > &);

    MatrizIndices GetCaja(size_t f1, size_t c1, size_t f2, size_t c2) const;
    MatrizIndices GetFila(size_t fila) const;
    MatrizIndices GetCol(size_t col) const;

    void Offset(const VIndices &);
  };


#endif
