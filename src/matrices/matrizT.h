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
//MatrizT.h 
#ifndef MATRIZT_H
#define MATRIZT_H

#include <iostream>
#include "ProtoMatriz.h"
#include "ConstRefCaja.h"
#include "ConstRefFila.h"
#include "ConstRefCol.h"
#include <list>
#include <algorithm>
#include <sstream>

//T Elementos a almacenar.
//STO Objeto para almacenamiento.
template <class T, class STO>
class MatrizT : public ProtoMatriz, public STO
  {
  protected:
    inline virtual size_t Indice(const size_t &fila,const size_t &col) const
      { return fila*cls-cls+col-1; }
    bool igual_a(const MatrizT<T,STO> &m2) const;
    MatrizT(const MatrizT<T,STO> &orig,size_t f1, size_t c1, size_t f2, size_t c2);
  public:
    typedef std::list<T> lst_T;
    typedef typename STO::iterator iterator;
    typedef typename STO::const_iterator const_iterator;
    typedef typename STO::value_type value_type;
    typedef typename STO::reference reference;
    typedef typename STO::const_reference const_reference;
    typedef ConstRefCaja<MatrizT<T,STO> > const_ref_caja;
    typedef ConstRefFila<MatrizT<T,STO> > const_ref_fila;
    typedef ConstRefCol<MatrizT<T,STO> > const_ref_col;

    MatrizT(size_t filas= 1,size_t cols= 1);
    MatrizT(size_t filas,size_t cols,T val);
    template <class InputIterator>
    MatrizT(const size_t &fls,const size_t &cls,InputIterator b,InputIterator e);
    MatrizT(const MatrizT<T,STO> &otra);
    MatrizT<T,STO>& operator=(const MatrizT<T,STO> &m);
    MatrizT<T,STO>& operator=(const T &n)
      { return Con(n); }

    void resize(size_t filas,size_t cols,T val);
    inline size_t size(void) const
      { return STO::size(); }
    inline const STO &getVector(void) const
      { return static_cast<const STO &>(*this); }

    inline const_reference front() const
      { return STO::front(); }
    inline reference front()
      { return STO::front(); }
    inline const_reference back() const
      { return STO::back(); }
    inline reference back()
      { return STO::back(); }
    inline const_iterator begin() const
      { return STO::begin(); }
    inline iterator begin()
      { return STO::begin(); }
    inline const_iterator end() const
      { return STO::end(); }
    inline iterator end()
      { return STO::end(); }
    MatrizT<T,STO>& Con(const T &t);
    virtual reference operator()(size_t fila=1,size_t col=1)
      { return STO::operator[](Indice(fila,col)); }
    virtual const_reference operator()(size_t fila=1,size_t col=1) const
      { return STO::operator[](Indice(fila,col)); }
    virtual reference at(size_t fila=1,size_t col=1)
      { return STO::at(Indice(fila,col)); }
    virtual const_reference at(size_t fila=1,size_t col=1) const
      { return STO::at(Indice(fila,col)); }
    void Swap(size_t f1,size_t c1,size_t f2, size_t c2)
      { std::swap((*this)(f1,c1),(*this)(f2,c2)); }    
    MatrizT<T,STO> &Trn(void)
      {
        MatrizT<T,STO> temp= GetTrn();
        (*this)= temp;
        return *this;
      }
    MatrizT<T,STO> GetTrn(void) const;
    MatrizT<T,STO> GetCaja(size_t f1, size_t c1, size_t f2, size_t c2) const
      { return MatrizT<T,STO>(*this,f1,c1,f2,c2); }
    const_ref_caja GetConstRefCaja(size_t f1, size_t c1, size_t f2, size_t c2) const
      { return const_ref_caja(*this,f1,c1,f2,c2); }
    const_ref_caja GetConstRefCaja(const RangoIndice &rango_filas,const RangoIndice &rango_cols) const
      { return const_ref_caja(*this,rango_filas,rango_cols); }
    const_ref_caja GetConstRefCaja(size_t f=1, size_t c=1) const
      { return const_ref_caja(*this,f,c); }
    MatrizT<T,STO> GetFila(size_t fila) const
      { return GetCaja(fila,1,fila,cls); }
    const_ref_fila GetConstRefFila(size_t f, size_t c1, size_t c2) const
      { return const_ref_fila(*this,f,c1,c2); }
    const_ref_fila GetConstRefFila(size_t f, const RangoIndice &rango_cols) const
      { return const_ref_fila(*this,f,rango_cols); }
    const_ref_fila GetConstRefFila(size_t f=1, size_t c=1) const
      { return const_ref_fila(*this,f,c); }
    const_ref_col GetConstRefCol(size_t c, size_t f1, size_t f2) const
      { return const_ref_col(*this,c,f1,f2); }
    const_ref_col GetConstRefCol(const RangoIndice &rango_filas,size_t c) const
      { return const_ref_col(*this,rango_filas,c); }
    const_ref_col GetConstRefCol(size_t c=1, size_t f=1) const
      { return const_ref_col(*this,c,f); }
    MatrizT<T,STO> GetCol(size_t col) const
      { return GetCaja(1,col,fls,col); }
    MatrizT<T,STO> GetMenor(size_t f,size_t c) const;
    void PutCaja(size_t f,size_t c,const MatrizT<T,STO> &caja);      
    void PutFila(size_t fila,const MatrizT<T,STO> &f)
      {
        if (!CompCols(*this,f)) return;
        PutCaja(fila,1,f);
      }
    void PutCol(size_t col,const MatrizT<T,STO> &c)
      {
        if (!CompFilas(*this,c)) return;
        PutCaja(1,col,c);
      }
    void OrlaCol(const MatrizT<T,STO> &c)
      {
        MatrizT<T,STO> m(fls,cls+1);
        m.PutCaja(1,1,*this);
        m.PutCol(cls+1,c);
        *this= m;
      }
    void OrlaFila(const MatrizT<T,STO> &f)
      {
        MatrizT<T,STO> m(fls+1,cls);
        m.PutCaja(1,1,*this);
        m.PutFila(fls+1,f);
        *this= m;
      }
    void SwapFilas(size_t f1,size_t f2);
    void SwapCols(size_t c1,size_t c2);
    virtual void Print(std::ostream &) const;
    virtual void Input(std::istream &);
    virtual void Input(const std::string &);
    inline friend bool operator==(const MatrizT<T,STO> &m1,const MatrizT<T,STO> &m2)
      { return m1.igual_a(m2); }
    virtual ~MatrizT(void) {}
  };

//! @brief Constructor por defecto.
template <class T,class STO>
MatrizT<T,STO>::MatrizT(size_t filas,size_t cols)
  : ProtoMatriz(filas,cols), STO(Tam()) {}
//! @brief Constructor.
template <class T,class STO>
MatrizT<T,STO>::MatrizT(size_t filas,size_t cols,T val)
  : ProtoMatriz(filas,cols), STO(Tam(),val) {}
//! @brief Constructor de copia.
template <class T,class STO>
MatrizT<T,STO>::MatrizT(const MatrizT<T,STO> &otra) 
  : ProtoMatriz(otra),STO(otra) {}

//! @brief Constructor con lista inicialización.
template <class T,class STO> template<class InputIterator>
MatrizT<T,STO>::MatrizT(const size_t &fls,const size_t &cls,InputIterator b,InputIterator e)
  : ProtoMatriz(fls,cls), STO(b,e)
  {
    if(STO::size()!=(fls*cls))
      std::cerr << "MatrizT; el número de elementos de la lista es distinto del de la matriz" << std::endl;
  }


//! @brief Operador asignación.
template <class T,class STO>
MatrizT<T,STO>& MatrizT<T,STO>::operator=(const MatrizT<T,STO> &m)
  {
    ProtoMatriz::operator=(m);
    STO::operator=(m);
    return *this;
  }

//! @brief Operador asignación.
template <class T,class STO>
void MatrizT<T,STO>::resize(size_t filas,size_t cols,T val)
  {
    ProtoMatriz::resize(filas,cols);
    STO::resize(Tam());
  }

template <class MATR>
MATR GetMenor(const MATR &matriz,const size_t &f,const size_t &c)
  {
    const size_t fl= matriz.getNumFilas();
    const size_t cl= matriz.getNumCols();
    MATR menor(fl-1,cl-1);
    register size_t m,m1,p,p1;
    for(m= m1= 1; m<=fl;m++)
      {
        if (m == f) continue;
        for(p= p1= 1;p<=cl;p++)
          {
            if (p == c) continue;
            menor(m1,p1)= matriz(m,p);
            p1++;
          }
        m1++;
      }
    return menor;
  }

//! @brief Devuelve el menor de la matriz correspondiente a la fila y columna que se pasan como parámetro.
template <class T,class STO>
MatrizT<T,STO> MatrizT<T,STO>::GetMenor(size_t f,size_t c) const
  { return ::GetMenor(*this,f,c); }

//! @brief Coloca la caja en la posición (f,c) de esta matriz.
template <class T,class STO>
void MatrizT<T,STO>::PutCaja(size_t f,size_t c,const MatrizT<T,STO> &caja)
  {
    check_put_caja(f,c,caja);
    for(register size_t i=1;i<=caja.fls;i++)
      for(register size_t j=1;j<=caja.cls;j++)
        MatrizT<T,STO>::operator()(i+f-1,j+c-1)= caja(i,j);
  }

//! @brief Operador salida.
template <class T,class STO>
std::ostream &operator<<(std::ostream &os,const MatrizT<T,STO> &m)
  {
    m.Print(os);
    return os;
  }

//! @brief Lectura desde istream.
template <class T,class STO>
void MatrizT<T,STO>::Input(std::istream &is)
  {
    std::cerr << "MatrizT<T,STO>::Input no implementada." << std::endl;
    return;
  }

//! @brief Lectura desde string.
template <class T,class STO>
void MatrizT<T,STO>::Input(const std::string &str)
  {
    std::istringstream iss(str);
    Input(iss);
  }

template <class T,class STO>
std::istream &operator >> (std::istream &is,MatrizT<T,STO> &m)
  {
    m.Input(is);
    return is;
  }

template <class M>
inline M traspuesta(const M &m)
  { return m.GetTrn(); }

template<class T,class STO>
MatrizT<T,STO>::MatrizT(const MatrizT<T,STO> &orig,size_t f1, size_t c1, size_t f2, size_t c2): ProtoMatriz(f2-f1+1,c2-c1+1), STO(Tam()) 
  {
    orig.check_get_caja(f1,c1,f2,c2);
    for(register size_t i=1;i<=fls;i++)
      for(register size_t j=1;j<=cls;j++)
        (*this)(i,j)= orig(i+f1-1,j+c1-1);
  }

template<class T,class STO>
MatrizT<T,STO>& MatrizT<T,STO>::Con(const T &t)
  {
    for(register size_t i=1;i<=fls;i++)
      for(register size_t j=1;j<=cls;j++)
        (*this)(i,j)= t;
    return *this;
  }

template<class T,class STO>
MatrizT<T,STO> MatrizT<T,STO>::GetTrn(void) const
  {
    MatrizT<T,STO> retval(cls,fls);
    for(register size_t i=1;i<=fls;i++)
      for(register size_t j=1;j<=cls;j++)
        retval(j,i)= (*this)(i,j); 
    return retval;
  }
template<class T,class STO>
void MatrizT<T,STO>::SwapFilas(size_t f1,size_t f2)
  { for(register size_t c=1;c<=cls;c++) Swap(f1,c,f2,c); }
template<class T,class STO>
void MatrizT<T,STO>::SwapCols(size_t c1,size_t c2)
  { for(register size_t f=1;f<=fls;f++) Swap(f,c1,f,c2); }
template<class T,class STO>
void MatrizT<T,STO>::Print(std::ostream &os) const
  {
    os << '[';
    const size_t fls= getNumFilas(),cls= getNumCols();
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
template<class T,class STO>
bool MatrizT<T,STO>::igual_a(const MatrizT<T,STO> &m2) const
  {
    if (!CompDim(*this,m2)) return false;
    for(register size_t i=1;i<=fls;i++)
      for(register size_t j=1;j<=cls;j++)
        if ((*this)(i,j) != m2(i,j)) return false;
    return true;
  }

//! @brief Devuelve una matriz columna cuyos elementos son los 
//! indicados por los indices de fila del vector de enteros 
//! que se pasa como parámetro.
//! @param matriz: Matriz de la que se extraen los valores.
//! @param indices_fila: Indices de fila de los elementos a extraer.
//! @param icol: Indice de la columna de los elementos a extraer.
template <class MATRV,class MATRI>
MATRV ExtraeValores(const MATRV &matriz,const MATRI &indices_fila,const size_t &icol= 1)
  {
    MATRV retval(indices_fila.getNumFilas(),1);
    for(register size_t ifila=1;ifila<=retval.getNumFilas();ifila++)
      retval(ifila,1)= matriz(indices_fila(ifila),icol);
    return retval;
  }

#endif

