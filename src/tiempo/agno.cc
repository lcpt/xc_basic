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
//agno.cc

#include "agno.h"
#include "xc_basic/src/util/matem.h"

Agno::Agno(const unsigned long &dias)
  {
    a= INT(dias/NUM_DIAS_AGNO_GREG0RIANO)+ORIGEN;
    unsigned long dd= num_dias_ant(a);
    if(dias <= dd) a--;
    if(dias > (dd + num_dias(a))) a++;
  }
int Agno::multiplos(const int &n,const int &f) const
//Devuelve el numero de agnos multiplos de n
//desde org hasta el agno f.
  { return num_multiplos(n,ORIGEN,f); }
int Agno::bisiesto(const int &f) const
//Son bisisestos los agnos multiplos de 4 que no sean multiplos
//de 100 salvo que lo sean de 400.
  {
    if (es_multiplo(f,4))
      if (es_multiplo(f,100))
        if (es_multiplo(f,400))
          return 1;
        else
          return 0;
      else
        return 1;
    else
      return 0;
  }
int Agno::bisiestos(const int f) const
//Devuelve el nUmero de agnos bisiestos
//hasta el agno "f".
  {
    int retval= multiplos(4,f);
    retval -= multiplos(100,f);
    retval += multiplos(400,f);
    return retval;
  }
long Agno::num_dias_ant(const int f) const
//Devuelve el numero de dias desde el 1 de enero de 1584
//hasta el 1 de enero del agno f.
  {
    if (f > ORIGEN)
      return (f-ORIGEN) * 365 + bisiestos(f-1);
    else
      return 0;
  }
long Agno::num_dias(const int f) const
//Devuelve el numero de dias del agno f.
  {
    if (bisiesto(f))
      return 366;
    else
      return 365;
  }
