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
//ConstRefCaja.cc

#include "ConstRefCaja.h"

//! @brief Constructor por defecto.
BaseRefCaja::BaseRefCaja(const ProtoMatriz &mat,const size_t &f1,const size_t &c1)
  : ProtoMatriz(std::max(mat.getNumFilas()-f1+1,size_t(0)),std::max(mat.getNumCols()-c1+1,size_t(0))), offset_f(f1-1),offset_c(c1-1)
  {}

//! @brief Constructor.
BaseRefCaja::BaseRefCaja(const ProtoMatriz &mat,const size_t &f1,const size_t &c1,const size_t &f2,const size_t &c2)
  : ProtoMatriz(std::max(f2-f1+1,size_t(0)),std::max(c2-c1+1,size_t(0))), offset_f(f1-1),offset_c(c1-1)
  {}

//! @brief Constructor.
BaseRefCaja::BaseRefCaja(const ProtoMatriz &mat,const RangoIndice &rango_filas,const RangoIndice &rango_cols)
  : ProtoMatriz(rango_filas.Size(),rango_cols.Size()), offset_f(rango_filas.Offset()),offset_c(rango_cols.Offset())
  {}

//! @brief Constructor columna única.
BaseRefCaja::BaseRefCaja(const ProtoMatriz &mat,const RangoIndice &rango_filas,const size_t &col)
  : ProtoMatriz(rango_filas.Size(),1), offset_f(rango_filas.Offset()),offset_c(col-1)
  {}

//! @brief Constructor fila única.
BaseRefCaja::BaseRefCaja(const ProtoMatriz &mat,const size_t &fila,const RangoIndice &rango_cols)
  : ProtoMatriz(1,rango_cols.Size()), offset_f(fila-1),offset_c(rango_cols.Offset())
  {}

//! @brief Devuelve el rango de filas.
RangoIndice BaseRefCaja::RangoFilas(void) const
  { return RangoIndice(offset_f+1,offset_f+getNumFilas()); }

//! @brief Devuelve el rango de columnas.
RangoIndice BaseRefCaja::RangoCols(void) const
  { return RangoIndice(offset_c+1,offset_c+getNumCols()); }

