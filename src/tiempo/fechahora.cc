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
//fechahora.cc

#include "fechahora.h"
#include "agno.h"
#include "mes.h"
#include "diasemana.h"
#include <sys/time.h>

void FechaHora::LocalTime(void)
//Hora local.
  {
    time_t ss= time(NULL);
    struct tm *loctime;
    loctime = localtime (&ss);
    Put(loctime);
  }
void FechaHora::TimeOfDay(void)
  {
    struct timeval TP;
    gettimeofday(&TP,NULL);
    Put(&TP);
  }
void FechaHora::LocalTimeOfDay(void)
  {
    struct timeval TP;
    gettimeofday(&TP,NULL);
    struct tm *lt;
    lt = localtime ((time_t *) &TP.tv_sec);
    f.Put(lt);
    h.Put(lt->tm_hour,lt->tm_min,lt->tm_sec,TP.tv_usec);
  }
DiaSemana FechaHora::GetDiaSemana(void) const
  { return f.GetDiaSemana(); }
Mes FechaHora::GetMes(void) const
  { return f.GetMes(); }
Agno FechaHora::GetAgno(void) const
  { return f.GetAgno(); }
