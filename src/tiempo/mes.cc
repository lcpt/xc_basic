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
//mes.cc

#include "mes.h"
#include "agno.h"
#include "xc_basic/src/texto/en_letra.h"


Mes::Mes(const unsigned long &dias,const int &bisiesto)
  {
    unsigned short int mm= mes_aprox(dias,bisiesto);
    if(mm<12) mm++; //mm= 12 si el día es 31 de diciembre.
    Put(mm);
    unsigned int dd= DiasAnteriores(bisiesto);
    if(dias <= dd) (*this)--;
    if(dias > (dd + Dias(bisiesto))) (*this)++;
  }
unsigned short int Mes::mes_aprox(const unsigned long &dias,const int &bisiesto)
  {
    if(bisiesto)
      return INT(dias/MEDIA_MES_BISIESTO);
    else
      return INT(dias/MEDIA_MES);
  }
const char *Mes::GetNombre(void) const
  {
    switch (Get())
      {
        case 1:
          return "Enero";
        case 2:
          return "Febrero";
        case 3:
          return "Marzo";
        case 4:
          return "Abril";
        case 5:
          return "Mayo";
        case 6:
          return "Junio";
        case 7:
          return "Julio";
        case 8:
          return "Agosto";
        case 9:
          return "Septiembre";
        case 10:
          return "Octubre";
        case 11:
          return "Noviembre";
        case 12:
          return "Diciembre";
        default:
          return "";
     };
  }
std::string Mes::EnNumero(void) const
  { return num2str(Get(),0); }
unsigned int Mes::DiasAnteriores(int bisiesto) const 
//Devuelve el numero de dias transcurridos en los meses anteriores
  {
    unsigned int dias= 0;
    if(Get() > 1) dias= dias + 31;
    if(Get() > 2)
      { 
        if (bisiesto)
          dias= dias + 29;
        else
          dias= dias + 28;
      }
    if(Get() > 3)
      dias= dias + INT(30.6 * Get() - 91.4);
    return dias;
  }
unsigned short int Mes::Dias(int bisiesto) const
//Devuelve el numero de dias del mes
  {
    if (Get() == Ultimo())
      return 31;
    else
      {
        Mes m(*this);
        m++;
        return m.DiasAnteriores(bisiesto)-this->DiasAnteriores(bisiesto);
      }
  }

