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

#include "MEDNormalizedUnstructuredMesh.txx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Exception.hxx"
#include "PointLocatorAlgos.txx"
#include "PointLocator.hxx"

#include <list>

MEDMEM::PointLocator::PointLocator(const MEDMEM::MESH& mesh)
{
  int meshdim=mesh.getMeshDimension();
  int spacedim=mesh.getSpaceDimension();
  if (meshdim != spacedim) throw MEDMEM::MEDEXCEPTION("Locator is not implemented for meshdim != spacedim");
  switch (meshdim)
      {
      case 2:
        _medmesh = new MEDNormalizedUnstructuredMesh<2,2> (&mesh);
        _point_locator=new INTERP_KERNEL::PointLocatorAlgos<MEDNormalizedUnstructuredMesh<2,2> >(*(static_cast<MEDNormalizedUnstructuredMesh<2,2>* >(_medmesh)));
        break;
      case 3:
        _medmesh = new MEDNormalizedUnstructuredMesh<3,3> (&mesh);
        _point_locator=new INTERP_KERNEL::PointLocatorAlgos<MEDNormalizedUnstructuredMesh<3,3> >(*(static_cast<MEDNormalizedUnstructuredMesh<3,3>* >(_medmesh)));
        break;
      }
}

MEDMEM::PointLocator::~PointLocator()
{
  delete _medmesh;
  delete _point_locator;
}

std::list<int> MEDMEM::PointLocator::locate(const double* x)
{
  return _point_locator->locates(x,1e-12);
}

MEDMEM::PointLocatorInSimplex::PointLocatorInSimplex(const MEDMEM::MESH& mesh)
{
  int meshdim=mesh.getMeshDimension();
  int spacedim=mesh.getSpaceDimension();
  if (meshdim != spacedim) throw MEDMEM::MEDEXCEPTION("Locator is not implemented for meshdim != spacedim");
  switch (meshdim)
      {
      case 2:
        _medmesh = new MEDNormalizedUnstructuredMesh<2,2> (&mesh);
        _point_locator=new INTERP_KERNEL::PointLocatorInSimplex<MEDNormalizedUnstructuredMesh<2,2> >(*(static_cast<MEDNormalizedUnstructuredMesh<2,2>* >(_medmesh)));
        break;
      case 3:
        _medmesh = new MEDNormalizedUnstructuredMesh<3,3> (&mesh);
        _point_locator=new INTERP_KERNEL::PointLocatorInSimplex<MEDNormalizedUnstructuredMesh<3,3> >(*(static_cast<MEDNormalizedUnstructuredMesh<3,3>* >(_medmesh)));
        break;
      }
}

void MEDMEM::PointLocatorInSimplex::getNodeWightsInSimplex( const std::vector<const double*>& nodes,
                                                            const double* p, double* weghts )
{
  INTERP_KERNEL::barycentric_coords( nodes, p, weghts );
}
