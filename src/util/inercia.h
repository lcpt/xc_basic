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
//inercia.h

#ifndef INERCIA_H
#define INERCIA_H

#include <cmath>
#include "matem.h"

//! @brief Devuelve el ángulo que define un eje principal de inercia.
double theta_inercia(const double &Ix,const double &Iy,const double &Pxy);

//! @brief Funcion auxiliar para calcular los momentos principales de inercia.
inline double R_inercia(const double &Ix,const double &Iy,const double Pxy)
  { return sqrt(sqr((Ix-Iy)/2)+sqr(Pxy)); }

//! @brief Devuelve el momento de inercia principal mayor.
inline double I1_inercia(const double &iy,const double &iz,const double &pyz)
  { return (iy+iz)/2+R_inercia(iy,iz,pyz); }

//! @brief Devuelve el momento de inercia principal menor.
inline double I2_inercia(const double &iy,const double &iz,const double &pyz)
  { return (iy+iz)/2-R_inercia(iy,iz,pyz); }


#endif
