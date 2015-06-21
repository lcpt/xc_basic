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
//ProtoMatriz.h 
#ifndef PROTOMATRIZ_H
#define PROTOMATRIZ_H

#include <string>
#include <iostream>

class ProtoMatriz
  {
  protected:
    size_t fls; //filas.
    size_t cls; //columnas.
    inline void PutDim(size_t filas,size_t cols)
      {
        fls= filas;
        cls= cols;
      }
    inline void inic(size_t filas,size_t cols)
      { PutDim(filas,cols); }
    inline virtual bool check_range(const size_t &fila,const size_t &col) const
      { return ((fila<=fls) && (col<=cls)); }
    inline void check_put_caja(size_t f,size_t c,const ProtoMatriz &caja) const
      {
        if(((f+caja.getNumFilas())>(fls+1)) || ((c+caja.getNumCols())>(cls+1)))
          std::cerr << "Indices erróneos en función PutCaja." << std::endl;
      }
    inline void check_get_caja(size_t f1, size_t c1, size_t f2, size_t c2) const
      {
        if ( (f2 < f1) || (c2 < c1) )
          std::cerr << "Indices erróneos en función GetCaja." << std::endl;
      }
    inline void check_sto_sum(const ProtoMatriz &m) const
      {
        if (!CompDim(*this,m))
          std::cerr << "Matrices de dimensiones distintas en operador += " << std::endl
                    << "  this: " << " (" << fls << 'x' << cls << ')' << std::endl
                    << "  m= " << " (" << m.fls << 'x' << m.cls << ')' << std::endl;
      }
    inline void check_sto_dif(const ProtoMatriz &m) const
      {
        if (!CompDim(*this,m))
          std::cerr << "Matrices de dimensiones distintas en operador -="  << std::endl
                    << "  this: " << " (" << fls << 'x' << cls << ')' << std::endl
                    << "  m= " << " (" << m.fls << 'x' << m.cls << ')' << std::endl;
      }
    inline void check_traza(void) const
      {
        if(!Cuadrada())
          std::cerr << "La matriz no es cuadrada." << std::endl;
      }
    inline friend int check_dot(const ProtoMatriz &v1,const ProtoMatriz &v2)
      {
        if (!v1.EsFila())
          std::cerr << "La primera matriz del producto escalar no es fila." << std::endl;
        if (!v2.EsColumna())
          std::cerr << "La segunda matriz del producto escalar no es columna." << std::endl;
        if (v1.cls != v2.fls)
          std::cerr << "Matrices de dimensiones incompatibles en producto escalar."
                    << "  m1= " << v1 << " (" << v1.fls << 'x' << v1.cls << ')' << std::endl
                    << "  m2= " << v2 << " (" << v2.fls << 'x' << v2.cls << ')'
                    << std::endl;
        return 1;
      }
    inline friend int check_sum(const ProtoMatriz &m1,const ProtoMatriz &m2)
      {
        if (!CompDim(m1,m2))
          {
            std::cerr << "Matrices de dimensiones distintas en operador +" 
                 /* << m1.GetStrDim() << ' ' << m2.GetStrDim() */ << std::endl;
            return 0;
          }
        return 1;
      }
    inline friend int check_dif(const ProtoMatriz &m1,const ProtoMatriz &m2)
      {
        if (!CompDim(m1,m2))
          {
            std::cerr << "Matrices de dimensiones distintas en operador -"  
                 /* << m1.GetStrDim() << ' ' << m2.GetStrDim() */ << std::endl;
            return 0;
          }
        return 1;
      }
    inline friend int check_prod(const ProtoMatriz &m1,const ProtoMatriz &m2)
      {
        if (m1.cls != m2.fls)
          {
            std::cerr << "Matrices de dimensiones incompatibles en producto." << std::endl;
            std::cerr << "  m1= " << m1 << std::endl;
            std::cerr << "  m2= " << m2 << std::endl;
            return 0;
          }
        return 1;
      }
  public:
    ProtoMatriz(size_t filas= 1,size_t cols= 1)
      { inic(filas,cols); }
    ProtoMatriz(const ProtoMatriz &otra)
      { inic(otra.fls,otra.cls); }
    ProtoMatriz& operator =(const ProtoMatriz &m)
      {
        inic(m.fls,m.cls);
        return *this;
      }
    virtual ~ProtoMatriz(void) {}
    inline virtual void resize(size_t filas,size_t cols)
      { inic(filas,cols); }
    inline virtual size_t Tam(void)
      { return (fls*cls); }
    inline size_t getNumFilas(void) const
      { return fls; }                    
    inline size_t getNumCols(void) const
      { return cls; }
    inline bool CheckIndices(const size_t &f,const size_t &c) const
      { return check_range(f,c);; }
    //! @brief Devuelve verdadero si los índices corresponden a un elemento
    //! "interior" de la matriz.
    inline bool interior(const size_t &i,const size_t &j) const
      { return ( (i>1) && (j>1) && (i<fls) && (j<cls) ); }
    inline int Cuadrada(void) const
      { return (fls == cls); }
    inline bool EsFila(void) const
      { return (fls == 1); }
    inline bool EsColumna(void) const
      { return (cls == 1); }
    virtual void Print(std::ostream &os) const=0;
    friend inline bool CompFilas(const ProtoMatriz &m1,const ProtoMatriz &m2)
      { return (m1.fls == m2.fls); }
    friend inline bool CompCols(const ProtoMatriz &m1,const ProtoMatriz &m2)
      { return (m1.cls == m2.cls); }
    friend inline bool CompDim(const ProtoMatriz &m1,const ProtoMatriz &m2)
      { return (CompFilas(m1,m2) && CompCols(m1,m2)); }
    friend inline std::ostream &operator<<(std::ostream &os,const ProtoMatriz &m)
      {
        m.Print(os);
        return os;
      }
  };

#endif
