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
//op_dif.h
//Operadores diferenciales.
#ifndef OP_DIF_H
#define OP_DIF_H

#include "xc_basic/src/matrices/m_double.h"
#include "xc_basic/src/util/matem.h"
#include "xc_basic/src/util/mchne_eps.h"

inline double calcula_incremento(const double &t)
  {
    double retval(sqrt_mchne_eps_dbl); // sqrt_mchne_eps_dbl: Valor de la raiz cuadrada de
                                       //la precisión de la máquina.
    if(t != 0.0) retval*=fabs(t);
    return retval;
  }

template<class F>
inline double parcial_fd(const m_double &x,const F &f,const size_t &i)
//Devuelve la derivada parcial de la función f
//en el punto x respecto al parámetro i empleando
//el método "forward difference" 
  {
    m_double x2(x);
    register const double temp= x2(i);
    register const double h= calcula_incremento(temp);
    x2(i)+=h;
    return (f(x2)-f(x))/h;
  }

template<class F>
inline double parcial_cdf(const m_double &x,const F &f,const size_t &i)
//Devuelve la derivada parcial de la función f
//en el punto x respecto al parámetro i empleando
//el método "extended central difference" 
  {
    m_double x_2(x); //Punto a -2*h de x
    m_double x_1(x); //Punto a -h de x
    m_double x1(x);  //Punto a +x de x
    m_double x2(x);  //Punto a +2h de x
    register const double temp= x(i);
    register const double h= calcula_incremento(temp);
    register const double h2= 2.0*h;
    x_2(i)-=h2;
    x_1(i)-=h;
    x1(i)+=h;
    x2(i)+=h2;
    return (f(x_2)-8.0*f(x_1)+8.0*f(x1)-f(x2))/(12.0*h);
  }

template<class F>
inline m_double gradiente_cdf(const m_double &x,const F &f)
//Devuelve el vector gradiente de f en el punto x
//empleando el método "extended central difference"
  {
    m_double g(x);
    size_t fls= x.getNumFilas();
    for(register size_t i= 1;i<=fls;i++)
      g(i)= parcial_cdf(x,f,i);
    return g;
  }

template<class F>
inline m_double gradiente_cdf_fila(const m_double &x,const F &f)
//Devuelve el vector gradiente de f en el punto x
//empleando el método "extended central difference"
//como vector fila
  {
    m_double g(1,x.getNumFilas());
    size_t fls= x.getNumFilas();
    for(register size_t i= 1;i<=fls;i++)
      g(1,i)= parcial_cdf(x,f,i);
    return g;
  }

template<class MF>
inline m_double jacobiano_cdf(const m_double &x,const MF &mf)
//Devuelve el vector jacobiano de f en el punto x
//empleando el método "extended central difference"
  {
    size_t fls= mf.getNumFilas();
    size_t cls= x.getNumFilas();
    m_double J(fls,cls);
    for(register size_t i= 1;i<=fls;i++)
      {
        m_double g= gradiente_cdf_fila(x,mf(i));
        J.PutFila(i,g);
      }
    return J;
  }

#endif
