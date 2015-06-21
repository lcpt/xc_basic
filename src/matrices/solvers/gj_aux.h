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
//gj_aux.h
//Funciones auxiliares para los  mEtodos
//de Gauss y de Jordan para la solucion de un sistema de ecuaciones lineal.

#ifndef GJ_AUX_H
#define GJ_AUX_H

#include "math.h"
const double zero = 1e-7;

//Las funciones siguientes deberAn estar definidas para cualquier objeto
//con el que se quieran emplear las rutinas de gauss.h y jordan.h

inline double gj_abs(const double &r)
  { return fabs(r); }
inline double gj_inv(const double &r)
  { return 1/r; }

#endif
