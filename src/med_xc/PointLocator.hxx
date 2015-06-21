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

#ifndef _POINT_LOCATOR_HXX_
#define _POINT_LOCATOR_HXX_

#include "MEDMEM.hxx"

#include <list>
#include <vector>

namespace INTERP_KERNEL
{
  class GenericMesh;
  class GenericPointLocatorAlgos;
}

namespace MEDMEM
{
  class MESH;

  class MEDMEM_EXPORT PointLocator
  {
  public:
    PointLocator(const MEDMEM::MESH& mesh);
    virtual ~PointLocator();
    std::list<int> locate(const double* x);
  protected:
    PointLocator() {}
    INTERP_KERNEL::GenericMesh *_medmesh;
    INTERP_KERNEL::GenericPointLocatorAlgos* _point_locator;
  };

  class MEDMEM_EXPORT PointLocatorInSimplex : public PointLocator
  {
  public:
    PointLocatorInSimplex(const MEDMEM::MESH& mesh);

    static void getNodeWightsInSimplex( const std::vector<const double*>& nodes,
                                        const double* p, double* weghts );
  };
}

#endif
