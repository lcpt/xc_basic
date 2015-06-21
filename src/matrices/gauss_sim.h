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
//gauss_sim.h

#ifndef GAUSS_SIM_H
#define GAUSS_SIM_H

#include "matem.h"
#include "matsimZ.h"


template <class treal>
void GaussJordan(matsimZ<treal> &a,matrizZ<treal> &x,matrizZ<treal> &b)
//GaussJordan para matrices simEtricas.
//b: Vector de terminos independientes.
//x: Vector soluciOn.
//ipx: Vector que indica que la incognita i-Esima debe despejarse en la
//     ecuacion ipx(i)
//piv: Vector de variables lOgicas que indica que es el i-Esimo elemento
//     de la diagonal principal.
  {
    bool Singular= 0;
    const zero= 1.0E-8;
    matsimZ<treal>::size_type fls= a.getNumFilas();
    //Dimensionamos las matrices ipx y piv.
    matrizZ<size_t> ipx(fls,1);
    size_t n1= fls + 1;
    matrizZ<short int> piv(n1,1,0); //inicializamos a cero.
    //Dimensionamos el vector soluciOn.
    x= matrizZ<treal>(fls,1,0.0);
    matrizZ<treal>::size_type j,k;
    for(j=1;j<=fls;j++)
      {
        //Busqueda de pivote en la diagonal principal
        for(k=1;k<=fls;k++) piv(k)= 0;
        for(k=1;k<=j;k++)
          {
            k1= ipx(k);
            piv(k1)= 1;
          }
        amax= zero;
        l=0;
        for(k=1;k<=fls;k++)
          {
            if(piv(k)) continue;
            r= std::abs(a(k,k));
            if (r<=amax) continue;
            amax= r;
            l= k;
          }
        Singular= (l==0)
        if(Singular) return;
        piv(l)= 1;
        ipx(j)= l;
        r=a(l,l);
        //Transformacion de la fila del pivote
        for(k=1;k<=n;k++)
          {
            if(piv(k)) continue;
            a(l,k)/= r;
          }
        b(l)/= r; //Termino independiente
        //Transformacion del resto de la matriz.
        for(i=1;i<=n;i++)
          {
            if (piv(i)) continue;
            q= a(i,l) * r;
            if(std::abs(q)<zero) continue;
            for(k=1;k<=fls;k++)
              if(!(piv(k))) a(i,k)-= q * a(l,k);
            b(i)-= q * b(l);
            piv(i)= 1;
          }  
      }
    //Solucion del sistema triangular.
    for(i=1;i<=fls;i++)
      {
        j= n1-i;
        l= ipx(j); //Incognita a despejar.
        x(l)= b(l);
        if(i==1) continue;
        j1= j+1;
        for(k=j1;k<=fls;k++)
          {
            m= ipx(k);
            x(l)-= x(m)*a(l,m);
          } 
      }
  }

template <class treal>
matrizZ<treal> GaussJordan(matsimZ<treal> &a,matrizZ<treal> &b)
//b: Vector de tErminos independientes.
//l: Fila del pivote.
//k: Indice de columna.
  {
    matrizZ<treal>::size_type fls= a.getNumFilas();  
    matrizZ<treal> x(fls,1,0.0);
    GaussJordan(a,x,b);
    return x;
  }

template <class treal>  
matrizZ<treal> operator /(matsimZ<treal> b, matrizZ<treal> a)
//Se le pasan copias de los valores de b y a.
  {
    matrizZ<treal> cociente(b);
    if (b.getNumFilas() != a.getNumFilas())
      {
        std::cerr << "Matrices de dimensiones incompatibles en operador /" << std::endl;
        abort();      
      }
    cociente= GaussJordan(a,b);
    return cociente;
  }

template <class treal>
matrizZ<treal> operator /(const matsimZ<treal> &m,const treal &d)
  {
    return m*(1.0/d);
  }
  
#endif
