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
//RangoMatriz.cc

#include "RangoMatriz.h"

//! @brief Devuelve verdadero si el rango no contiene ningún índice.
bool RangoMatriz::Vacio(void) const
{ return (rango_filas.Vacio() || rango_cols.Vacio()); }

//! @brief Imprime el rango.
void RangoMatriz::Print(std::ostream &os) const
  { os << rango_filas << ',' << rango_cols; }

//! @brief Reduce los valores del rango de manera que ambos sean menores que
//! los que se pasan como parámetro.
void RangoMatriz::Recorta(const size_t &fmax,const size_t &cmax)
  {
    rango_filas.Recorta(fmax);
    rango_cols.Recorta(cmax);
  }

//! @brief Asigna a ESTE la intersección de ambos rangos.
void RangoMatriz::Intersec(const RangoMatriz &otro)
  {
    rango_filas.Intersec(otro.rango_filas);
    rango_cols.Intersec(otro.rango_cols);
  }

//! @brief Devuelve la intersección de ambos rangos.
RangoMatriz RangoMatriz::Intersec(const RangoMatriz &otro) const
  {
    RangoMatriz retval(*this);
    retval.Intersec(otro);
    return retval;
  }

std::ostream &operator<<(std::ostream &os,const RangoMatriz &rango)
  {
    rango.Print(os);
    return os;
  }

RangoMatriz recorta(const RangoMatriz &r,const size_t &fmax,const size_t &cmax)
  {
    RangoMatriz retval(r);
    retval.Recorta(fmax,cmax);
    return retval;
  }

//! @brief Devuelve la intersección de ambos rangos.
RangoMatriz intersec(const RangoMatriz &r1,const RangoMatriz &r2)
  { return r1.Intersec(r2); }
