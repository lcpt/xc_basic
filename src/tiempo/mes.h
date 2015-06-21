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
//Mes.h


#ifndef MES_H
#define MES_H

#include "ciclico.h"
#include <iostream>
#include <string>

const double MEDIA_MES= 365.0/12.0;
const double MEDIA_MES_BISIESTO= 366.0/12.0;

typedef Ciclico<unsigned short int,1,12> c_mes;

class Mes : public c_mes
  {
  //El mes 1 es enero.
    static unsigned short int mes_aprox(const unsigned long &dias,const int &bisiesto);
  public:
    Mes(void): c_mes() {}
    Mes(const unsigned short int &m): c_mes(m) {}
    Mes(const c_mes &c): c_mes(c){} 
    Mes(const Mes &m): c_mes(m) {}
    Mes(const unsigned long &dias,const int &bisiesto);
    Mes &operator=(const unsigned short int &m)
      {
        Put(m);
        return *this;
      }
    Mes &operator=(const Mes &m)
      {
        c_mes::operator=(m);
        return *this;
      }
    const char *GetNombre(void) const;
    std::string EnNumero(void) const;
    unsigned int DiasAnteriores(int bisiesto) const;
    unsigned short int Dias(int bisiesto) const;
    friend std::ostream &operator <<(std::ostream &stream,const Mes &m)
      {
        stream << m.GetNombre();
        return stream;
      }
  };

#endif









































