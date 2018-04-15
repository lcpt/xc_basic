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
    size_t n_rows; //number of rows.
    size_t n_columns; //number of columns.
    inline void PutDim(size_t nRows,size_t nCols)
      {
        n_rows= nRows;
        n_columns= nCols;
      }
    inline void inic(size_t n_rows,size_t n_columns)
      { PutDim(n_rows,n_columns); }
    inline virtual bool check_range(const size_t &iRow,const size_t &col) const
      { return ((iRow<=n_rows) && (col<=n_columns)); }
    inline void check_put_caja(size_t f,size_t c,const ProtoMatriz &caja) const
      {
        if(((f+caja.getNumberOfRows())>(n_rows+1)) || ((c+caja.getNumberOfColumns())>(n_columns+1)))
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
                    << "  this: " << " (" << n_rows << 'x' << n_columns << ')' << std::endl
                    << "  m= " << " (" << m.n_rows << 'x' << m.n_columns << ')' << std::endl;
      }
    inline void check_sto_dif(const ProtoMatriz &m) const
      {
        if (!CompDim(*this,m))
          std::cerr << "Matrices de dimensiones distintas en operador -="  << std::endl
                    << "  this: " << " (" << n_rows << 'x' << n_columns << ')' << std::endl
                    << "  m= " << " (" << m.n_rows << 'x' << m.n_columns << ')' << std::endl;
      }
    inline void check_traza(void) const
      {
        if(!Cuadrada())
          std::cerr << "La matriz no es cuadrada." << std::endl;
      }
    inline friend int check_dot(const ProtoMatriz &v1,const ProtoMatriz &v2)
      {
        if (!v1.isRow())
          std::cerr << "First matrix of scalar product is not a row matrix."
		    << std::endl;
        if (!v2.isColumn())
          std::cerr << "Second matrix of scalar product is not a column matrix."
		    << std::endl;
        if (v1.n_columns != v2.n_rows)
          std::cerr << "Matrices de dimensiones incompatibles en producto escalar."
                    << "  m1= " << v1 << " (" << v1.n_rows << 'x' << v1.n_columns << ')' << std::endl
                    << "  m2= " << v2 << " (" << v2.n_rows << 'x' << v2.n_columns << ')'
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
        if (m1.n_columns != m2.n_rows)
          {
            std::cerr << "Matrices de dimensiones incompatibles en producto." << std::endl;
            std::cerr << "  m1= " << m1 << std::endl;
            std::cerr << "  m2= " << m2 << std::endl;
            return 0;
          }
        return 1;
      }
  public:
    ProtoMatriz(size_t n_rows= 1,size_t n_columns= 1)
      { inic(n_rows,n_columns); }
    ProtoMatriz(const ProtoMatriz &otra)
      { inic(otra.n_rows,otra.n_columns); }
    ProtoMatriz& operator =(const ProtoMatriz &m)
      {
        inic(m.n_rows,m.n_columns);
        return *this;
      }
    virtual ~ProtoMatriz(void) {}
    inline virtual void resize(size_t n_rows,size_t n_columns)
      { inic(n_rows,n_columns); }
    inline virtual size_t Tam(void)
      { return (n_rows*n_columns); }
    inline size_t getNumberOfRows(void) const
      { return n_rows; }                    
    inline size_t getNumberOfColumns(void) const
      { return n_columns; }
    inline bool CheckIndices(const size_t &f,const size_t &c) const
      { return check_range(f,c);; }
    //! @brief Return verdadero si los índices corresponden a un elemento
    //! "interior" de la matriz.
    inline bool interior(const size_t &i,const size_t &j) const
      { return ( (i>1) && (j>1) && (i<n_rows) && (j<n_columns) ); }
    inline int Cuadrada(void) const
      { return (n_rows == n_columns); }
    inline bool isRow(void) const
      { return (n_rows == 1); }
    inline bool isColumn(void) const
      { return (n_columns == 1); }
    virtual void Print(std::ostream &os) const=0;
    friend inline bool compareRowNumber(const ProtoMatriz &m1,const ProtoMatriz &m2)
      { return (m1.n_rows == m2.n_rows); }
    friend inline bool compareColumnNumber(const ProtoMatriz &m1,const ProtoMatriz &m2)
      { return (m1.n_columns == m2.n_columns); }
    friend inline bool CompDim(const ProtoMatriz &m1,const ProtoMatriz &m2)
      { return (compareRowNumber(m1,m2) && compareColumnNumber(m1,m2)); }
    friend inline std::ostream &operator<<(std::ostream &os,const ProtoMatriz &m)
      {
        m.Print(os);
        return os;
      }
  };

#endif
