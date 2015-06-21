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
//Fecha.h

//Ver: Guia practica para programar calculadoras y computadoras de bolsillo.
//Autores: M. Appert B.-R. de Fraguier.
//Compañia Editorial Continental, S.A. de C.V. Mexico

#ifndef FECHA_H
#define FECHA_H


#include <iostream>
#include <string>

#define usint unsigned short int

class DiaSemana;
class Mes;
class Agno;

class Fecha
  {
  protected:
    //Numero de dias transcurridos desde el 1 de enero de 1583.
    //el dia de entrada en vigor del calendario gregoriano
    //fue el 15 de octubre de 1582.
    long ndias;
  public:
    Fecha(void)
      { ndias = 0; }
    Fecha(const long &dd)
      { ndias= dd; }
    Fecha(const usint &dd,const usint &mm,const int &aa)
      { Put(dd,mm,aa); }
    Fecha(const Fecha &otra)
      { ndias= otra.ndias; }
    DiaSemana GetDiaSemana(void) const;
    long GetDias(void)
      { return ndias; }
    Mes GetMes(void) const;
    long GetDiaMes(void) const;
    Agno GetAgno(void) const;
    int GetNumAgno(void) const;
    long GetDiaAgno(void) const;
    void Put(const usint &dd,const usint &mm,const int &aa);
    void Put(const time_t &t);
    void Put(struct tm *lt);
    void Put(struct timeval *tv);
    void Time(void);
    void LocalTime(void);
    Fecha &operator++(int)
      {
        ndias++;
        return *this;
      }
    Fecha &operator--(int)
      {
        ndias--;
        return *this;
      }
    friend long operator -(const Fecha &a,const Fecha &b)
      { return a.ndias - b.ndias; }
    Fecha &operator +=(const long &dd)
      {
        ndias+= dd;
        return *this;
      }
    Fecha &operator -=(const long &dd)
      {
        ndias-= dd;
        return *this;
      }
    friend Fecha operator+(const Fecha &a,const long &dd)
      { return Fecha(a.ndias+dd); }
    friend Fecha operator-(const Fecha &a,const long &dd)
      { return Fecha(a.ndias-dd); }
    friend int operator<(const Fecha &a,const Fecha &b)
      { return a.ndias < b.ndias; }
    friend int operator==(const Fecha &a,const Fecha &b)
      { return a.ndias == b.ndias; }
    friend int operator>=(const Fecha &a,const Fecha &b)
      { return a.ndias >= b.ndias; }
    friend int operator<=(const Fecha &a,const Fecha &b)
      { return a.ndias <= b.ndias; }
    std::string TextoCorto(void) const;
    friend std::ostream &operator<<(std::ostream &stream,const Fecha &m);
    friend std::istream &operator>>(std::istream &is,Fecha &m);
  };

Fecha atofecha(const char *str);
inline Fecha atofecha(const std::string &str)
  { return atofecha(str.c_str()); }

//std::istream &get_token(std::istream &is,token_pair<Fecha> pair);

Fecha Hoy(void);

#endif






















