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
//ConstRefFila.h 

#ifndef CONSTREFFILA_H
#define CONSTREFFILA_H

#include "ConstRefCaja.h"

//! @brief Referencia a una fila de una matriz.
template <class MAT>
class ConstRefFila: public ConstRefCaja<MAT>
  {
  public:
    typedef typename ConstRefCaja<MAT>::const_reference const_reference;

    ConstRefFila(const MAT &m,const size_t &f= 1,const size_t &c= 1);
    ConstRefFila(const MAT &m,const size_t &,const size_t &,const size_t &);
    ConstRefFila(const MAT &mat,const size_t &,const RangoIndice &);
    virtual const_reference operator()(size_t col) const
      { return ConstRefCaja<MAT>::operator()(1,col); }
  };

template<class MAT>
ConstRefFila<MAT>::ConstRefFila(const MAT &mat,const size_t &f,const size_t &c)
  : ConstRefCaja<MAT>(mat,f,c,f,mat.getNumCols())
  {}

template<class MAT>
ConstRefFila<MAT>::ConstRefFila(const MAT &mat,const size_t &f,const size_t &c1,const size_t &c2)
  : ConstRefCaja<MAT>(mat,f,RangoIndice(c1,c2))
  {}

template<class MAT>
ConstRefFila<MAT>::ConstRefFila(const MAT &mat,const size_t &f,const RangoIndice &rango_cols)
  : ConstRefCaja<MAT>(mat,f,rango_cols)
  {}

#endif
