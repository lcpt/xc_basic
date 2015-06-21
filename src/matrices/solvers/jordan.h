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
//Jordan.h
//Procedimiento de Jordan para la solucion de un sistema de ecuaciones lineal.

#ifndef JORDAN_H
#define JORDAN_H

#include <cmath>
#include "xc_basic/src/matrices/matrizZ.h"
#include "xc_basic/src/util/matem.h"
#include "xc_basic/src/matrices/solvers/pivote.h"

template <class treal>
void eliminar( matrizZ<treal> &a,
               matrizZ<treal> &b,
               const size_t &j,size_t *pivot)
  {
    size_t i,k,l,n= a.getNumFilas(),m= b.getNumCols();
    l=pivot[j]; treal r=gj_inv(a(l,j));
    for(k=j+1;k<= n;k++) a(l,k)*=r;
    for(k=1;k<=m;k++) b(l,k)*=r;
    for(i=1;i<=n;i++)
      if ((i!=l) && (gj_abs(a(i,j))>zero))
	{
          for(k=j+1;k<=n;k++) a(i,k)-= a(i,j)*a(l,k);
          for(k=1;k<=m;k++) b(i,k)-= a(i,j)*b(l,k);
        }
  }

template <class treal>
matrizZ<treal> calcularx( const matrizZ<treal> &a,
                          const matrizZ<treal> &b,
                          size_t *pivot)
  {
    size_t n= a.getNumFilas(),m=b.getNumCols();
    matrizZ<treal> x(n,m,0.0);
    for(size_t j=1;j<= n;j++)
      for(size_t k=1;k<=m;k++) x(j,k)= b(pivot[j],k);
    return x;
  }

template <class treal>
matrizZ<treal> jordan(matrizZ<treal> &a,matrizZ<treal> &b,int &regular)
  {
    size_t j;
    //Dimensionamos la matriz de indices de pivote.
    typedef typename matrizZ<treal>::size_type size_type;
    const size_type n= a.getNumFilas();
    size_type *pivot= new size_type[n];
    set_szt fp;
    regular= 1; j=0;
    while (regular && (j<n))
      {
        j++;
        pivot[j]= pivote(j,a,fp);
        regular= (pivot[j]>0);
        if(regular)
          {
            fp.insert(pivot[j]);
            eliminar(a,b,j,pivot);
          }
      }
     matrizZ<treal> x;
     if(regular) x= calcularx(a,b,pivot);
     delete[] pivot;
     return x;
  }

template <class treal>
matrizZ<treal> jordan_const(const matrizZ<treal> &a,const matrizZ<treal> &b,int &regular)
  {
    matrizZ<treal> c=a,d=b;
    return jordan(c,d,regular);
  }

template <class treal>  
matrizZ<treal> operator /(const matrizZ<treal> &b, const matrizZ<treal> &a)
//Se le pasan copias de los valores de b y a.
  {
    if (b.getNumFilas() != a.getNumFilas())
      {
        std::cerr << "Matrices de dimensiones incompatibles en operador /" << std::endl;
        abort();      
      }
    int regular;
    matrizZ<treal> x= jordan_const(a,b,regular);
    if (!regular) std::cerr << " matriz singular" << std::endl;
    return x;
  }

template <class treal>
matrizZ<treal> operator /(const matrizZ<treal> &m,const treal &d)
  { return m*gj_inv(d); }

#endif
