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
//Funcion.h

#ifndef FUNCION_H
#define FUNCION_H

#include <cassert>
#include <algorithm>
#include "NmbVars.h"

class Funcion
  {
  protected:
    NmbVars Vars; //Nombres de las variables independientes.
  public:
    Funcion(void) {}
    Funcion(const NmbVars &vars)
      { Vars= vars; }      
    Funcion(const char *vars)
      { Vars= PutVars(vars); }
    Funcion(const char var)
      { Vars= PutVars(var); }
    Funcion(const Funcion &otra) 
      { Vars= otra.Vars; }
    Funcion &operator=(const Funcion &otra)
      {
        Vars= otra.Vars;
        return *this;
      }
    short unsigned int GetDim(void) const
      { return Vars.size(); }
    inline char GetNmbVar(short unsigned int i) const
    //Las variables se numeran de 1 a n;
      { return Vars.at(i-1); }
    void PutNmbVar(short unsigned int i,char nmb)
      { Vars[i-1]= nmb; }
    void PutStr(const char *vars)
      { Vars= PutVars(vars); }
    void PutChar(const char var)
      { Vars= PutVars(var); }
    void CambioVar(short unsigned int j,const NmbVars &vars)
      { Vars= Vars.CambioVar(j,vars); }
    const NmbVars &GetVars(void) const
      { return Vars; }
    friend Funcion operator +(const Funcion &f1,const Funcion &f2)
      { 
        Funcion f(f1.Vars.Combina(f2.Vars));
        return f;
      }
    friend Funcion operator -(const Funcion &f1,const Funcion &f2)
      {
        Funcion f(f1.Vars.Combina(f2.Vars));
        return f;
      }
    friend Funcion operator *(const Funcion &f1,const Funcion &f2)
      {
        Funcion f(f1.Vars.Combina(f2.Vars));
        return f;
      }      
    inline friend bool operator==(const Funcion &f1,const Funcion &f2)
      {
        if (f1.Vars != f2.Vars) return 0;
        return 1;
      }
    inline friend bool operator!=(const Funcion &f1,const Funcion &f2)
      { return !(f1==f2); }
  };



#endif

