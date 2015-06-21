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

#ifndef __MEDMEM_RCBASE_HXX__
#define __MEDMEM_RCBASE_HXX__

#include "MEDMEM.hxx"

namespace MEDMEM {
  
  class MEDMEM_EXPORT RCBASE {
  public:
    RCBASE();
    void addReference() const;
    bool removeReference() const;
  protected:
    void clearRefCouner();
    virtual ~RCBASE();
  private:
    mutable int _cnt;
  };

  /*!
   * \brief Class calling RCBASE::removeReference() at distruction.
   * Useful to avoid memory leaks in case of exceptions;
   * helps not to forget to delete anything, etc.
   */
  class MEDMEM_EXPORT AutoDeref
  {
  public:
    AutoDeref(const RCBASE* obj);
    ~AutoDeref();
  private:
    const RCBASE* _obj;
    AutoDeref(const AutoDeref&);
    AutoDeref& operator=(const AutoDeref&);
  };    

}

#endif
