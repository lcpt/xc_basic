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
//MatrizZ.h 
#ifndef MATRIZZ_H
#define MATRIZZ_H

#include <iostream>
#include "vectorZ.h"
#include "matrizT.h"
#include <cmath>

template <class MATR>
MATR prod_vectorial(const MATR &v1,const MATR &v2);

//! @brief Matriz cuyos elementos tienen estructura de anillo
//! respecto a las operaciones + y *.
template <class numero>
class matrizZ : public MatrizT<numero,vectorZ<numero> >
  {
  public:
    typedef vectorZ<numero> vectorZ_numero;
    typedef MatrizT<numero,vectorZ<numero> > mT_numero;  
    //typedef vectorZ_allocator matrizZ_allocator;
    typedef typename vectorZ_numero::reference reference;
    typedef typename vectorZ_numero::const_reference const_reference;
    typedef typename vectorZ_numero::value_type value_type;    
    typedef typename vectorZ_numero::size_type size_type;
    typedef typename mT_numero::lst_T lst_numero;

  private:
    void TSRGT(numero eps,size_t *it,matrizZ<numero> &C,matrizZ<size_t> &Kp,matrizZ<size_t> &Lp) const;
    numero DMGT(const numero &eps) const;

  protected:
    numero suma_de_fila(size_type i) const;
    numero suma_de_columna(size_type j) const;
  public:
    matrizZ(void) : mT_numero(1,1) {}
    matrizZ(size_type filas,size_type cols) : mT_numero(filas,cols) {}
    matrizZ(size_type filas,size_type cols,numero val) : mT_numero(filas,cols,val) {}
    matrizZ(size_type filas,size_type cols,const lst_numero &ln) : mT_numero(filas,cols)
      { vectorZ_numero::operator=(vectorZ_numero(ln)); }
    matrizZ(const matrizZ<numero> &otra) : mT_numero(otra) {}
    template <class InputIterator>
    matrizZ(const size_t &fls,const size_t &cls,InputIterator b,InputIterator e);
    matrizZ<numero> &operator=(const matrizZ<numero> &m)
      {
	mT_numero::operator=(m);
        return *this;
      }
    matrizZ<numero>& operator=(const numero &n)
      {
        mT_numero::Con(n);
        return *this;
      }
    matrizZ<numero>& operator+=(const matrizZ<numero> &m)
      {
        this->check_sto_sum(m);
        this->Suma(m);
        return *this;
      }
    matrizZ<numero>& operator-=(const matrizZ<numero> &m)
      {
        this->check_sto_dif(m);
        this->Resta(m);
        return *this;
      }
    matrizZ<numero> &operator*=(const numero &n)
      {
        this->Prod(n);
        return *this;
      }
    matrizZ<numero> &operator*=(const matrizZ<numero> &m)
      {
        *this= (*this)*m;
        return *this;
      }
    numero maximo_de_fila(size_type i) const;
    numero maximo_de_columna(size_type j) const;
    void PutSuma(size_type i,size_type j,const numero &n)
    //Le suma n a la posiciOn i,j.
      { vectorZ_numero::PutSuma(Indice(i,j),n); }
    void PutResta(size_type i,size_type j,const numero &n)
    //Le resta n a la posici'on i,j.
      { vectorZ_numero::PutResta(Indice(i,j),n); }    
    void PutProd(size_type i,size_type j,const numero &n)
    //Multiplica la posici'on i por n.
      { vectorZ_numero::PutProd(Indice(i,j),n); }
    void Swap(size_type f1,size_type c1,size_type f2, size_type c2)
      { vectorZ_numero::Swap(Indice(f1,c1),Indice(f2,c2)); }    
    matrizZ<numero> &Trn(void)
      {
	mT_numero::Trn();
        return *this;
      }
    matrizZ<numero> GetTrn(void) const
      {
        matrizZ<numero> m(*this);
        m.Trn();
        return m;
      }
    matrizZ<numero> GetCaja(size_t f1, size_t c1, size_t f2, size_t c2) const;
    matrizZ<numero> GetFila(size_type fila) const
      { return GetCaja(fila,1,fila,this->cls); }
    matrizZ<numero> GetCol(size_type col) const
      { return GetCaja(1,col,this->fls,col); }
    matrizZ<numero> GetMenor(size_t f,size_t c) const;
    void Idn(void);
    void SumaCaja(size_t f,size_t c,const matrizZ<numero> &caja);
    numero GetDetLento(void) const;
    numero GetDet(const numero &eps= 1e-10) const;
    numero Traza(void) const;
    inline bool Nulo(const numero &tol= numero()) const
      { return (this->Abs2()<(tol*tol)); }
    numero NormaPorFilas(void) const;
    numero NormaPorColumnas(void) const;
    inline void Neg(void)
      { return vectorZ<numero>::Neg(); }

    virtual void Input(std::istream &);
    virtual void Input(const std::string &);

    friend bool operator==(const matrizZ<numero> &m1,const matrizZ<numero> &m2)
      { return ((const mT_numero &) m1 == (const mT_numero &) m2); }
/*     friend matrizZ<numero> operator+(const matrizZ<numero> &m1,const matrizZ<numero> &m2) return suma(m1.fls,m1.cls); */
/*       { */
/*         check_sum(m1,m2); */
/*         //matrizZ<numero> suma(m1.fls,m1.cls); */
/*         suma.Suma(m1,m2); */
/*         return; */
/*       } */
    friend matrizZ<numero> operator-(const matrizZ<numero> &m1,const matrizZ<numero> &m2)
      {
        check_dif(m1,m2);
        matrizZ<numero> resta(m1.fls,m1.cls);
        resta.Resta(m1,m2);
        return resta;
      }
    numero dot(const matrizZ<numero> &v2) const;
    //Producto escalar de este por v2.
    //v2: Vector columna.

    numero Abs2(void) const;
    numero Abs(void) const;

    inline friend numero dot(const matrizZ<numero> &v1,const matrizZ<numero> &v2)
    //Producto escalar de dos vectores.
    //v1: Vector fila.
    //v2: Vector columna.
      { return v1.dot(v2); }

/*     friend matrizZ<numero> operator*(const matrizZ<numero> &m1,const matrizZ<numero> &m2) */
/*       { */
/*         check_prod(m1,m2); */
/*         matrizZ<numero> producto(m1.fls,m2.cls); */
/*         for(register size_type i=1;i<=m1.fls;i++) */
/*           for(register size_type j=1;j<=m2.cls;j++) */
/*  	    { */
/*               register numero t=numero(); */
/*               for(register size_type k=1;k<= m1.cls;k++) */
/*                 t+= m1(i,k) * m2(k,j); */
/*               producto(i,j)= t; */
/* 	    } */
/*         return producto; */
/*       } */
    friend matrizZ<numero> operator*(const matrizZ<numero> &m,const numero &p)
      {
        matrizZ<numero> producto(m);
        producto.Prod(p);
        return producto;
      }
    friend matrizZ<numero> operator*(const numero &p,const matrizZ<numero> &m)
      { return m*p; }
    friend matrizZ<numero> operator^(const matrizZ<numero> &v1,const matrizZ<numero> &v2)
    //¡Ojo! está escrito para vectores de dimensión 3 xxx.
      { return prod_vectorial(v1,v2); }
  };

template <class numero> template<class InputIterator>
matrizZ<numero>::matrizZ(const size_t &fls,const size_t &cls,InputIterator b,InputIterator e)
  : mT_numero(fls,cls,b,e) {}

template <class numero>
matrizZ<numero> operator-(const matrizZ<numero> &m)
  {
    matrizZ<numero> retval(m);
    retval.Neg();
    return retval;
  }

template <class numero>
matrizZ<numero> matrizZ<numero>::GetMenor(size_t f,size_t c) const
  { return ::GetMenor(*this,f,c); }
  
template <class numero>  
void matrizZ<numero>::SumaCaja(size_t f,size_t c,const matrizZ<numero> &caja)
  {
    register size_type i,j;
    for(i=1;i<=caja.fls;i++)
      for(j=1;j<=caja.cls;j++)
        PutSuma(i+f-1,j+c-1,caja(i,j));
  }

template <class numero>
void matrizZ<numero>::Idn(void)
  {
    register size_type i;
    if(this->Cuadrada())
      {
	vectorZ_numero::Con(numero());
	for(i= 1;i<=this->fls;i++) (*this)(i,i)= neutro_producto(numero()); //Neutro del producto
      }
  }

//!  @brief Producto vectorial. ¡Ojo! está escrito para vectores columna de dimensión 3 xxx.
template <class MATR>
MATR prod_vectorial(const MATR &v1,const MATR &v2)
  {
    MATR v(v1.getNumFilas(),v1.getNumCols());
    MATR m(v1.getNumFilas(),3);
    m.PutCol(2,v1);
    m.PutCol(3,v2);
    MATR menor(2,2);
    menor= GetMenor(m,1,1);
    v(1,1)= menor.GetDet();
    menor= GetMenor(m,2,1);
    v(2,1)= -menor.GetDet();
    menor= GetMenor(m,3,1);
    v(3,1)= menor.GetDet();
    return v;
  }

//! @brief Devuelve el determinante.
//! 
//! Devuelve el determinante empleando un algoritmo recursivo lentísimo 
//! pero que no necesita divisiones.
template <class numero>
numero matrizZ<numero>::GetDetLento(void) const
  {
    this->check_traza();
    numero p= neutro_suma(numero());
    const numero cero= neutro_suma(p);
    size_type n= this->getNumFilas();
    switch(n)
      {
        case 0:
          break;
        case 1:
          p= (*this)(1,1);
          break;
        case 2:
          p= (*this)(1,1)*(*this)(2,2)-(*this)(1,2)*(*this)(2,1);
          break;
        case 3:
          p= (*this)(1,1)*(*this)(2,2)*(*this)(3,3);
          p+= (*this)(1,2)*(*this)(2,3)*(*this)(3,1);
          p+= (*this)(1,3)*(*this)(2,1)*(*this)(3,2);
          p-= (*this)(1,3)*(*this)(2,2)*(*this)(3,1);
          p-= (*this)(1,2)*(*this)(2,1)*(*this)(3,3);
          p-= (*this)(1,1)*(*this)(2,3)*(*this)(3,2);
          break;
        default:
          {
            for(register size_type i= 1;i<= n;i++)
	      {
                matrizZ<numero> menor= (*this).GetMenor(i,1);
                numero n= (*this)(i,1)*menor.GetDetLento();
                if ((i+1)%2 == cero)
                  p+= n;
                else
                  p-= n;
              }
          }
          break;
      }
    return p;
  }

//! @brief Devuelve el determinante.
//! 
template <class numero>
numero matrizZ<numero>::GetDet(const numero &eps) const
  { return DMGT(eps); }

template <class numero>
std::ostream &operator<<(std::ostream &os,const matrizZ<numero> &m)
  {
    os << '[';
    register typename matrizZ<numero>::size_type i,j;
    typename matrizZ<numero>::size_type fls= m.getNumFilas(),cls= m.getNumCols();
    for(i= 1;i<=fls;i++)
      {
	os << '[';
        if(cls > 0) os << m(i,1);
	for(j= 2;j<=cls;j++)
	  os << ',' << m(i,j);
	os << ']';
      }
    os << ']';
    return os;
  }

template <class numero>
std::istream &operator>>(std::istream &is,matrizZ<numero> &m)
  {
    m.Input(is);
    return is;
  }

template<class numero>
inline matrizZ<numero> operator+(const matrizZ<numero> &m1,const matrizZ<numero> &m2)
  {
    matrizZ<numero> suma(m1);
    check_sum(m1,m2);
    suma.Suma(m2);
    return suma;
  }

template<class numero>
inline matrizZ<numero> operator*(const matrizZ<numero> &m1,const matrizZ<numero> &m2)
  {
    check_prod(m1,m2);
    matrizZ<numero> producto(m1.getNumFilas(),m2.getNumCols());
    size_t nfilas= m1.getNumFilas();
    size_t ncols_m1= m1.getNumCols();
    size_t ncols= m2.getNumCols();
    typedef typename matrizZ<numero>::size_type sz_type;
    for(register sz_type i=1;i<=nfilas;i++)
      for(register sz_type j=1;j<=ncols;j++)
        {
          register numero t= m1(i,1) * m2(1,j); //Inicializamos (pueden ser matrices por cajas).
          for(register sz_type k=2;k<= ncols_m1;k++)
            t+= m1(i,k) * m2(k,j);
          producto(i,j)= t;
        }
    return producto;
  }

template<class numero>
void matrizZ<numero>::TSRGT(numero eps,size_t *it,matrizZ<numero> &C,matrizZ<size_t> &Kp,matrizZ<size_t> &Lp) const
/*The function TSRGT applies to input real square matrix A(n,n) the upper
triangularization algorithm of Gauss method with full pivoting and keeps
trace of successive transformations done in integer vectors KP and LP.
----------------------------------------------------------------------------
  Input parameters:
  eps        precision (real)
  n          size of A matrix (integer)
  A          pointer to input real square matrix (double**)
  Output parameters:
  it         flag=1 if A matrix ok, =0 if A matrix is singular (integer)
  C          pointer to table storing main diagonal elements and supra-
             diagonal elements of upper triangular matrix and the multi-
             plying coefficients used during triangularization process (double**)
  KP         table storing informations concerning the column exchanges
             during process (int*)
  LP         table storing informations concerning the line exchanges
             during process (int*)
-----------------------------------------------------------------------------
The table C is first initialized to A matrix, then receives at each step k
of the triangularization process, usefull elements of A matrix at step k for
k=1,2,...n.
The variables po(real), lo and ko(integer) store respectively pivot at step k,
its line number and its column number.
-----------------------------------------------------------------------------*/
  {
    const size_t n= this->fls;
    register size_t i,j,k;
    size_t ko,lo; numero po,t0;
    //Iguala la matriz C a ésta
    C= *this;
    *it=1; k=1;
    while (*it==1 && k<n)
      {
        po=C(k,k); lo=k; ko=k;
        for(i=k; i<=n; i++)
          for(j=k; j<=n; j++)
            if(fabs(C(i,j))>fabs(po))
              { po=C(i,j); lo=i; ko=j; }
        Lp(k)=lo; Kp(k)=ko;
        if(fabs(po)<eps)
          {
            *it=0;
	    std::cerr << std::endl << "Error: Pivote demasiado pequeño!" << std::endl;
          }
        else
          {
            if(lo!=k)
              for(j=k; j<=n; j++)
                { t0=C(k,j); C(k,j)=C(lo,j); C(lo,j)=t0; }
            if(ko!=k)
              for(i=1; i<=n; i++)
                { t0=C(i,k); C(i,k)=C(i,ko); C(i,ko)=t0; }
	    for(i=k+1; i<=n; i++)
              {
                C(i,k)/= po;
                for(j=k+1; j<=n; j++)
                  C(i,j)-= C(i,k)*C(k,j);
	      }
            k++;
          }
      }
    if(*it==1 && fabs(C(n,n))<eps) *it=0;
  } //TSRGT()

template <class numero>
numero matrizZ<numero>::DMGT(const numero &eps) const
/*The function DMGT returns the determinant of a real square matrix
A(n,n) by Gauss method with full pivoting.
----------------------------------------------------------------------------
  Input parameters:
  eps        precision (double)
  n          size of A matrix (int)
  A          pointer to input real square matrix (double**)
  Output parameters:
  None
-----------------------------------------------------------------------------
The procedure TSRGT is used to reduce A matrix to an upper triangular matrix.
Output variables are it(integer), C(pMat), Kp and Lp(pVecI).
If it=0, matrix A is singular, if it=1, matrix A is regular. Table C contains
at location i,j (j>=i) the corresponding element of the upper triangular matrix.
Tables Lp and Kp contain informations relative to exchanges of line or column
that occured during the process. For instance, the element number k of Lp is
an integer <> k if an exchange of line has been made at step k (k=1,2,...,n).
The number of exchanges of lines and columns is stored in L(integer). the
determinant of A matrix is stored in d0(real).
Note: types pMat and pVecI must be declared and allocated for by main program,
except local variables C,Kp,Lp allocated (and disposed of) here.
-----------------------------------------------------------------------------*/
  {
    const size_t n= this->fls;
    size_t it;
    register size_t k,l;
    numero d0;
    const numero cero= neutro_suma(d0);
    const numero uno= neutro_producto(d0);
    matrizZ<numero> C(n,n);
    matrizZ<size_t> Kp(n,1,0), Lp(n,1,0);

    TSRGT(eps,&it,C,Kp,Lp);  //call triangularization procedure

    if(it==0) //matriz singular, det=0
      return cero;  
    else //matriz regular, det<>0
      {         
        d0=uno;
        for(k=1; k<=n; k++)  d0 *= C(k,k);
        l=0;
        for(k=1; k<n; k++)
          {
            if (Lp(k)!=k)  l++;
            if (Kp(k)!=k)  l++;
          }
        if ((l%2)!=0)  d0=-d0;
      }
    return d0;  //devuelve el determinante
  } //DMGT

//! @brief Devuelve la «caja» comprendida entre los índices que se pasan como parámetro.
template <class numero>
matrizZ<numero> matrizZ<numero>::GetCaja(size_t f1, size_t c1, size_t f2, size_t c2) const
  {
    this->check_get_caja(f1,c1,f2,c2);
    matrizZ<numero> caja(f2-f1+1,c2-c1+1);
    for(register size_type i=1;i<=caja.fls;i++)
      for(register size_type j=1;j<=caja.cls;j++)
        caja(i,j)= matrizZ<numero>::operator()(i+f1-1,j+c1-1);
    return caja;
  }

//! @brief Devuelve la traza de la matriz.
template <class numero>
numero matrizZ<numero>::Traza(void) const
  {
    this->check_traza();
    numero n= numero();
    for(register size_type i= 1;i<=this->fls;i++) n+= (*this)(i,i);
    return n;
  }
template <class numero>
numero matrizZ<numero>::suma_de_fila(size_type i) const
  {
    numero sumaf= (*this)(i,1);
    for(register size_t j=2;j<=this->cls;j++)
      sumaf+= (*this)(i,j);
    return sumaf;
  }
template <class numero>
numero matrizZ<numero>::suma_de_columna(size_type j) const
  {
    numero sumac= (*this)(1,j);
    for(register size_t i=2;i<=this->cls;i++)
      sumac+= (*this)(i,j);
    return sumac;
  }
template <class numero>
numero matrizZ<numero>::maximo_de_fila(size_type i) const
  {
    numero maxf= (*this)(i,1);
    for(register size_t j=2;j<=this->cls;j++)
      maxf= std::max(maxf,(*this)(i,j));
    return maxf;
  }
template <class numero>
numero matrizZ<numero>::maximo_de_columna(size_type j) const
  {
    numero maxc= (*this)(1,j);
    for(register size_t i=2;i<=this->cls;i++)
      maxc+= std::max(maxc,(*this)(i,j));
    return maxc;
  }
//! @brief Devuelve el valor máximo de los elementos del vector
//! que resulta de sumar los elementos de las filas.
template <class numero>
numero matrizZ<numero>::NormaPorFilas(void) const
  {
    numero maximo= suma_de_fila(1);
    for(register size_t i=2;i<=this->fls;i++) maximo= max(maximo,suma_de_fila(i));
    return maximo;
  }
//! @brief Devuelve el valor máximo de los elementos del vector
//! que resulta de sumar los elementos de las columnas.
template <class numero>
numero matrizZ<numero>::NormaPorColumnas(void) const
  {
    numero maximo= suma_de_columna(1);
    for(register size_t j=2;j<=this->cls;j++) maximo= max(maximo,suma_de_columna(j));
    return maximo;
  }
template <class numero>
numero matrizZ<numero>::dot(const matrizZ<numero> &v2) const
//Producto escalar de este por v2.
//v2: Vector columna.
  {
    check_dot(*this,v2);
    numero retval((*this)(1,1)*v2(1,1));
    register typename matrizZ<numero>::size_type i;
    for(i=2;i<=this->cls;i++)
      { retval+= (*this)(1,i) * v2(i,1); }
    return retval;
  }

//! @brief Devuelve el cuadrado del módulo (norma euclídea) de la matriz.
template <class numero>
numero matrizZ<numero>::Abs2(void) const
  {
    numero r= numero();
    const matrizZ<numero> trn= this->GetTrn();
    if(this->EsFila()) //Es matriz fila.
      r= dot(trn);
    else
      if(this->EsColumna()) //Es matriz columna.
        r= trn.dot(*this);
      else //Es cualquiera
        {
          const matrizZ<numero> prod= (*this)*trn;
          r= prod.Traza();
        }
    return r;
  }

//! @brief Devuelve el módulo (norma euclídea) de la matriz.
template <class numero>
numero matrizZ<numero>::Abs(void) const
  { return sqrt(Abs2()); }

//! @brief Lectura desde istream.
template <class numero>
void matrizZ<numero>::Input(std::istream &is)
  {
    char c;
    typename matrizZ<numero>::lst_numero ln;
    size_t fls=0,cls=1;
    int sigue= 1;
    numero ni;
    is >> c;
    if(c != '[') is.putback(c);
    std::istream::sentry ipfx(is);
    while(ipfx)
      {
        is >> c;
        switch(c)
          {
	  case '[':
            fls++;
            break;
          case ']':
            sigue= 0;
            is >> c;
            if(c != ']')
              {
                is.putback(c);
                continue;
              }
            else
              {
                (*this)= matrizZ<numero>(fls,cls,ln.begin(),ln.end());
                return;
              }
            break;
          case ',':
            if(sigue) cls++;
            break;
          default:
            break;
          }
        is >> ni;
        ln.push_back(ni);
      }
    (*this)= matrizZ<numero>(fls,cls,ln.begin(),ln.end());
    return;
  }

//! @brief Lectura desde string.
template <class numero>
void matrizZ<numero>::Input(const std::string &str)
  {
    std::istringstream iss(str);
    Input(iss);
  }

//! @brief Devuelve la norma de la matriz.
template <class treal>  
inline treal Abs(const matrizZ<treal> &m)
  { return m.Abs(); }

#endif

