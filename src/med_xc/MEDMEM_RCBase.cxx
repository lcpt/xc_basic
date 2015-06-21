// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "MEDMEM_RCBase.hxx"
#ifdef _DEBUG_
#include <iostream>
#endif

MEDMEM::RCBASE::RCBASE():_cnt(1)
{
}

MEDMEM::RCBASE::~RCBASE()
{
}
/*!
 * \brief To be called at destructor beginning to avoid recursive calls of destructor
 * in case of cyclic dependencies between reference counters like e.g. between
 * the GMESH and the GROUPs it contains
 */
void MEDMEM::RCBASE::clearRefCouner()
{
  _cnt = -100;
}

void MEDMEM::RCBASE::addReference() const
{
  _cnt++;
}

bool MEDMEM::RCBASE::removeReference() const
{
  bool ret=((--_cnt)==0);
  if(ret)
    delete this;
  return ret;
}

MEDMEM::AutoDeref::AutoDeref(const RCBASE* obj): _obj(obj)
{
}

MEDMEM::AutoDeref::~AutoDeref()
{
  if ( _obj )
  {
    _obj->removeReference();
    _obj = 0;
  }
}
