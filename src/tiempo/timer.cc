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
//timer.cc

#include "timer.h"

const std::string TimeElapsed::str_sec= "segundos";
const std::string TimeElapsed::str_msec= "microsegundos";

//! @brief devuelve la cadena de caracteres que identifica la unidad de tiempo.
const std::string &TimeElapsed::getUnidades(void) const
  {
    if(precision)
      return str_msec;
    else
      return str_sec;
  }

std::ostream &operator<<(std::ostream &os,const TimeElapsed &te)
  {
    os << "userTime: " << te.userTime << " sysTime: " << te.systemTime
       << " realTime: " << te.realTime << ' ' << te.getUnidades() << std::endl;
    return os;
  }

//! @brief if precision is set to true, then the real and system time
//! will be measured in microseconds
Timer::Timer(const bool &prec)
  : utime(0), stime(0), rtime(0), started(false), precision(prec)
  {}

//! @brief stop timer and return time measured so far.
//! if timer was stopped the time returned will be 0.
TimeElapsed Timer::stop(void)
  {
    if(!started)
      return TimeElapsed(0, 0, 0,precision);

    if(getrusage(RUSAGE_SELF, &endusage) == -1)
      puts("getrusage error!");
    end= time(NULL);
    started= false;

    if(precision)
      {
        long uusecs= (endusage.ru_utime.tv_sec 
                      - beginusage.ru_utime.tv_sec) * 1000000 
                      + endusage.ru_utime.tv_usec - beginusage.ru_utime.tv_usec;
        utime+= uusecs;

        long susecs= (endusage.ru_stime.tv_sec 
                      - beginusage.ru_stime.tv_sec) * 1000000 
                      + endusage.ru_stime.tv_usec - beginusage.ru_stime.tv_usec;
        stime+= susecs;
      }
    else
      {
        long usecs= (endusage.ru_utime.tv_sec 
                     - beginusage.ru_utime.tv_sec);
        utime+= usecs;
        long ssecs= (endusage.ru_stime.tv_sec 
                     - beginusage.ru_stime.tv_sec);
        stime+= ssecs;
      }

    rtime+= (end - begin);
    return TimeElapsed(utime, stime, rtime,precision);
  }

//! @brief return time measured up to this point.
TimeElapsed Timer::getTime(void)
  {
    if(!started)
      return TimeElapsed(utime, stime, rtime,precision);

    if(getrusage(RUSAGE_SELF, &endusage) == -1)
      puts("getrusage error!");
    end= time(NULL);

    if(precision)
      {
        long uusecs= (endusage.ru_utime.tv_sec 
                       - beginusage.ru_utime.tv_sec) * 1000000 
          + endusage.ru_utime.tv_usec - beginusage.ru_utime.tv_usec;

        long susecs= (endusage.ru_stime.tv_sec 
                       - beginusage.ru_stime.tv_sec) * 1000000 
          + endusage.ru_stime.tv_usec - beginusage.ru_stime.tv_usec;

        return TimeElapsed(utime + uusecs, 
                           stime + susecs, 
                           rtime + end - begin,precision);
      }
    else
      {
        long usecs= (endusage.ru_utime.tv_sec 
                      - beginusage.ru_utime.tv_sec);
        
        long ssecs= (endusage.ru_stime.tv_sec 
                      - beginusage.ru_stime.tv_sec);
        
        return TimeElapsed(utime + usecs, 
                           stime + ssecs, 
                           rtime + end - begin,precision);
      }
  }

//! @brief reset the timer, this will reset the time measured to 0 and
//! will leave the timer in the same status (started or stopped).
void Timer::reset(void)
  {
    utime= stime= rtime= 0;

    if (started)
      {
        begin= time(NULL);
        if (getrusage(RUSAGE_SELF, &beginusage) == -1)
          puts("getrusage error!");
      }
  }

//! @brief start timer, if already started then do nothing
void Timer::start(void)
  {
    if(started) return;

    started= true;
    begin= time(NULL);
    if(getrusage(RUSAGE_SELF, &beginusage) == -1)
      puts("getrusage error!");
  }
