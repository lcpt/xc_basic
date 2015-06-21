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

#ifndef MEDMEM_INDEX_CHECKING_POLICY_HXX
#define MEDMEM_INDEX_CHECKING_POLICY_HXX 

#include "MEDMEM.hxx"

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_STRING.hxx"

namespace MEDMEM {

class MEDMEM_EXPORT IndexCheckPolicy {
public :
  inline void checkMoreThanZero(const std::string & classname, int index) const {
    if (index <= 0)
      throw MEDEXCEPTION(LOCALIZED(STRING("In ") << classname << ", index : " << index << " is less or equal to zero"));
  }
  inline void checkLessOrEqualThan(const std::string & classname, int max, int index) const {
    if (index > max)
      throw MEDEXCEPTION(LOCALIZED(STRING("In ") << classname << ", index : " << index << " is more than " << max ));
  }

  inline void checkInInclusiveRange(const std::string & classname, int min, int max, int index) const {
    if (index < min || index > max )
      throw MEDEXCEPTION(LOCALIZED(STRING("In ") << classname << ", index : " << index 
                                           << " not in rang [" << min << "," << max <<"]"));
  }

  inline void checkEquality(const std::string & classname, int a, int b) const {
    if ( a == b )
      throw MEDEXCEPTION(LOCALIZED(STRING("In ") << classname << ", Value shouldn't be : " << a ));
  }

};

class MEDMEM_EXPORT NoIndexCheckPolicy {
public :
  inline void checkMoreThanZero(const string &classname, int index) const {}
  inline void checkLessOrEqualThan(const std::string & classname, int max, int index) const {}
  inline void checkInInclusiveRange(const std::string & classname, int min, int max, int index) const {}
  inline void checkEquality(const std::string & classname, int a, int b) const {}
};

}// END NAMESPACE
#endif
