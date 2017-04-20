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
//matdispZ.h
//Matrices dispersas.

#ifndef MATDISPZ_H
#define MATDISPZ_H

#include "matrizZ.h"
#include <map>
#include <algorithm>

template <class numero>
class matdispZ : public matrizZ<numero>
  {
  public:
    typedef matrizZ<numero> matrizZ_numero;
    typedef typename matrizZ_numero::size_type size_type;
    typedef std::map<size_type,numero> map_elem;

    class sp_vector: public map_elem
      {
      public:
        typedef typename map_elem::iterator iterator;
        typedef typename map_elem::const_iterator const_iterator;
        typedef typename map_elem::const_reverse_iterator const_reverse_iterator;
      private:
        inline int TieneFila(const typename sp_vector::size_type f) const
          { return (find(f)!=this->end()); }
      public:
        sp_vector &operator+=(const sp_vector &v);
        sp_vector &operator-=(const sp_vector &v);
        void QuitaElem(const numero &n);
        void PutCol(const typename sp_vector::size_type c,matrizZ_numero &m) const;
        sp_vector getNumFilas(const typename sp_vector::size_type f1,const typename sp_vector::size_type f2) const;
        size_t ndiagL(const size_t &icol) const;
        size_t ndiagU(const size_t &icol) const;
        void writeCpp(std::ostream &os,const size_t &icol) const;
	void PutColBanda(const size_t &sz,const size_t &i,const size_t &ndiagu,numero *vptr) const;
      };
    typedef std::map<size_type, sp_vector> map_cols;
    map_cols cols;


    typedef typename sp_vector::const_iterator const_f_iterator;
    typedef typename sp_vector::iterator f_iterator;
    typedef typename map_cols::const_iterator const_c_iterator;
    typedef typename map_cols::iterator c_iterator;

  private:
    inline virtual size_type Tam(size_type filas,size_type cols)
      { return 0; }
    inline virtual size_type Indice(const size_type &fila,const size_type &col) const
      { return 0; }
    inline bool TieneColumna(const size_type &c) const
      { return (cols.find(c)!=cols.end()); }
    inline const numero &PorDefecto(void) const
      { return (matrizZ_numero::operator()(1,1)); }
    bool IgualA(const matdispZ<numero> &otra) const;

    template<class M>
    matdispZ<numero> Post(const M &b) const;

    matdispZ<numero> Post(const matdispZ<numero> &b) const;

  public:

    matdispZ(size_type fls=1,size_type cls= 1)
      : matrizZ_numero(1,1,numero())
      { this->PutDim(fls,cls); }
    matdispZ(const matdispZ<numero> &otra)
      : matrizZ_numero(otra), cols(otra.cols)
      { this->PutDim(otra.fls,otra.cls); }
    matdispZ<numero>& operator =(const matdispZ<numero> &m)
      {
        matrizZ_numero::operator =(m);
        cols= m.cols;
        return *this;
      }
    matdispZ<numero>& operator+=(const matdispZ<numero> &m);
    matdispZ<numero>& operator-=(const matdispZ<numero> &m);


    size_t ndiagL(void) const;
    size_t ndiagU(void) const;
    void FillVectorBanda(numero *vptr) const;
    //Acceso a los elementos.
    numero &operator()(size_t fila=1,size_t col=1)
      {
        assert(col>0 && col<=this->cls);
        assert(fila>0 && fila<=this->fls);
        return cols[col][fila];
      }
    const numero &operator()(size_t fila=1,size_t col=1) const
      {
        const_c_iterator k1= cols.find(col);
        if(k1!=cols.end())
          {
	    const_f_iterator k2= (k1->second).find(fila);
            if(k2!= (k1->second).end())
              return k2->second;
            else
              return PorDefecto();
          }
        else 
          return PorDefecto();
      }
    const_c_iterator ClsBegin(void) const
      { return cols.begin(); }
    const_c_iterator ClsEnd(void) const
      { return cols.end(); }
    c_iterator ClsBegin(void)
      { return cols.begin(); }
    c_iterator ClsEnd(void)
      { return cols.end(); }
    const_f_iterator FlsBegin(const const_c_iterator &ci) const
      { return ci->second.begin(); }
    const_f_iterator FlsFind(const const_c_iterator &ci,size_t f) const
      { return ci->second.find(f); }
    const_f_iterator FlsEnd(const const_c_iterator &ci) const
      { return ci->second.end(); }
    f_iterator FlsFind(const c_iterator &ci,size_t f)
      { return ci->second.find(f); }
    sp_vector &GetCol(const c_iterator &ci)
      { return ci->second; }

    size_type size(void) const;
    void Compacta(void);
    void Anula(void);
    void Identity(void);
    inline bool Cuadrada(void) const
      { return (this->fls == this->cls); }
    matdispZ<numero> &Trn(void);
    matdispZ<numero> GetTrn(void)
      {
        matdispZ<numero> retval(*this);
        retval.Trn();
        return retval;
      }
    matdispZ<numero> GetCaja(size_t f1, size_t c1, size_t f2, size_t c2) const;
    matdispZ<numero> GetFila(size_t fila) const
      { return GetCaja(fila,1,fila,this->cls); }
    matdispZ<numero> GetCol(size_t col) const
      { return GetCaja(1,col,this->fls,col); }
    void PutCaja(size_t f,size_t c,const matdispZ<numero> &caja);
    numero Traza(void) const;
    matrizZ_numero GetCompleta(void) const;

    void writeCpp(std::ostream &os) const;

    template <class V>
    numero dot(const V &v2) const
    //Producto escalar de este por v2.
    //v2: Vector columna.
      {
        check_dot(*this,v2);
        return (this->Post(v2))(1,1);
      }
    numero dot(const matdispZ<numero> &v2) const
    //Producto escalar de este por v2.
    //v2: Vector columna.
      {
        check_dot(*this,v2);
        return (this->Post(v2))(1,1);
      }
    friend matdispZ<numero> operator+(const matdispZ<numero> &m1,const matdispZ<numero> &m2)
      {
        check_sum(m1,m2);
        matdispZ<numero> suma(m1);
        suma+= m2;
        suma.Compacta();
        return suma;
      }
    friend matdispZ<numero> operator-(const matdispZ<numero> &m1,const matdispZ<numero> &m2)
      {
        check_dif(m1,m2);
        matdispZ<numero> resta(m1);
        resta-=m2;
        resta.Compacta();
        return resta;
      }
    inline friend bool operator==(const matdispZ<numero> &m1,const matdispZ<numero> &m2)
      { return m1.IgualA(m2); }
    //Producto escalar de dos vectores.
    //v1: Vector fila.
    //v2: Vector columna.
    friend numero dot(const matdispZ<numero> &v1,const matdispZ<numero> &v2)
      { return v1.dot(v2); }
    friend numero dot(const matdispZ<numero> &v1,const matrizZ<numero> &v2)
      { return v1.dot(v2); }
    friend matdispZ<numero> operator*(const matdispZ<numero> &m1,const matdispZ<numero> &m2)
      {
        check_prod(m1,m2);
        matdispZ<numero> producto(m1.Post(m2));
        return producto;
      }
    friend matdispZ<numero> operator*(const matdispZ<numero> &m1,const matrizZ<numero> &m2)
      {
        check_prod(m1,m2);
        matdispZ<numero> producto(m1.Post(m2));
        return producto;
      }
    /*
    friend matdispZ<numero> operator*(const matdispZ<numero> &m1,const matrizZ<numero> &m2)
      {
        check_prod(m1,m2);
        matdispZ<numero> producto(m1.fls,m2.getNumCols());
        matdispZ<numero>::size_type i=1,j=1;
        for (i= 1;i<=m1.fls;i++)
          for (j= 1;j<=m2.getNumCols();j++)
            producto(i,j) = dot(m1.GetFila(i),m2.GetCol(j));
        return producto;
      }
    */
    /*
    friend matdispZ<numero> operator *(const matdispZ<numero> &m,const numero &p)
      {
        matdispZ<numero> producto(m);
        producto.Prod(p);
        return producto;
      }
    friend matdispZ<numero> operator *(const numero &p,const matdispZ<numero> &m)
      { return m*p; }

    friend std::istream &operator >> (std::istream &stream,matdispZ<numero> &m)
      { return ::operator >>(stream,(matrizZ_numero &) m); }
    */
  };

//sp_vector
template<class numero>
typename matdispZ<numero>::sp_vector &matdispZ<numero>::sp_vector::operator+=(const matdispZ<numero>::sp_vector &v)
  {
    const_iterator f;
    for(f= v.begin();f!=v.end();f++)
      if(TieneFila(f->first))
        (*this)[f->first]+= f->second;
      else
        (*this)[f->first]= f->second;
    return *this;
  }
template<class numero>
typename matdispZ<numero>::sp_vector &matdispZ<numero>::sp_vector::operator-=(const matdispZ<numero>::sp_vector &v)
  {
    const_iterator f;
    for(f= v.begin();f!=v.end();f++)
      if(TieneFila(f->first))
        (*this)[f->first]-= f->second;
      else
        (*this)[f->first]= f->second;
    return *this;
  }
template<class numero>
void matdispZ<numero>::sp_vector::QuitaElem(const numero &n)
  {
    iterator f;
    for(f= this->begin();f!=this->end();f++)
      if(f->second == n)
        this->erase(f);
  }
template<class numero>
void matdispZ<numero>::sp_vector::PutCol(const typename sp_vector::size_type c,matrizZ_numero &m) const
  {
    const_iterator f;
    for(f= this->begin();f!=this->end();f++)
      m(f->first,c)= f->second;
  }
template<class numero>
typename matdispZ<numero>::sp_vector matdispZ<numero>::sp_vector::getNumFilas(const typename sp_vector::size_type f1,const typename sp_vector::size_type f2) const
  {
    if(f2<f1) return;
    sp_vector retval;
    const_iterator f;
    for(f= this->begin();f!=this->end();f++)
      if((f->first>=f1) && (f->first<=f2))
        retval[f->first-f1+1]= f->second;
    return retval;
  }

//! @brief Devuelve el número de diagonales, con algún elemento no nulo, que tiene la columna por debajo de la diagonal principal. 
//! @param icol: Índice de la columna a la que corresponde este vector.
template<class numero>
size_t matdispZ<numero>::sp_vector::ndiagL(const size_t &icol) const
  {
    size_t retval= 0;
    if(!this->empty())
      {
        const numero zero= numero();
        for(const_reverse_iterator f= this->rbegin();f!=this->rend();f++)
          {
            const size_t ifila= f->first;
            if(ifila>icol) //El elemento esta por debajo de la diagonal principal.
              {
                if(f->second != zero) //no nulo.
                  {
                    retval= ifila-icol;
                    break;
                  }
              }
            else //El elemento esta en la diagonal principal o por encima.
              break;
          }
      }
    return retval;
  }

//! @brief Devuelve el número de diagonales, con algún elemento no nulo, que tiene la columna por encima de la diagonal principal. 
//! @param icol: Índice de la columna a la que corresponde este vector.
template<class numero>
size_t matdispZ<numero>::sp_vector::ndiagU(const size_t &icol) const
  {
    size_t retval= 0;
    if(!this->empty())
      {
        const numero zero= numero();
        for(const_iterator f= this->begin();f!=this->end();f++)
          {
            const size_t ifila= f->first;
            if(ifila<icol) //El elemento esta por encima de la diagonal principal.
              {
                if(f->second != zero) //no nulo.
                  {
                    retval= icol-ifila;
                    break;
                  }
              }
            else //El elemento esta en la diagonal principal o por debajo.
              break;
          }
      }
    return retval;
  }

//! @brief Escribe los elementos no nulos de la columna en formato de C++.
template<class numero>
void matdispZ<numero>::sp_vector::writeCpp(std::ostream &os,const size_t &icol) const
  {
    if(!this->empty())
      {
        const numero zero= numero();
        for(const_iterator f= this->begin();f!=this->end();f++)
          if(f->second != zero)
            os << "mtx(" << f->first << ',' << icol << ")= " << f->second << "; ";
      }    
  }

//! @brief Coloca los elementos de la columna que forman parte de la banda
//! en el vector que se pasa como parámetro.
//! @param sz: Ancho de banda.
//! @param icol: Índice de la columna.
//! @param ndiagu: Número de diagonales por encima de la principal.
//! @param vptr: Vector en el que se colocal los elementos.
template<class numero>
void matdispZ<numero>::sp_vector::PutColBanda(const size_t &sz,const size_t &icol,const size_t &ndiagu,numero *vptr) const
  {
    if(!this->empty())
      {
        const numero zero= numero();
        const int offset= (icol-1)*sz-icol+ndiagu;
        for(const_iterator f= this->begin();f!=this->end();f++)
          {
            const size_t ifila_banda= f->first+offset;
            if(f->second != zero)
              vptr[ifila_banda]= f->second;
          }
      }    
  }

//matdispZ
template <class numero>
matdispZ<numero>& matdispZ<numero>::operator+=(const matdispZ<numero> &m)
  {
    const_c_iterator c;
    for(c= m.cols.begin();c!=m.cols.end();c++)
      if(TieneColumna(c->first))
        cols[c->first]+= c->second;
      else
        cols[c->first]= c->second;
    return *this;
  }

template <class numero>
matdispZ<numero>& matdispZ<numero>::operator-=(const matdispZ<numero> &m)
  {
    const_c_iterator c;
    for(c= m.cols.begin();c!=m.cols.end();c++)
      if(TieneColumna(c->first))
        cols[c->first]-= c->second;
      else
        cols[c->first]= c->second;
    return *this;
  }

template <class numero>
typename matdispZ<numero>::size_type matdispZ<numero>::size(void) const
  {
    size_type retval= 0;
    const_c_iterator c;
    for(c= cols.begin();c!=cols.end();c++)
      retval+=c->second.size();
    return retval;
  }

template <class numero>
void matdispZ<numero>::Compacta(void)
  {
    const numero &n= PorDefecto();
    typename map_cols::iterator c;
    for(c= cols.begin();c!=cols.end();c++)
      c->second.QuitaElem(n);
  }

template <class numero>
void matdispZ<numero>::Anula(void)
  { cols.erase(cols.begin(),cols.end()); }

template <class numero>
void matdispZ<numero>::Identity(void)
  {
    this->Anula();
    if(this->fls!=this->cls)
      std::cerr << "matdispZ::" << __FUNCTION__
	        << "not a square matrix: "
	        << this->fls << " x " << this->cls << std::endl;
    const size_t sz= std::min(this->fls,this->cls);
    for(size_t i=1;i<=sz;i++)
      (*this)(i,i)= 1.0;
    Compacta();
  }

template <class numero>
matdispZ<numero> &matdispZ<numero>::Trn(void)
  {
    matrizZ_numero::Trn();
    const_c_iterator c;
    const_f_iterator f;
    for(c= cols.begin();c!=cols.end();c++)
      for(f= c->second.find(c->first+1);f!=c->second.end();f++)
        std::swap((*this)(f->first,c->first),(*this)(c->first,f->first));
    Compacta();
    return *this;
  }

template <class numero>
size_t matdispZ<numero>::ndiagL(void) const
  {
    const_c_iterator c;
    size_t retval= 0;
    for(c= cols.begin();c!=cols.end();c++)
      retval= std::max(retval,c->second.ndiagL(c->first));
    return retval;
  }
template <class numero>
size_t matdispZ<numero>::ndiagU(void) const
  {
    const_c_iterator c;
    size_t retval= 0;
    for(c= cols.begin();c!=cols.end();c++)
      retval= std::max(retval,c->second.ndiagU(c->first));
    return retval;
  }

//! @brief Rellena el vector en banda que se pasa como parámetro para su empleo en Arpack++.
template<class numero>
void matdispZ<numero>::FillVectorBanda(numero *vptr) const
  {
    const size_t ndiagu= ndiagU();
    const size_t ndiagl= ndiagL();
    const size_t ancho_banda= ndiagl+ndiagu+1;
    for(const_c_iterator c= cols.begin();c!=cols.end();c++)
      c->second.PutColBanda(ancho_banda,c->first,ndiagu,vptr);
  }

//! @brief Escribe la matriz en formato de C++ (sólo los elementos no nulos.
template<class numero>
void matdispZ<numero>::writeCpp(std::ostream &os) const
  {
    for(const_c_iterator c= cols.begin();c!=cols.end();c++)
      c->second.writeCpp(os,c->first);
  }


template <class numero>
matdispZ<numero> matdispZ<numero>::GetCaja(size_t f1, size_t c1, size_t f2, size_t c2) const
  {
    // XXX "mejorar esta rutina"
    this->check_get_caja(f1,c1,f2,c2);
    matdispZ<numero> caja(f2-f1+1,c2-c1+1);
    const_c_iterator c;
    const_f_iterator f;
    for(c= cols.begin();c!=cols.end();c++)
      for(f= c->second.begin();f!=c->second.end();f++)
        if( (c->first>=c1) && (c->first<=c2) &&
            (f->first>=f1) && (f->first<=f2))
          caja(f->first-f1+1,c->first-c1+1)= f->second;
    return caja;
  }

template <class numero>
void matdispZ<numero>::PutCaja(size_t f,size_t c,const matdispZ<numero> &caja)
  {
    check_put_caja(f,c,caja);
    size_t i,j;
    for (i=1;i<=caja.fls;i++)
      for (j=1;j<=caja.cls;j++)
        (*this)(i+f-1,j+c-1)= caja(i,j);
    Compacta();
  }

template<class numero>
numero matdispZ<numero>::Traza(void) const
  {
    this->check_traza();
    numero n= numero();
    const_c_iterator c;
    const_f_iterator f;
    for(c= cols.begin();c!=cols.end();c++)
      if((f= c->second.find(c->first)) != c->second.end())
        n+=f->second;
    return n;
  }

template<class numero>
typename matdispZ<numero>::matrizZ_numero matdispZ<numero>::GetCompleta(void) const
  {
    matrizZ_numero retval(this->fls,this->cls,PorDefecto());
    const_c_iterator c;
    for(c= cols.begin();c!=cols.end();c++)
      c->second.PutCol(c->first,retval);
    return retval;
  }

template<class numero>
bool matdispZ<numero>::IgualA(const matdispZ<numero> &otra) const
  {
    if(!CompDim(*this,otra)) return false;
    typename matdispZ<numero>::const_c_iterator c;
    for(c= this->cols.begin();c!=this->cols.end();c++)
      {
        typename matdispZ<numero>::const_f_iterator f;
        for(f= c->second.begin();f!=c->second.end();f++)
          if(f->second != otra(f->first,c->first)) return false;
      }
    return true;
  }

template<class numero>
matdispZ<numero> matdispZ<numero>::Post(const matdispZ<numero> &b) const
//Devuelve el producto de esta matriz por
//la matriz b.
  {
    check_prod(*this,b);
    matdispZ<numero> ret(this->getNumFilas(),b.getNumCols());
    const_c_iterator bc;
    const_c_iterator c;
    const_f_iterator f;
    for(bc= b.cols.begin();bc!=b.cols.end();bc++)
      for(c= cols.begin();c!=cols.end();c++)
        for(f= c->second.begin();f!=c->second.end();f++)
          ret(f->first,bc->first)+= f->second*b(c->first,bc->first);
    return(ret);
  }

template<class numero>
template<class M>
matdispZ<numero> matdispZ<numero>::Post(const M &b) const
//Devuelve el producto de esta matriz por
//la matriz b.
  {
    check_prod(*this,b);
    matdispZ<numero> ret(this->getNumFilas(),b.getNumCols());
    size_type bc;
    const_c_iterator c;
    const_f_iterator f;
    for(bc= 1;bc<=b.getNumCols();bc++)
      for(c= cols.begin();c!=cols.end();c++)
        for(f= c->second.begin();f!=c->second.end();f++)
          ret(f->first,bc)+= f->second*b(c->first,bc);
    return(ret);
  }


#endif
