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
//Agno.h

//Ver: Guia practica para programar calculadoras y computadoras de bolsillo.
//Autores: M. Appert B.-R. de Fraguier.
//Compagnia Editorial Continental, S.A. de C.V. Mexico

#ifndef AGNO_H
#define AGNO_H

#include <iostream>

const double NUM_DIAS_AGNO_JULIANO= 365.25;
const double NUM_DIAS_AGNO_GREG0RIANO= 365.2425;
const double NUM_DIAS_AGNO_TROPICO= 365.242198781;
const int ORIGEN= 1583;

class Agno
  {
  protected:
    int a;
  protected:
    int multiplos(const int &n,const int &f) const;
    //Devuelve el numero de agnos multiplos de n
    //desde org hasta el agno f.
    int bisiesto(const int &f) const;
    int bisiestos(const int f) const;
    //Devuelve el nUmero de agnos bisiestos
    //hasta el agno "f".
    long num_dias_ant(const int f) const;
    //Devuelve el numero de dias desde el 1 de enero de 1584
    //hasta el 1 de enero del agno f.
    long num_dias(const int f) const;
    //Devuelve el numero de dias del agno f.
  public:
    Agno(void)
      : a(ORIGEN) {}
    Agno(const int &agno)
      : a(agno) {}
    Agno(const Agno &otro)
      : a(otro.a) {}
    Agno(const unsigned long &dias);
    Agno &operator =(const int &agno)
      {
        a= agno;
        return *this;
      }
    Agno &operator =(const Agno &agno)
      {
        a= agno.a;
        return *this;
      }
    //! @brief Devuelve el número del año.
    const int &GetNumero(void) const
      { return a; }
    //! @brief Devuelve el numero de agnos multiplos de n
    //! desde org hasta el agno anterior a "a".
    int GetNumMultiplos(const int &n,const int &) const
      { return multiplos(n,a-1); }
    //! @brief Devuelve el nUmero de agnos bisiestos
    //! hasta el agno anterior a "a".
    int GetNumBisiestos(void) const
      { return bisiestos(a-1); }
    //! @brief Son bisisestos los agnos múltiplos de 4 que no sean múltiplos
    //! de 100 salvo que lo sean de 400.
    int Bisiesto(void) const
      { return bisiesto(a); }
    //! @brief Devuelve el numero de dias desde el 1 de enero de 1584
    //!hasta el 1 de enero de este agno.
    long DiasAnteriores(void) const
      { return num_dias_ant(a); }
    inline Agno &operator++(int)
      {
        a++;
        return *this;
      }
    inline Agno &operator--(int)
      {
        a--;
        return *this;
      }
    inline Agno &operator +=(const int &n)
      {
        a+= n;
        return *this;
      }
    inline Agno &operator -=(const int &n)
      {
        a-= n;
        return *this;
      }
    inline friend Agno operator +(const Agno &a,const int &n)
      {
        Agno cc(a);
        cc+= n;
        return cc;
      }
    friend Agno operator -(const Agno &a,const int &n)
      {
        Agno cc(a-n);
        cc-= n;
        return cc;
      }
    friend int operator <(const Agno &a,const Agno &b)
      { return a.a < b.a; }
    friend int operator ==(const Agno &a,const Agno &b)
      { return a.a == b.a; }
    friend int operator !=(const Agno &a,const Agno &b)
      { return a.a != b.a; }
    friend int operator <(const Agno &a,const int &b)
      { return a.a < b; }
    friend int operator <=(const Agno &a,const int &b)
      { return a.a <= b; }
    friend int operator ==(const Agno &a,const int &b)
      { return a.a == b; }
    friend int operator !=(const Agno &a,const int &b)
      { return a.a != b; }
    friend int operator <(const int &a,const Agno &b)
      { return a < b.a; }
    friend int operator ==(const int &a,const Agno &b)
      { return a == b.a; }
    friend int operator !=(const int &a,const Agno &b)
      { return a != b.a; }
    friend std::ostream &operator <<(std::ostream &stream,const Agno &a)
      {
        stream << a.a;
        return stream;
      }
  };


#endif









































