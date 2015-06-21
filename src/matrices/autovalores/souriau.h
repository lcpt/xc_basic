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
//souriau.h
//Procedimiento de Souriau para la obtencion de la ecuacion caracterIstica
//de una matriz.

#ifndef SOURIAU_H
#define SOURIAU_H

#include <cmath>
#include "xc_basic/src/matrices/matrizZ.h"
#include "xc_basic/src/util/matem.h"

template <class treal>
matrizZ<treal> calbk(const matrizZ<treal> &ak,const treal &tk)
  {
    size_t n= ak.getNumFilas();
    matrizZ<treal> b(ak);
    for(size_t i=1;i<=n;i++)
      b(i,i)+=tk;
    return b;  
  }

template <class treal>  
vectorZ<treal> souriau(matrizZ<treal> &a)
//Procedimiento de Souriau para la obtenciOn de la ecuaciOn caracterIstica
//de una matriz.
//Devuelve un 
//	a:	Matriz de la que se obtiene la ecuaciOn caracterIstica.
//	Valor devuelto:
//		vector en el que el elemento de Indice (k,1) es el
//		coeficiente de grado n-k del polinomio.
  {
    size_t n= a.getNumFilas();
    vectorZ<treal> t(n+1,1);    
    matrizZ<treal>  ak(n,n,0.0),b(n,n);
    t[0]= 1;
    for(size_t k=1;k<=n;k++)
      {
        b= calbk(ak,t[k-1]);
        ak= a*b;
        t[k]=-ak.Traza()/k;
      }
    return t;
  }
#endif
