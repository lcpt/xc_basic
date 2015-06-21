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
//hora.cc

#include "hora.h"
#include "xc_basic/src/util/matem.h"
#include <sys/time.h>

long Hora::QuitaDias(void)
  {
    long dias= INT(GetDias());
    segs-= dias*24*3600;
    return dias;
  }
void Hora::Get( unsigned long &dd,
          unsigned short int &hh,
          unsigned short int &mm,
          double &ss) const
  {
     dd= INT(GetDias());
     hh= INT(GetHoras()) - dd * 24;
     mm= INT(GetMinutos()) - hh * 60 - dd * 60 * 24;
     ss= GetSegundos() - mm * 60 - hh * 3600 - dd * 3600 * 24;
  }
void Hora::LocalTime(void)
//Hora local.
  {
    time_t ss= time(NULL);
    struct tm *loctime;
    loctime = localtime (&ss);
    Put(loctime);
  }
void Hora::TimeOfDay(void)
  {
    struct timeval TP;
    gettimeofday(&TP,NULL);
    Put(&TP);
  }
void Hora::LocalTimeOfDay(void)
  {
    struct timeval TP;
    gettimeofday(&TP,NULL);
    struct tm *lt;
    lt = localtime ((time_t *) &TP.tv_sec);
    Put(lt->tm_hour,lt->tm_min,lt->tm_sec,TP.tv_usec);
  }

void Hora::Put(const time_t &t)
  { segs= t; }
void Hora::Put(struct tm *lt)
  { Put(lt->tm_hour,lt->tm_min,lt->tm_sec); }
void Hora::Put(struct timeval *tv)
  { segs= tv->tv_sec + tv->tv_usec * 1.0E-6; }

std::ostream &operator <<(std::ostream &stream,const Hora &m)
  {
    unsigned long dd;
    unsigned short int hh;
    unsigned short int mm;
    double ss;
    m.Get(dd,hh,mm,ss);
    stream << hh << ":"
           << mm << ":"
           << ss;
    return stream;
   }
