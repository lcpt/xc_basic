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
//DiaSemana.h

//Ver: Guia practica para programar calculadoras y computadoras de bolsillo.
//Autores: M. Appert B.-R. de Fraguier.
//Compagnia Editorial Continental, S.A. de C.V. Mexico

#ifndef DIASEMANA_H
#define DIASEMANA_H


#include "ciclico.h"
#include <iostream>

typedef Ciclico<unsigned short int,1,7> c_ds;

class DiaSemana: public c_ds
  {
  //El 1 es el Lunes y el 7 el Domingo
  public:
    DiaSemana(void): c_ds (1){}
    DiaSemana(const DiaSemana &d): c_ds(d){}
    DiaSemana(const Ciclico<short unsigned int,1,7> &c): c_ds(c){} 
    DiaSemana(const unsigned short int &d): c_ds (d){}
    const char *GetNombre(void) const;
    friend std::ostream &operator <<(std::ostream &stream,const DiaSemana &d)
      { return stream << d.GetNombre(); }
  };

#endif









































