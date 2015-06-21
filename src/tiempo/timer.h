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
/* -------------------------------------------------------------

   Timer.hpp

   Implementation of a Timer class.

   (P)1999-2000  Laurentiu Cristofor (laur72_98@yahoo.com)

   -------------------------------------------------------------*/

#ifndef TIMER_H
#define TIMER_H

#include <cstdio>
#include <string>
#include <sys/types.h>
#include <ctime>
#include <sys/resource.h>
#include <iostream>


// This class is a container for time results
class TimeElapsed
  {
  private:
    long userTime, systemTime, realTime;
    bool precision;
    static const std::string str_sec;
    static const std::string str_msec;
  public:
    TimeElapsed(long user=0, long system=0, long real=0,const bool &prec= false)
      :userTime(user), systemTime(system), realTime(real), precision(prec) {}
    const long &getUserTime() const
      { return userTime; }
    const long &getSystemTime() const
      { return systemTime; }
    const long &getRealTime() const
      { return realTime; }
    const std::string &getUnidades(void) const;
    friend std::ostream &operator<<(std::ostream &os,const TimeElapsed &te);
  };


class Timer
  {
  private:
    time_t begin, end;
    struct rusage beginusage, endusage;
    long utime, stime, rtime; //!< the time counted so far
    bool started; //!< indicate whether the timer has been started

    // if true then return number of microseconds
    // for user and system time;
    // else return number of seconds.
    bool precision;
  public:
    Timer(const bool &prec= false);

    void start(void);
    TimeElapsed stop(void);
    void reset(void);

    TimeElapsed getTime(void);

    bool isStarted(void) const
      { return started; }
    bool getPrecision(void) const
      { return precision; }
  };

#endif // TIMER_H
