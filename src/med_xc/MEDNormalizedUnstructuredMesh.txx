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
#ifndef __MEDNORMALIZEDUNSTRUCTUREDMESH_TXX__
#define __MEDNORMALIZEDUNSTRUCTUREDMESH_TXX__

#include "MEDNormalizedUnstructuredMesh.hxx"
#include "InterpKernelException.hxx"

#include "MEDMEM_Mesh.hxx"

template<int SPACEDIM,int MESHDIM>
MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::MEDNormalizedUnstructuredMesh(const MEDMEM::MESH *mesh):_meshInMedMode(mesh),
                                                                                                         _conn_for_interp(0),
                                                                                                         _own_conn_for_interp(false),
                                                                                                         _conn_index_for_interp(0),
                                                                                                         _own_conn_index_for_interp(false)
{
  prepare();
}

template<int SPACEDIM,int MESHDIM>
MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::~MEDNormalizedUnstructuredMesh()
{
  releaseTempArrays();
}

template<int SPACEDIM,int MESHDIM>
void MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getBoundingBox(double *boundingBox) const
{
  vector< vector<double> > ret=_meshInMedMode->getBoundingBox();
  //low left corner
  int i;
  for(i=0;i<SPACEDIM;i++)
    boundingBox[i]=ret[0][i];
  for(i=0;i<SPACEDIM;i++)
    boundingBox[i+SPACEDIM]=ret[1][i];
}

template<int SPACEDIM,int MESHDIM>
INTERP_KERNEL::NormalizedCellType MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getTypeOfElement(int eltId) const
{
  MED_EN::medGeometryElement type=_meshInMedMode->getElementType(MED_EN::MED_CELL,eltId);
  if(type==MED_EN::MED_POLYGON)
    return INTERP_KERNEL::NORM_POLYGON;
  if(type==MED_EN::MED_POLYHEDRA)
    return INTERP_KERNEL::NORM_POLYHED;
  return (INTERP_KERNEL::NormalizedCellType)(((unsigned long)type/100-2)*10+((unsigned long)type%100));
}

template<int SPACEDIM,int MESHDIM>
unsigned char MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getNumberOfNodesOfElement(int eltId) const
{
  if(_meshInMedMode->getElementType(MED_EN::MED_CELL,eltId)!=MED_EN::MED_POLYHEDRA)
    return (unsigned char) (_conn_index_for_interp[eltId]-_conn_index_for_interp[eltId-1]);
  else
    {
      return (unsigned char)(_conn_index_for_interp[eltId]-_conn_index_for_interp[eltId-1])-
        std::count(_conn_for_interp+_conn_index_for_interp[eltId-1]-1,_conn_for_interp+_conn_index_for_interp[eltId]-1,-1);
    }
}

template<int SPACEDIM,int MESHDIM>
unsigned long MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getNumberOfElements() const
{
  return _meshInMedMode->getNumberOfElements(MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
}

template<int SPACEDIM,int MESHDIM>
unsigned long MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getNumberOfNodes() const
{
  return _meshInMedMode->getNumberOfNodes();
}

template<int SPACEDIM,int MESHDIM>
const int *MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getConnectivityPtr() const
{
  return _conn_for_interp;
}

template<int SPACEDIM,int MESHDIM>
const double *MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getCoordinatesPtr() const
{
  return _meshInMedMode->getCoordinates(MED_EN::MED_FULL_INTERLACE);
}

template<int SPACEDIM,int MESHDIM>
const int *MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::getConnectivityIndexPtr() const
{
  return _conn_index_for_interp;
}

template<int SPACEDIM,int MESHDIM>
void MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::releaseTempArrays()
{
  if(_own_conn_for_interp)
    delete [] _conn_for_interp;
  if(_own_conn_index_for_interp)
    delete [] _conn_index_for_interp;
  _conn_for_interp=0;
  _conn_index_for_interp=0;
  _own_conn_for_interp=false;
  _own_conn_index_for_interp=false;
}

template<int SPACEDIM,int MESHDIM>
void MEDNormalizedUnstructuredMesh<SPACEDIM,MESHDIM>::prepare()
{
  releaseTempArrays();
  _conn_for_interp=(int *)_meshInMedMode->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  _own_conn_for_interp=false;
  _conn_index_for_interp=(int *)_meshInMedMode->getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_CELL);
  _own_conn_index_for_interp=false;
}

#endif
