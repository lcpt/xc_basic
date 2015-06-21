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
//FForma.h
// ORIGINAL: shape.h Copyright (c) 1999, A.H. van den Boogaard
//FeaTure is put on the internet as free software. I would very 
//much like to hear your feedback and if possible improvements. See 
//also the 'Future plans' part in the manual.
//Modificado por LCPT para integrarlo en Swing.

#ifndef FFORMA_H
#define FFORMA_H

#include <cstddef>
#include "xc_basic/src/matrices/m_double.h"

//!
//! \ingroup Elems
//!
//! \defgroup FuncsForma Funciones de forma.


class PtosIntegracion;

//! @ingroup FuncsForma
//!
//! @brief Clase base de las funciones de forma.
class FForma
  {
  protected:
    //! @brief Constructor por defecto
    FForma(){}

  public:
    //! Destructor.
    virtual ~FForma(){}
    //! @brief Devuelve los valores de las funciones de forma en el punto cuyas coordenadas naturales
    //! se pasan como parámetro.
    virtual m_double get_N_vec(const m_double &natcor) const=0;
    //! @brief Devuelve los valores de derivadas parciales de las funciones de forma en el 
    //! punto cuyas coordenadas naturales se pasan como parámetro.
    virtual m_double getPartialDerivatives(const m_double &natcor) const=0;
    //! @brief Devuelve los puntos de integración para el dominio de las funciones de forma.
    virtual PtosIntegracion get_ptos_integ(void) const=0;
    //! @brief Devuelve el número de puntos de integración para el dominio de las funciones de forma.
    virtual size_t get_nr_ptos_integ() const=0;

    //! @brief Devuelve los puntos de integración para un lado (dominios 2D) 
    //! o una cara (dominios 3D)de las funciones de forma.
    virtual PtosIntegracion get_ptos_integ_face(size_t face_nr) const=0;
    //! ??
    virtual m_double dVdA(size_t face_nr) const=0;
  };

//! @ingroup FuncsForma
//!
//! @brief Clase base para dominios de tipo segmento.
class Segmento: public FForma
  {
  private:
    size_t int_scheme;//!< Número de puntos de integración.

  protected:
    //! @brief Constructor.
    Segmento(size_t intscheme)
      :int_scheme(intscheme){}
  public:
    //! @brief Destructor.
    ~Segmento(){}
    PtosIntegracion get_ptos_integ(void) const;
    //! @brief Devuelve el número de puntos de integración.
    size_t get_nr_ptos_integ(void) const { return int_scheme; }

    m_double dVdA(size_t face_nr) const;
    PtosIntegracion get_ptos_integ_face(size_t face_nr) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Funciones de forma para segmento lineal (2 nodos).
class Seg_lin:public Segmento
  {
  public:
    //! @brief Constructor
    Seg_lin(size_t nxi)
      : Segmento(nxi){}
    //! @brief Destructor
    ~Seg_lin(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Clase base para dominios triangulares.
class Triangular: public FForma
  {
  private:
    size_t int_scheme;//!< Número de puntos de integración.

  protected:
    //! @brief Constructor.
    Triangular(size_t intscheme)
      :int_scheme(intscheme){}
  public:
    //! @brief Destructor.
    ~Triangular(){}
    PtosIntegracion get_ptos_integ(void) const;
    //! @brief Devuelve el número de puntos de integración.
    size_t get_nr_ptos_integ(void) const { return int_scheme; }

    m_double dVdA(size_t face_nr) const;
    PtosIntegracion get_ptos_integ_face(size_t face_nr) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Funciones de forma para triángulo lineal (3 nodos).
class Tri_lin:public Triangular
  {
  public:
    //! @brief Constructor.
    Tri_lin(size_t intscheme)
      : Triangular(intscheme){}
    //! @brief Destructor.
    ~Tri_lin(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Funciones de forma para triángulo cuadrático (6 nodos).
class Tri_quad:public Triangular
  {
  public:
    //! @brief Constructor
    Tri_quad(size_t intscheme)
      : Triangular(intscheme){}
    //! @brief Destructor
    ~Tri_quad(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Funciones de forma para otro triángulo cuadrático (6 nodos).
class Tri_quad_alt:public Triangular
  {
  public:
    //! @brief Constructor
    Tri_quad_alt(size_t intscheme):Triangular(intscheme){}
    //! @brief Destructor
    ~Tri_quad_alt(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Clase base para dominios de cuatro lados.
class Quadrilateral:public FForma
  {
  private:
    size_t int_scheme[2];

  protected:
    //! @brief Constructor
    Quadrilateral(size_t nxi, size_t neta)
      { int_scheme[0]=nxi; int_scheme[1]=neta; }

  public:
    //! @brief Destructor
    ~Quadrilateral(){}
    PtosIntegracion get_ptos_integ() const;
    //! @brief Devuelve el número de puntos de integración.
    size_t get_nr_ptos_integ() const
      { return int_scheme[0]*int_scheme[1]; }

    m_double dVdA(size_t face_nr) const;
    PtosIntegracion get_ptos_integ_face(size_t face_nr) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Funciones de forma para cuadrilátero lineal (4 nodos).
class Quad_lin:public Quadrilateral
  {
  public:
    //! @brief Constructor
    Quad_lin(size_t nxi, size_t neta)
      : Quadrilateral(nxi, neta){}
    //! @brief Destructor
    ~Quad_lin(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Funciones de forma para cuadrilátero cuadrático (8 nodos).
class Quad_quad:public Quadrilateral
  {
  public:
    //! @brief Constructor
    Quad_quad(size_t nxi, size_t neta)
      : Quadrilateral(nxi, neta){}
    //! @brief Destructor
    ~Quad_quad(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Funciones de forma para otro cuadrilátero cuadrático (8 nodos).
class Quad_quad_alt:public Quadrilateral
  {
  public:
    //! @brief Constructor
    Quad_quad_alt(size_t nxi, size_t neta)
      : Quadrilateral(nxi, neta){}
    //! @brief Destructor
    ~Quad_quad_alt(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Clase base de las funciones de forma para dominios hexaédricos.
class Hexaedrico:public FForma
  {
  private:
    size_t int_scheme[3];

  protected:
    //! @brief Constructor
    Hexaedrico(size_t nxi, size_t neta, size_t nzeta)
      { int_scheme[0]=nxi; int_scheme[1]=neta; int_scheme[2]=nzeta; }

  public:
    //! @brief Destructor
    ~Hexaedrico(){}
    PtosIntegracion get_ptos_integ() const;
    //! @brief Devuelve el número de puntos de integración.
    size_t get_nr_ptos_integ() const
      { return int_scheme[0]*int_scheme[1]*int_scheme[2]; }

    m_double dVdA(size_t face_nr) const;
    PtosIntegracion get_ptos_integ_face(size_t face_nr) const;
  };

//! @ingroup FuncsForma
//!
//! @brief Funciones de forma para el hexahedro lineal (8 nodos).
class Hex_lin:public Hexaedrico
  {
  public:
    //! @brief Constructor.
    Hex_lin(size_t nxi, size_t neta, size_t nzeta)
      : Hexaedrico(nxi, neta, nzeta){}
    //! @brief Destructor.
    ~Hex_lin(){}
    m_double get_N_vec(const m_double &natcor) const;
    m_double getPartialDerivatives(const m_double &natcor) const;
  };

#endif // FFORMA_H
