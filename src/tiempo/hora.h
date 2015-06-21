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
//Hora.h

#ifndef HORA_H
#define HORA_H

#include <iostream>

class Hora
  {
  protected:
    //Numero de segundos transcurridos desde las 0 horas.
    double segs;
  public:
    Hora(void)
      { segs = 0; }
    Hora(const double &ss)
      { segs= ss; }
    Hora( const unsigned short int &hh,
          const unsigned short int &mm,
          const double &ss)
      { Put(hh,mm,ss); }
    Hora(const Hora &otra)
      { segs= otra.segs; }
    double GetDias(void) const
      { return segs/(24*3600); }
    double GetHoras(void) const
      { return segs/3600; }
    double GetMinutos(void) const
      { return segs/60; }
    double GetSegundos(void) const
      { return segs; }
    int GetAM(void) const
      { return (segs < (12*3600)); }
    long QuitaDias(void);
    inline void Put( const unsigned short int &hh,
                     const unsigned short int &mm,
                     const double &ss)
      { segs = hh*3600 + mm * 60 + ss; }
    inline void Put( const unsigned short int &hh,
                     const unsigned short int &mm,
                     const unsigned short int &ss,
                     const long int &us)
      { segs = hh*3600 + mm * 60 + ss + us * 1E-6; }
    void Get( unsigned long &dd,
              unsigned short int &hh,
              unsigned short int &mm,
              double &ss) const;
    void Put(const time_t &t);
    void Put(struct tm *lt);
    void Put(struct timeval *tv);
    void Time(void)
    //Hora GMT.
      { Put(time(NULL)); }
    void LocalTime(void); //Hora local.
    void TimeOfDay(void);
    void LocalTimeOfDay(void);
    Hora &operator +=(const double &ss)
      {
        segs+= ss;
        return *this;
      }
    Hora &operator -=(const double &ss)
      {
        segs-= ss;
        return *this;
      }
    Hora &operator *=(const double &d)
      {
        segs*= d;
        return *this;
      }
    Hora &operator /=(const double &d)
      {
        segs/= d;
        return *this;
      }
    Hora &operator +=(const Hora &h)
      { return Hora::operator+=(h.segs); }
    Hora &operator -=(const Hora &h)
      { return Hora::operator-=(h.segs); }
    friend Hora operator -(const Hora &a,const Hora &b)
      { return Hora(a.segs - b.segs); }
    friend Hora operator +(const Hora &a,const Hora &b)
      { return Hora(a.segs + b.segs); }
    friend Hora operator +(const Hora &a,const double &ss)
      { return Hora(a.segs+ss); }
    friend Hora operator -(const Hora &a,const double &ss)
      { return Hora(a.segs-ss); }
    friend Hora operator *(const Hora &a,const double &d)
      { return Hora(a.segs*d); }
    friend Hora operator /(const Hora &a,const double &d)
      { return Hora(a.segs/d); }
    friend Hora operator *(const double &d,const Hora &a)
      { return Hora(a.segs*d); }
    friend int operator <(const Hora &a,const Hora &b)
      { return a.segs < b.segs; }
    friend int operator ==(const Hora &a,const Hora &b)
      { return a.segs == b.segs; }
    friend std::ostream &operator <<(std::ostream &stream,const Hora &m);
  };

#endif









































