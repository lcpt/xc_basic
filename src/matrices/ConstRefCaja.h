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
//ConstRefCaja.h 

#ifndef CONSTREFCAJA_H
#define CONSTREFCAJA_H

#include "ProtoMatriz.h"
#include "RangoIndice.h"

//! @brief Base para las referencias a una caja de una matriz.
class BaseRefCaja: public ProtoMatriz
  {
  protected:
    size_t offset_f; //!< Offset fila.
    size_t offset_c; //!< Offset columna.
  public:
    BaseRefCaja(const ProtoMatriz &m,const size_t &f1= 1,const size_t &c1= 1);
    BaseRefCaja(const ProtoMatriz &m,const size_t &,const size_t &,const size_t &,const size_t &);
    BaseRefCaja(const ProtoMatriz &mat,const RangoIndice &rango_filas,const RangoIndice &rango_cols);
    BaseRefCaja(const ProtoMatriz &mat,const RangoIndice &,const size_t &);
    BaseRefCaja(const ProtoMatriz &mat,const size_t &,const RangoIndice &);
    RangoIndice RangoFilas(void) const;
    RangoIndice RangoCols(void) const;
  };

//! @brief Referencia a una caja de una matriz.
template <class MAT>
class ConstRefCaja: public BaseRefCaja
  {
    const MAT &m;
  public:
    typedef typename MAT::const_reference const_reference;

    ConstRefCaja(const MAT &m,const size_t &f1= 1,const size_t &c1= 1);
    ConstRefCaja(const MAT &m,const size_t &,const size_t &,const size_t &,const size_t &);
    ConstRefCaja(const MAT &mat,const RangoIndice &rango_filas,const RangoIndice &rango_cols);
    ConstRefCaja(const MAT &mat,const RangoIndice &,const size_t &);
    ConstRefCaja(const MAT &mat,const size_t &,const RangoIndice &);
    virtual const_reference operator()(size_t fila=1,size_t col=1) const
      { return m(fila+offset_f,col+offset_c); }
    void Print(std::ostream &) const;
  };

//! @brief Constructor por defecto.
template<class MAT>
ConstRefCaja<MAT>::ConstRefCaja(const MAT &mat,const size_t &f1,const size_t &c1)
  : BaseRefCaja(mat,f1,c1), m(mat) {}

//! @brief Constructor.
template<class MAT>
ConstRefCaja<MAT>::ConstRefCaja(const MAT &mat,const size_t &f1,const size_t &c1,const size_t &f2,const size_t &c2)
  : BaseRefCaja(mat,f1,c1,f2,c2), m(mat)
  {}

//! @brief Constructor.
template<class MAT>
ConstRefCaja<MAT>::ConstRefCaja(const MAT &mat,const RangoIndice &rango_filas,const RangoIndice &rango_cols)
  : BaseRefCaja(mat,rango_filas,rango_cols), m(mat) {}

//! @brief Constructor columna única.
template<class MAT>
ConstRefCaja<MAT>::ConstRefCaja(const MAT &mat,const RangoIndice &rango_filas,const size_t &col)
  : BaseRefCaja(mat,rango_filas,col), m(mat) {}

//! @brief Constructor fila única.
template<class MAT>
ConstRefCaja<MAT>::ConstRefCaja(const MAT &mat,const size_t &fila,const RangoIndice &rango_cols)
  : BaseRefCaja(mat,fila,rango_cols), m(mat) {}

template<class MAT>
void ConstRefCaja<MAT>::Print(std::ostream &os) const
  {
    os << '[';
    size_t fls= getNumFilas(),cls= getNumCols();
    for(register size_t i= 1;i<=fls;i++)
      {
        os << '[';
        if(cls > 0) os << (*this)(i,1);
	for(register size_t j= 2;j<=cls;j++)
	  os << ',' << (*this)(i,j);
        os << ']';
      }
    os << ']';
  }

template <class MAT>
inline std::ostream &operator<<(std::ostream &os,const ConstRefCaja<MAT> &c)
  {
    c.Print(os);
    return os;
  }

#endif
