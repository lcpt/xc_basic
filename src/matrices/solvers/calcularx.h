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
//calcularx.h
//Procedimiento para la sustitucion hacia atras en los mEtodos
//de Gauss y Crout para la solucion de un sistema de ecuaciones lineal.

#ifndef CALCULARX_H
#define CALCULARX_H

#include <cmath>
#include "xc_basic/src/matrices/matrizZ.h"
#include <set>
#include "xc_basic/src/matrices/solvers/gj_aux.h"

template <class treal>
matrizZ<treal> calcularx( const matrizZ<treal> &a,
                          const matrizZ<treal> &b,
                          size_t *pivot)
  {
    size_t j,l,k,n= a.getNumberOfRows(),m= b.getNumberOfColumns();
    matrizZ<treal> x(n,m,0.0);
    for(j=n;j>=1;j--)
      {
        l=pivot[j-1];
        for(size_t c=1;c<=m;c++) x(j,c)=b(l,c);
        k=j;
        while(k<n)
          {
            k++;
            size_t c;
            for(c=1;c<=m;c++) x(j,c)-= a(l,k)*x(k,c);
          }
      }
    return x;
  }
#endif
