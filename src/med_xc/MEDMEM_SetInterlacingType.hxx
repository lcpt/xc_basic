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

#ifndef MEDMEM_SET_INTERLACING_TYPE
#define MEDMEM_SET_INTERLACING_TYPE

#include "MEDMEM_Tags.hxx"
using namespace MEDMEM;

namespace {

  template < typename T > struct SET_INTERLACING_TYPE {
    static const MED_EN::medModeSwitch _interlacingType = MED_EN::MED_UNDEFINED_INTERLACE; };
  template < > struct SET_INTERLACING_TYPE<FullInterlace>{
    static const MED_EN::medModeSwitch _interlacingType = MED_EN::MED_FULL_INTERLACE; };
  template < > struct SET_INTERLACING_TYPE<NoInterlace> {
    static const MED_EN::medModeSwitch _interlacingType = MED_EN::MED_NO_INTERLACE; };
  template < > struct SET_INTERLACING_TYPE<NoInterlaceByType> {
    static const MED_EN::medModeSwitch _interlacingType = MED_EN::MED_NO_INTERLACE_BY_TYPE; };

}
#endif
