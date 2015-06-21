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
//MatridZ.h
//Matrices tridiagonales.
//Se almacena la matriz por filas el primer elemento del vector es el que se devuelve
//para indices fuera de las tres diagonales.
#ifndef MATRIDZ_H
#define MATRIDZ_H

#include "matrizZ.h"

template <class numero>
class matridZ : public matrizZ<numero>
  {
  public:
    typedef matrizZ<numero> matrizZ_numero;  
    //typedef typename matrizZ<numero>::matrizZ_allocator matridZ_allocator;
    typedef typename matrizZ_numero::reference reference;
    typedef typename matrizZ_numero::const_reference const_reference;
    typedef typename matrizZ_numero::size_type size_type;
  protected:
    inline virtual size_type Tam(size_type filas,size_type cols) const
      { return (3*filas-1); }
    inline virtual int se_guarda(size_type fila,size_type col) const
      { return (labs(fila-col) <= 1); }
    inline virtual size_type Indice( const size_type &fila,
                                     const size_type &col) const
      {
        if(!se_guarda(fila,col) || (fila == 0) || (col == 0))
          return 0;
        else
          return ((fila-1)*3+col-fila+1);
      }
  public:
    matridZ(void) : matrizZ_numero (1,1)
      { vectorZ_numero::operator[](0)= 0.0; }
    matridZ(const size_type &dim) : matrizZ_numero(1,Tam(dim,dim))
      { PutDim(dim,dim); vectorZ_numero::operator[](0)= 0.0; }
    matridZ(const size_type &dim,const numero &val) : matrizZ_numero(1,Tam(dim,dim))
      { PutDim(dim,dim); vectorZ_numero::operator[](0)= 0.0; }
    matridZ(const matridZ<numero> &otra) : matrizZ_numero(otra)
      { PutDim(otra.fls,otra.cls); vectorZ_numero::operator[](0)= 0.0; }
    matridZ<numero>& operator =(const matridZ<numero> &m)
      {
        matrizZ_numero::operator =(m);
        return *this;
      }
    matridZ<numero>& operator +=(const matridZ<numero> &m)
      {
        matrizZ_numero::Suma(m);
        return *this;
      }
    matridZ<numero>& operator -=(const matridZ<numero> &m)
      {
        matrizZ_numero::Resta(m);
        return *this;
      }
    inline void Trn(void) {}
    inline bool Cuadrada(void)
      { return 1; }
    friend matridZ<numero> operator *(const matridZ<numero> &m,const numero &p)
      {
        matridZ<numero> producto(m);
        producto.Prod(p);
        return producto;
      }
    friend matridZ<numero> operator *(const numero &p,const matridZ<numero> &m)
      { return m*p; }
    friend matridZ<numero> operator +(const matridZ<numero> &m1,const matridZ<numero> &m2)
      {
        if (!CompDim(m1,m2))
          matrizZError("Matrices de dimensiones distintas en operador +");
        matridZ<numero> suma(m1.fls);
        suma.Suma(m1,m2);
        return suma;
      }
    friend matridZ<numero> operator -(const matridZ<numero> &m1,const matridZ<numero> &m2)
      {
        if (!CompDim(m1,m2)) 
          matrizZError("Matrices de dimensiones distintas en operador -");
        matridZ<numero> resta(m1.fls,m1.cls);
        resta.Resta(m1,m2);
        return resta;
      }
    friend std::ostream &operator << (std::ostream &stream,const matridZ<numero> &m)
      { return ::operator <<(stream,(const matrizZ_numero &) m); }
    friend std::istream &operator >> (std::istream &stream,matridZ<numero> &m)
      { return ::operator >>(stream,(matrizZ_numero &) m); }
    friend bool operator ==(const matridZ<numero> &m1,const matridZ<numero> &m2)
      { return ((const matrizZ_numero &) m1 == (const matrizZ_numero &) m2); }
  };

#endif



