// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

/*
 File Unit.hxx
 $Header: /home/server/cvs/MED/MED_SRC/src/MEDMEM/MEDMEM_Unit.hxx,v 1.11.20.2.10.1 2012-04-13 08:48:31 vsr Exp $
*/

#ifndef UNIT_HXX
#define UNIT_HXX

#include "MEDMEM.hxx"

#include <string>

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

namespace MEDMEM {
class MEDMEM_EXPORT UNIT { 
protected:
  string _name ;
  string _description ;

  int    _masse ; // kilogram (kg)
  int    _length ; // meter (m)
  int    _time ; // second (s)
  int    _temperature ; // Kelvin (K)
  int    _matterQuantity ; // mole (mol)
  int    _currentStrength ; // ampere (A)
  int    _lightIntensity ; // candela (cd)

  // Unit in IS !
  // Example : 
  //   J = kg.m^2.s^(-2) 
  //   so : Name="Joule",Masse=1,Length=2 and Time=-2, other are null 

public:
  UNIT();
  UNIT(string Name, string Description);
  ~UNIT();
  UNIT & operator=(const UNIT &unit);

  inline void setName(string Name) ;
  inline void setDescription(string Description) ;
  inline void setMasse(int Masse) ;
  inline void setLength(int Length) ;
  inline void setTime(int Time) ;
  inline void setTemperature(int Temperature) ;
  inline void setMatterQuantity(int MatterQuantity) ;
  inline void setCurrentStrength(int CurrentStrength) ;
  inline void setLightIntensity(int LightIntensity) ;

  inline string getName() const ;
  inline string getDescription() const ;
  inline int    getMasse() const ;
  inline int    getLength() const ;
  inline int    getTime() const ;
  inline int    getTemperature() const ;
  inline int    getMatterQuantity() const ;
  inline int    getCurrentStrength() const ;
  inline int    getLightIntensity() const ;

} ;

// inline method :

inline void UNIT::setName(string Name) {
  _name = Name ;
}
inline void UNIT::setDescription(string Description) {
  _description = Description ;
}
inline void UNIT::setMasse(int Masse) {
  _masse=Masse ;
}
inline void UNIT::setLength(int Length) {
  _length=Length ;
}
inline void UNIT::setTime(int Time) {
  _time=Time ;
}
inline void UNIT::setTemperature(int Temperature) {
  _temperature=Temperature ;
}
inline void UNIT::setMatterQuantity(int MatterQuantity) {
  _matterQuantity=MatterQuantity ;
}
inline void UNIT::setCurrentStrength(int CurrentStrength) {
  _currentStrength=CurrentStrength ;
}
inline void UNIT::setLightIntensity(int LightIntensity) {
  _lightIntensity=LightIntensity ;
}

inline string UNIT::getName() const            { return _name ; }
inline string UNIT::getDescription() const     { return _description ; }
inline int    UNIT::getMasse() const           { return _masse ; }
inline int    UNIT::getLength() const          { return _length ; }
inline int    UNIT::getTime() const            { return _time ; }
inline int    UNIT::getTemperature() const     { return _temperature ; }
inline int    UNIT::getMatterQuantity() const  { return _matterQuantity ; }
inline int    UNIT::getCurrentStrength() const { return _currentStrength ; }
inline int    UNIT::getLightIntensity() const  { return _lightIntensity ; }
}//End namespace MEDMEM

#endif /* UNIT_HXX */
