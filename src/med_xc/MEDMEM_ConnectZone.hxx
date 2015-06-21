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

// File: MEDMEM_ConnectZone.hxx  Author: Nadir Bouhamou (CEA Saclay)
//
# ifndef __CONNECTZONE_HXX__
# define __CONNECTZONE_HXX__

#include "MEDMEM.hxx"

// few STL include files
#include <map>

// few Med Memory include files
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_SkyLineArray.hxx"

namespace MEDMEM {
class MEDMEM_EXPORT CONNECTZONE
{
private :
  string _name;
  string _description;
  int _localDomainNumber;
  int _distantDomainNumber;

  MEDMEM::MESH * _localMesh;
  MEDMEM::MESH * _distantMesh;

  MEDMEM::MEDSKYLINEARRAY * _nodeCorresp;
  MEDMEM::MEDSKYLINEARRAY * _faceCorresp;
  
  map < pair <MED_EN::medEntityMesh, MED_EN::medEntityMesh>,
                                     MEDMEM::MEDSKYLINEARRAY * > 
  _entityCorresp;

public :
  CONNECTZONE();
  ~CONNECTZONE();
  CONNECTZONE(const CONNECTZONE & myConnectZone);

  string getName() const ;
  string getDescription() const ;
  int getDistantDomainNumber() const ;
  int getLocalDomainNumber() const ;
  MEDMEM::MESH * getLocalMesh() const ;
  MEDMEM::MESH * getDistantMesh() const ;

  bool isEntityCorrespPresent(MED_EN::medEntityMesh localEntity,
                              MED_EN::medEntityMesh distantEntity) const;
  const int * getNodeCorrespIndex() const;
  const int * getNodeCorrespValue() const;
  int getNodeNumber() const;
  const int * getFaceCorrespIndex() const;
  const int * getFaceCorrespValue() const;
  int getFaceNumber() const;
  const int * getEntityCorrespIndex(MED_EN::medEntityMesh localEntity,
                                    MED_EN::medEntityMesh distantEntity) const;
  const int * getEntityCorrespValue(MED_EN::medEntityMesh localEntity,
                                    MED_EN::medEntityMesh distantEntity) const;
  int getEntityCorrespNumber(MED_EN::medEntityMesh localEntity,
                  MED_EN::medEntityMesh distantEntity) const;
  int getEntityCorrespLength(MED_EN::medEntityMesh localEntity,
                  MED_EN::medEntityMesh distantEntity) const;
  void setName(string name) ;
  void setDescription(string description) ;
  void setDistantDomainNumber(int distantDomainNumber) ;
  void setLocalDomainNumber(int distantDomainNumber) ;
  void setLocalMesh(MEDMEM::MESH * localMesh) ;
  void setDistantMesh(MEDMEM::MESH * distantMesh) ;

  void setNodeCorresp(int * nodeCorresp, int nbnode);
  void setNodeCorresp(MEDMEM::MEDSKYLINEARRAY* array);
  void setFaceCorresp(int * faceCorresp, int nbface);
  void setFaceCorresp(MEDMEM::MEDSKYLINEARRAY* array);
  void setEntityCorresp(MED_EN::medEntityMesh localEntity,
                        MED_EN::medEntityMesh distantEntity,
                        int * entityCorresp, int nbentity);
  void setEntityCorresp(MED_EN::medEntityMesh localEntity,
                        MED_EN::medEntityMesh distantEntity,
                        MEDMEM::MEDSKYLINEARRAY* array);
};
}
# endif
