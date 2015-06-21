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
//Ciclico.h

//Numeros ciclicos como los dias de la semana, los meses, etc...

#ifndef CICLICO_H
#define CICLICO_H

//#include <function.h>
#include <iostream>

template <class numero,numero lim_inf,numero lim_sup>
class Ciclico
  {
  private:
    numero valor;
  protected:
    inline numero dist(void)
      { return (lim_sup - lim_inf + 1); }
    inline numero recorta(const numero &d)
      {
        numero v = d%dist();
        if (v == 0) v= lim_sup;
        return v;
      }
  public:
    Ciclico(void)
      { valor = lim_inf; }
    Ciclico(const numero &d)
      { Put(d); }
    Ciclico(const Ciclico<numero,lim_inf,lim_sup> &c)
      { valor = c.valor; }
    Ciclico<numero,lim_inf,lim_sup> &operator =(const Ciclico<numero,lim_inf,lim_sup> &c)
      {
        valor= c.valor;
        return *this;
      }
    Ciclico<numero,lim_inf,lim_sup> &operator =(const numero &n)
      {
        Put(n);
        return *this;
      }
    inline void Put(const numero &d)
      { valor = recorta(d); }
    inline numero Get(void) const
      { return valor; }
    Ciclico<numero,lim_inf,lim_sup> Primero(void) const
      {
        Ciclico<numero,lim_inf,lim_sup> c(lim_inf);
        return c;
      }
    Ciclico<numero,lim_inf,lim_sup> Ultimo(void) const
      {
        Ciclico<numero,lim_inf,lim_sup> c(lim_sup);
        return c;
      }
    Ciclico<numero,lim_inf,lim_sup> &operator++(int)
      {
        if (valor < lim_sup)
          valor++;
        else
          valor= lim_inf;
        return *this;
      }
    Ciclico<numero,lim_inf,lim_sup> &operator--(int)
      {
        if (valor > lim_inf)
          valor--;
        else
          valor= lim_sup;
        return *this;
      }
    Ciclico<numero,lim_inf,lim_sup> &operator +=(const numero &n)
      {
        valor = (valor-lim_inf+n)%dist()+lim_inf;
        return *this;        
      }
    Ciclico<numero,lim_inf,lim_sup> &operator -=(const numero &n)
      {
        (*this)+= (lim_sup-(n%dist()));
        return *this;
      }
    friend Ciclico<numero,lim_inf,lim_sup> operator +(const Ciclico<numero,lim_inf,lim_sup> &a,const numero &n)
      {
        Ciclico<numero, lim_inf, lim_sup> cc(a);
        cc+= n;
        return cc;
      }
    friend Ciclico<numero,lim_inf,lim_sup> operator -(const Ciclico<numero,lim_inf,lim_sup> &a,const numero &n)
      {
        Ciclico<numero,lim_inf,lim_sup> cc(a);
        cc-= n;
        return cc;
      }
    friend int operator <(const Ciclico<numero,lim_inf,lim_sup> &a,const Ciclico<numero,lim_inf,lim_sup> &b)
      { return a.valor < b.valor; }
    friend int operator ==(const Ciclico<numero,lim_inf,lim_sup> &a,const Ciclico<numero, lim_inf, lim_sup> &b)
      { return a.valor == b.valor; }
    friend int operator !=(const Ciclico<numero,lim_inf,lim_sup> &a,const Ciclico<numero, lim_inf, lim_sup> &b)
      { return a.valor != b.valor; }
    friend int operator ==(const Ciclico<numero,lim_inf,lim_sup> &a,const numero &b)
      { return a.valor == b; }
    friend int operator !=(const Ciclico<numero,lim_inf,lim_sup> &a,const numero &b)
      { return a.valor != b; }
    friend int operator <(const Ciclico<numero,lim_inf,lim_sup> &a,const numero &b)
      { return a.valor < b; }
    friend int operator ==(const numero &b,const Ciclico<numero,lim_inf,lim_sup> &a)
      { return a.valor == b; }
    friend int operator !=(const numero &b,const Ciclico<numero,lim_inf,lim_sup> &a)
      { return a.valor != b; }
    friend int operator <(const numero &b,const Ciclico<numero,lim_inf,lim_sup> &a)
      { return a.valor < b; }
  };

#endif
