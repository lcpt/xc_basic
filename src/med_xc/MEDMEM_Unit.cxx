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
 File MEDMEM_Unit.cxx
 $Header: /home/server/cvs/MED/MED_SRC/src/MEDMEM/MEDMEM_Unit.cxx,v 1.10.20.3.10.1 2012-04-13 08:48:31 vsr Exp $
*/

#include "MEDMEM_Unit.hxx"

using namespace std;
using namespace MEDMEM;

UNIT::UNIT():_name(""),_description(""),_masse(0),_length(0),_time(0),_temperature(0),_matterQuantity(0),_currentStrength(0),_lightIntensity(0) {
  MESSAGE_MED("UNIT()");
}

UNIT::UNIT(string Name, string Description):_name(Name),_description(Description),_masse(0),_length(0),_time(0),_temperature(0),_matterQuantity(0),_currentStrength(0),_lightIntensity(0) {
  MESSAGE_MED("UNIT(string Name, string Description)");
}

UNIT::~UNIT() {
  MESSAGE_MED("~UNIT()") ;
}

UNIT & UNIT::operator=(const UNIT &unit) {
  MESSAGE_MED("UNIT::operateur=") ;
  _name=unit._name ;
  _description=unit._description ;
  _masse=unit._masse ; 
  _length=unit._length ; 
  _time=unit._time ; 
  _temperature=unit._temperature ; 
  _matterQuantity=unit._matterQuantity ; 
  _currentStrength=unit._currentStrength ; 
  _lightIntensity=unit._lightIntensity ; 
  return *this;
}
