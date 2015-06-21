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
//ConstRefCol.h 

#ifndef CONSTREFCOL_H
#define CONSTREFCOL_H

#include "ConstRefCaja.h"

//! @brief Referencia a una columna de una matriz.
template <class MAT>
class ConstRefCol: public ConstRefCaja<MAT>
  {
  public:
    typedef typename ConstRefCaja<MAT>::const_reference const_reference;

    ConstRefCol(const MAT &m,const size_t &c= 1,const size_t &f= 1);
    ConstRefCol(const MAT &m,const size_t &,const size_t &,const size_t &);
    ConstRefCol(const MAT &mat,const RangoIndice &,const size_t &);
    virtual const_reference operator()(size_t fila) const
      { return ConstRefCaja<MAT>::operator()(fila,1); }
  };

template<class MAT>
ConstRefCol<MAT>::ConstRefCol(const MAT &mat,const size_t &c,const size_t &f)
  : ConstRefCaja<MAT>(mat,f,c,mat.getNumFilas(),c)
  {}

template<class MAT>
ConstRefCol<MAT>::ConstRefCol(const MAT &mat,const size_t &c,const size_t &f1,const size_t &f2)
  : ConstRefCaja<MAT>(mat,f1,c,f2,c)
  {}

template<class MAT>
ConstRefCol<MAT>::ConstRefCol(const MAT &mat,const RangoIndice &rango_filas,const size_t &col)
  : ConstRefCaja<MAT>(mat,rango_filas,col)
  {}

#endif
