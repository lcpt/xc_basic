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
//Normaliza.h

#ifndef NORMALIZA_H
#define NORMALIZA_H


#include "matrizZ.h"

template<class treal>
inline void Normaliza(matrizZ<treal> &m)
//Divide las componentes de la matriz por su modulo.
  {
    const treal vabs= Abs(m);
    if(vabs>0) m*= 1.0/vabs;
  }

template<class treal>
inline matrizZ<treal> Normaliza(const matrizZ<treal> &m)
//Divide las componentes de la matriz por su modulo.
  {
    matrizZ<treal> retval(m);
    Normaliza(retval);
    return retval;
  }

#endif
