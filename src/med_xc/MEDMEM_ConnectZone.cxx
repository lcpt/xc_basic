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

// few STL include files
//
#include <map>

// few Med Memory include files
#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_ConnectZone.hxx"

using namespace MEDMEM;

  CONNECTZONE::CONNECTZONE():
  _name("")
  ,_description("")
  ,_localDomainNumber(0)
  ,_distantDomainNumber(0)
  ,_nodeCorresp(0)
  ,_faceCorresp(0)
  {
        _entityCorresp.clear();
  }
  
  CONNECTZONE::~CONNECTZONE(){
  if (_nodeCorresp !=0) delete _nodeCorresp;
  if (_faceCorresp !=0) delete _faceCorresp;
  for (map < pair <MED_EN::medEntityMesh, MED_EN::medEntityMesh>,MEDMEM::MEDSKYLINEARRAY * >::iterator 
   iter = _entityCorresp.begin(); iter != _entityCorresp.end(); iter++)
   {
                delete iter->second;
   }
  }
  
  CONNECTZONE::CONNECTZONE(const CONNECTZONE & myConnectZone):
  _name(myConnectZone._name)
  ,_description(myConnectZone._description)
  ,_localDomainNumber(myConnectZone._localDomainNumber)
  ,_distantDomainNumber(myConnectZone._distantDomainNumber)
  ,_nodeCorresp(myConnectZone._nodeCorresp)
  ,_faceCorresp(myConnectZone._faceCorresp)
  ,_entityCorresp(myConnectZone._entityCorresp)
  {
  }
  string CONNECTZONE::getName() const 
  {
        return _name;
  }
  string CONNECTZONE::getDescription() const 
  {
        return _description;
  }
  int CONNECTZONE::getDistantDomainNumber() const 
  {
        return _distantDomainNumber;
  }
  int CONNECTZONE::getLocalDomainNumber() const 
  {
        return _localDomainNumber;
  }
  
  MEDMEM::MESH * CONNECTZONE::getLocalMesh() const 
  {
        return _localMesh;
  }
  
  MEDMEM::MESH * CONNECTZONE::getDistantMesh() const 
  {
        return _distantMesh;
  }

  bool CONNECTZONE::isEntityCorrespPresent(MED_EN::medEntityMesh localEntity,
                              MED_EN::medEntityMesh distantEntity) const
{
        typedef map<pair<MED_EN::medEntityMesh,MED_EN::medEntityMesh>, MEDMEM::MEDSKYLINEARRAY*>::const_iterator map_iter;
        
        for (map_iter iter=_entityCorresp.begin(); iter != _entityCorresp.end(); iter++)
        {
                if ((iter->first).first==localEntity && (iter->first).second==distantEntity)
                        return true;
        }
        return false;
}                
              
  const int * CONNECTZONE::getNodeCorrespIndex() const
  {
        return _nodeCorresp->getIndex();
  }
  
  const int * CONNECTZONE::getNodeCorrespValue() const
  {
        return _nodeCorresp->getValue();
  }
  int CONNECTZONE::getNodeNumber() const
  {
        return _nodeCorresp->getNumberOf();
  }
  const int * CONNECTZONE::getFaceCorrespIndex() const
  {
        return _faceCorresp->getIndex();
  }
  
  const int * CONNECTZONE::getFaceCorrespValue() const
  {
        return _faceCorresp->getValue();
  }
  int CONNECTZONE::getFaceNumber() const
  {
        return _faceCorresp->getNumberOf();
  }
const int * CONNECTZONE::getEntityCorrespIndex(MED_EN::medEntityMesh localEntity,
                                    MED_EN::medEntityMesh distantEntity) const
{
        typedef map<pair<MED_EN::medEntityMesh,MED_EN::medEntityMesh>, MEDMEM::MEDSKYLINEARRAY*>::const_iterator map_iter;
        
        for (map_iter iter=_entityCorresp.begin(); iter != _entityCorresp.end(); iter++)
        {
                if ((iter->first).first==localEntity && (iter->first).second==distantEntity)
                        return iter->second->getIndex();
        }
        return 0;                       
}

const int * CONNECTZONE::getEntityCorrespValue(MED_EN::medEntityMesh localEntity,
                                    MED_EN::medEntityMesh distantEntity) const
{
        typedef map<pair<MED_EN::medEntityMesh,MED_EN::medEntityMesh>, MEDMEM::MEDSKYLINEARRAY*>::const_iterator map_iter;
        
        for (map_iter iter=_entityCorresp.begin(); iter != _entityCorresp.end(); iter++)
        {
                if ((iter->first).first==localEntity && (iter->first).second==distantEntity)
                        return iter->second->getValue();
        }
        return 0;                       
}

int CONNECTZONE::getEntityCorrespNumber(MED_EN::medEntityMesh localEntity,
                  MED_EN::medEntityMesh distantEntity) const
{
  typedef map<pair<MED_EN::medEntityMesh,MED_EN::medEntityMesh>, MEDMEM::MEDSKYLINEARRAY*>::const_iterator map_iter;
  
  for (map_iter iter=_entityCorresp.begin(); iter != _entityCorresp.end(); iter++)
  {
    if ((iter->first).first==localEntity && (iter->first).second==distantEntity)
      return iter->second->getNumberOf();
  }
  return 0;           
}


int CONNECTZONE::getEntityCorrespLength(MED_EN::medEntityMesh localEntity,
                  MED_EN::medEntityMesh distantEntity) const
{
  typedef map<pair<MED_EN::medEntityMesh,MED_EN::medEntityMesh>, MEDMEM::MEDSKYLINEARRAY*>::const_iterator map_iter;
  
  for (map_iter iter=_entityCorresp.begin(); iter != _entityCorresp.end(); iter++)
  {
    if ((iter->first).first==localEntity && (iter->first).second==distantEntity)
      return iter->second->getLength();
  }
  return 0;           
}

  void CONNECTZONE::setName(string name) 
  {
        _name=name;
  }
  void CONNECTZONE::setDescription(string description)
  {
        _description=description;
  }
  void CONNECTZONE::setDistantDomainNumber(int distantDomainNumber)
  {
        _distantDomainNumber=distantDomainNumber;
  }
  void CONNECTZONE::setLocalDomainNumber(int localDomainNumber)
  {
        _localDomainNumber=localDomainNumber;
  }
  void CONNECTZONE::setLocalMesh(MEDMEM::MESH * localMesh)
  {
        _localMesh=localMesh;
  }
  
  void CONNECTZONE::setDistantMesh(MEDMEM::MESH * distantMesh)
  {
        _distantMesh=distantMesh;
  }

/*! transforms an int array containing 
 * the node-node connections
 * to a MEDSKYLINEARRAY
 */
  void CONNECTZONE::setNodeCorresp(int * nodeCorresp, int nbnode)
  {
        int* index= new int[nbnode];
        for (int i=0; i<nbnode; i++)
        {
                index[i]=2*i;
        }
        _nodeCorresp = new MEDMEM::MEDSKYLINEARRAY(nbnode, 2*nbnode, index, nodeCorresp);
  }
  
  void CONNECTZONE::setNodeCorresp(MEDMEM::MEDSKYLINEARRAY* array)
  {
        _nodeCorresp = array;
  }
  /*! transforms an int array containing 
 * the face-face connections
 * to a MEDSKYLINEARRAY
 */
  void CONNECTZONE::setFaceCorresp(int * faceCorresp, int nbface)
  {
        int* index= new int[nbface];
        for (int i=0; i<nbface; i++)
        {
                index[i]=2*i;
        }
        _faceCorresp = new MEDMEM::MEDSKYLINEARRAY(nbface, 2*nbface, index, faceCorresp);
  }
  
  void CONNECTZONE::setFaceCorresp(MEDMEM::MEDSKYLINEARRAY* array)
  {
        _faceCorresp = array;
  }
  
  /*! transforms an int array containing 
 * the entity-entity connections
 * to a MEDSKYLINEARRAY
 * 
 * the resulting MEDSKYLINEARRAY is put in the map
 */
  void CONNECTZONE::setEntityCorresp(MED_EN::medEntityMesh localEntity,
                        MED_EN::medEntityMesh distantEntity,
                        int * entityCorresp, int nbentity)
  {
        int* index= new int[nbentity];
        for (int i=0; i<nbentity; i++)
        {
                index[i]=2*i;
        }
        _entityCorresp[make_pair(localEntity,distantEntity)]=
         new MEDMEM::MEDSKYLINEARRAY(nbentity, 2*nbentity, index, entityCorresp);
  }
  
  
    void CONNECTZONE::setEntityCorresp(MED_EN::medEntityMesh localEntity,
                        MED_EN::medEntityMesh distantEntity,
                        MEDMEM::MEDSKYLINEARRAY* array)
  {
        _entityCorresp[make_pair(localEntity,distantEntity)]=array;
  }
  






