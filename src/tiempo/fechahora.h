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
//FechaHora.h


#ifndef FECHAHORA_H
#define FECHAHORA_H

#include "fecha.h"
#include "hora.h"
//#include "time.h"

class FechaHora
  {
  protected:
    Fecha f;
    Hora h;
  public:
    FechaHora(void) : f(),h() {}
    FechaHora(const long &dd, const double &ss) :f(dd+h.QuitaDias()), h(ss) {}
    FechaHora(const Fecha &ff, const Hora &hh) : f(ff+h.QuitaDias()), h(hh) {}
    FechaHora( const usint &dd,
               const usint &mm,
               const int &aa,
               const unsigned short int &hh,
               const unsigned short int &mmm,
               const double &ss ) : f(dd,mm,aa),h(hh,mmm,ss)
      { f+= h.QuitaDias(); }
      FechaHora(const FechaHora &otra) : f(otra.f),h(otra.h) {}
    void Put(const time_t &t)
      { h.Put(t); f.Put(t); }
    void Put(struct tm *lt)
      { h.Put(lt); f.Put(lt); }
    void Put(struct timeval *tv)
      { h.Put(tv); f.Put(tv); }
    void Time(void)
    //Hora GMT.
      { Put(time(NULL)); }
    void LocalTime(void);
    void TimeOfDay(void);
    void LocalTimeOfDay(void);
    DiaSemana GetDiaSemana(void) const;
    long GetDias(void)
      { return f.GetDias(); }
    Mes GetMes(void) const;
    long GetDiaMes(void) const
      { return f.GetDiaMes(); }
    Agno GetAgno(void) const;
    long GetDiaAgno(void) const
      { return f.GetDiaAgno(); }
    FechaHora &operator++(int)
      {
        f++;
        return *this;
      }
    FechaHora &operator--(int)
      {
        f--;
        return *this;
      }
    friend double operator -(const FechaHora &a,const FechaHora &b)
      {
        double d= (a.f-b.f)*24*3600;
        d+=(a.h-b.h).GetSegundos();
        return d;
      }
    FechaHora &operator +=(const long &dd)
      {
        f+= dd;
        return *this;
      }
    FechaHora &operator -=(const long &dd)
      {
        f-= dd;
        return *this;
      }
    friend FechaHora operator +(const FechaHora &a,const long &dd)
      {
        FechaHora fh(a);
        fh+= dd;
        return fh;
      }
    friend FechaHora operator -(const FechaHora &a,const long &dd)
      {
        FechaHora fh(a);
        fh-= dd;
        return fh;
      }
    friend int operator <(const FechaHora &a,const FechaHora &b)
      {
        if (a.f < b.f) return 1;
        if ( (a.f == b.f) && (a.h < b.h) ) return 1;
        return 0;
      }
    friend int operator ==(const FechaHora &a,const FechaHora &b)
      {
        if ( (a.f == b.f) && (a.h == b.h) ) return 1;
        return 0;
      }
    friend std::ostream &operator <<(std::ostream &stream,const FechaHora &m)
      {
        stream << m.f << " " << m.h;
        return stream;
      }
  };

#endif









































