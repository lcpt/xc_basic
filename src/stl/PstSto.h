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
//PstSto.h

#ifndef PSTSTO_H
#define PSTSTO_H

#include "persist.h"
#include <iostream>

class PstSto
  {
  protected:
    const ClassInfo &ci;
  public:
    PstSto(void): ci(class_info()) {}
    inline virtual ~PstSto(void) {}
    virtual std::string GetClassId(void) const= 0;
  };

template<class INP>
class PstStore : public PstSto
  {
  protected:
    INP &is;
  public:
    PstStore(INP &istrm): is(istrm)
      {}
    virtual std::string GetClassId(void) const
      {
	std::string str_class;
        if(is) is >> str_class;
        return str_class;
      }
    void *Read(void)
    //Lee un objeto del stream y devuelve el objeto y
    //el ClassRecord correspondiente en cr.
      {
        std::string str_class= GetClassId();
        void *buf= NULL;
        const ClassRecord *cr= ci.GetClassRecord(str_class);
        if(cr)
          {
            buf= cr->NewPtr();
            cr->Read(buf);
          }
        return buf;
      }
    void Write(const std::type_info &tir,const void *buf)
      {
        const ClassRecord *cr= ci.GetClassRecord(tir);
        if(cr) cr->Write(buf);
      }
  };



#endif
