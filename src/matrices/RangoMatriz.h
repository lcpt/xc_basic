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
//RangoMatriz.h

#ifndef RANGOMATRIZ_H
#define RANGOMATRIZ_H

#include "RangoIndice.h"

//! @brief Rango de variación de un índice, se emplea en ConstRefCaja.
class RangoMatriz
  {
    RangoIndice row_range; //!< row range.
    RangoIndice column_range; //!< column range.
  public:
    RangoMatriz(const RangoIndice &rf,const RangoIndice &rc)
      :row_range(rf),column_range(rc) {}
    template <class M>
    RangoMatriz(const M &);
    size_t getNumberOfRows(void) const
      { return row_range.Size(); }
    size_t getNumberOfColumns(void) const
      { return column_range.Size(); }
    size_t Size(void) const
      { return getNumberOfRows()*getNumberOfColumns(); }
    const RangoIndice &getRowRange(void) const
      { return row_range; }
    RangoIndice &getRowRange(void)
      { return row_range; }
    const RangoIndice &GetgetColumnRange(void) const
      { return column_range; }
    RangoIndice &GetgetColumnRange(void)
      { return column_range; }
    bool isRow(void) const
      { return (row_range.Size()==1); }
    bool isColumn(void) const
      { return (column_range.Size()==1); }
    bool Vacio(void) const;
    inline static const char &Separador(void)
      { return RangoIndice::Separador(); }
    void Recorta(const size_t &fmax,const size_t &cmax);
    void Intersec(const RangoMatriz &otro);
    RangoMatriz Intersec(const RangoMatriz &otro) const;
    void Print(std::ostream &os) const;
  };

RangoMatriz recorta(const RangoMatriz &r,const size_t &fmax,const size_t &cmax);
RangoMatriz intersec(const RangoMatriz &,const RangoMatriz &);
template <class M>
RangoMatriz recorta(const RangoMatriz &,const M &);
std::ostream &operator<<(std::ostream &os,const RangoMatriz &rango);

//! @brief Constructor a partir de una matriz.
template <class M>
RangoMatriz::RangoMatriz(const M &m)
  : row_range(1,m.getNumberOfRows()),column_range(1,m.getNumberOfColumns()) {}

//! @brief Recorte de los intervalos del rango a partir de una matriz.
template <class M>
RangoMatriz recorta(const RangoMatriz &rm,const M &m)
  { return rm.Intersec(RangoMatriz(m)); }
#endif
