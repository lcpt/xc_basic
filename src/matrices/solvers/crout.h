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
//Crout.h
//Procedimiento de crout para la soluciOn de un sistema de ecuaciones lineal.

#ifndef CROUT_H
#define CROUT_H

#include <cmath>
#include "xc_basic/src/matrices/matrizZ.h"
#include "xc_basic/src/util/matem.h"
#include "xc_basic/src/matrices/solvers/pivote.h"
#include "xc_basic/src/matrices/solvers/calcularx.h"

template <class treal>
void fila( const size_t &j,
           matrizZ<treal> &a,
           matrizZ<treal> &b,
           const set_szt &fp,
           size_t *pivot)
  {
    size_t i,k,l=pivot[j],n=a.getNumFilas();
    treal s,r= gj_inv(a(l,j));
    for(k=j+1;k<=n;k++)
      {
        s=a(l,k); i=1;
        while(i<j)
          {
            s-=a(l,i)*a(pivot[i],k);
            i++;
          }
        a(l,k)=s*r;
      }
    s=b(l,1); i=1;
    while(i<j)
      {
        s-=a(l,i)*b(pivot[i],1);
        i++;
      }
    b(l,1)=s*r;
  }

template <class treal>
void colum( const size_t &j,
            matrizZ<treal> &a,
            matrizZ<treal> &b,
            const set_szt &fp,
            size_t *pivot)
  {
    size_t l,i,n= a.getNumFilas();
    for(i=1;i<=n;i++)
    if (!In(fp,i))
      if(j<=n)
        for(l=1;l<=j-1;l++)
          a(i,j)-=a(i,l)*a(pivot[l],j);
      else
        for(l=1;l<=j-1;l++)
          b(i,1)-=a(i,l)*b(pivot[l],1);      
  }

template <class treal>
matrizZ<treal> crout(matrizZ<treal> &a,matrizZ<treal> &b,int &regular)
  {
    //Dimensionamos la matriz de indices de pivote.
    typedef typename matrizZ<treal>::size_type size_type;
    const size_type n= a.getNumFilas();
    size_type *pivot= new size_type[n];
    set_szt fp;
    regular=1; size_t j=0;
    while(regular && (j<n))
      {
        j++;
        pivot[j]=pivote(j,a,fp);
        regular=(pivot[j]>0);
        if(regular)
          {
            fp.insert(pivot[j]);
            fila(j,a,b,fp,pivot); colum(j+1,a,b,fp,pivot);
          }
      }
    matrizZ<treal> x;
    if(regular) x= calcularx(a,b,pivot);
    delete[] pivot;
    return x;
  }
#endif




