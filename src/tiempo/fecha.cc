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
//fecha.cc

#include "fecha.h"
#include "xc_basic/src/util/matem.h"
#include "diasemana.h"
#include "agno.h"
#include "mes.h"
#include "xc_basic/src/texto/en_letra.h"
#include "sys/time.h"
//#include "streams/imanip.h"



DiaSemana Fecha::GetDiaSemana(void) const
  {
    long dias= ndias + 5;
    long semanas= LONG_INT(dias/7);
    DiaSemana d(dias - 7 * semanas);
    return d;
  }
Mes Fecha::GetMes(void) const
  { return Mes(GetDiaAgno(),GetAgno().Bisiesto()); }
long Fecha::GetDiaMes(void) const
  { return (GetDiaAgno() - GetMes().DiasAnteriores(GetAgno().Bisiesto())); }

//! @brief Devuelve el año.
Agno Fecha::GetAgno(void) const
  { return Agno((const unsigned long &) ndias); }

//! @brief Devuelve el año.
int Fecha::GetNumAgno(void) const
  { return GetAgno().GetNumero(); }

long Fecha::GetDiaAgno(void) const
  { return (ndias - GetAgno().DiasAnteriores()); }
void Fecha::Put(const usint &dd,const usint &mm,const int &aa)
  {
    Mes mes(mm);
    Agno agno(aa);
    long dias_mes= mes.DiasAnteriores(agno.Bisiesto());
    long dias_agno= agno.DiasAnteriores();
    ndias= dd + dias_mes + dias_agno;
  }
void Fecha::Put(const time_t &t)
  {
    Put(1,1,1970);
    ndias+= LONG_INT(t/(24*3600));
  }
void Fecha::Put(struct tm *lt)
  { Put(lt->tm_mday,lt->tm_mon+1,lt->tm_year+1900); }
void Fecha::Put(struct timeval *tv)
  { Put( (const time_t &) tv->tv_sec); }
void Fecha::Time(void)
//Hora GMT.
  { Put(time(NULL)); }
void Fecha::LocalTime(void)
//Hora local.
  {
    time_t ss= time(NULL);
    struct tm *loctime;
    loctime = localtime (&ss);
    Put(loctime);
  }
std::string Fecha::TextoCorto(void) const
  {
    std::string retval(num2str(GetDiaMes(),0));
    retval+= "/" + GetMes().EnNumero();
    retval+= "/" + num2str(GetAgno(),0);
    return retval;
  }    
std::ostream &operator<<(std::ostream &stream,const Fecha &m)
  {
    stream << m.GetDiaSemana() << ", "
           << m.GetDiaMes() << " de "
           << m.GetMes() << " de "
           << m.GetAgno();
    return stream;
  }
std::istream &operator>>(std::istream &is,Fecha &m)
  {
    unsigned short int dd;
    unsigned short int mm;
    int aa;
    is >> dd >> mm >> aa;
    m.Put(dd,mm,aa);
    return is;
  }

Fecha atofecha(const char *str)
  {
    const char delimiters[] = " /-";
    size_t sz= strlen(str)+1;
    char *str2= (char *)malloc(sz);
    strcpy(str2,str);
    unsigned short int dd;
    unsigned short int mm;
    int aa;
    dd= atoi(strtok(str2,delimiters));
    mm= atoi(strtok(NULL,delimiters));
    aa= atoi(strtok(NULL,delimiters));
    free(str2);
    return Fecha(dd,mm,aa);
  }

// istream &get_token(istream &is,token_pair<Fecha> pair)
//   {
//     string str;
//     token_pair<string> pair_str(str,pair.delim);
//     get_token(is,pair_str);
//     pair.t= atofecha(pair_str.t);
//     return is;
//   }

Fecha Hoy(void)
  {
    Fecha f;
    f.Time();
    return f;
  }
