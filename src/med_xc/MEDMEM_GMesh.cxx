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
// File      : MEDMEM_GMesh.cxx
// Created   : Fri Jul 23 12:32:18 2010
// Author    : Edward AGAPOV (eap)
//

#include "MEDMEM_GMesh.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_DriverFactory.hxx"

using namespace MEDMEM;
using namespace MED_EN;
using namespace std;

/*!
\defgroup GMESH_general MESH General information
These methods are related to the retrieval of general information about the mesh.

\defgroup MESH_families Families and Groups handling
The methods described in this section enable the manipulation of families and groups. These
notions define subsets of MED elements in a mesh. They differ because families are non
overlapping (a mesh element is associated to zero or one family)  while groups are more general.

\defgroup MESH_io Mesh I/O
These methods describe how to read and write meshes. Generally speaking, meshes should be read
via a constructor and should be written with the write() method.

*/

//================================================================================
/*! Create an empty GMESH. */
//================================================================================

GMESH::GMESH()
{
  init();
}

//================================================================================
/*!
  Copy constructor
*/
//================================================================================

GMESH::GMESH(GMESH &m)
{
  _name = m._name;
  _description = m._description;
  _spaceDimension = m._spaceDimension;

  _familyNode = m._familyNode;
  for (int i=0; i<(int)m._familyNode.size(); i++)
  {
    _familyNode[i] = new FAMILY(* m._familyNode[i]);
    _familyNode[i]->setMesh(this);
    removeReference();
  }

  _familyCell = m._familyCell;
  for (int i=0; i<(int)m._familyCell.size(); i++)
  {
    _familyCell[i] = new FAMILY(* m._familyCell[i]);
    _familyCell[i]->setMesh(this);
    removeReference();
  }

  _familyFace = m._familyFace;
  for (int i=0; i<(int)m._familyFace.size(); i++)
  {
    _familyFace[i] = new FAMILY(* m._familyFace[i]);
    _familyFace[i]->setMesh(this);
    removeReference();
  }

  _familyEdge = m._familyEdge;
  for (int i=0; i<(int)m._familyEdge.size(); i++)
  {
    _familyEdge[i] = new FAMILY(* m._familyEdge[i]);
    _familyEdge[i]->setMesh(this);
    removeReference();
  }

  _groupNode = m._groupNode;
  for (int i=0; i<(int)m._groupNode.size(); i++)
  {
    _groupNode[i] = new GROUP(* m._groupNode[i]);
    _groupNode[i]->setMesh(this);
    removeReference();
  }

  _groupCell = m._groupCell;
  for (int i=0; i<(int)m._groupCell.size(); i++)
  {
    _groupCell[i] = new GROUP(* m._groupCell[i]);
    _groupCell[i]->setMesh(this);
    removeReference();
  }

  _groupFace = m._groupFace;
  for (int i=0; i<(int)m._groupFace.size(); i++)
  {
    _groupFace[i] = new GROUP(* m._groupFace[i]);
    _groupFace[i]->setMesh(this);
    removeReference();
  }

  _groupEdge = m._groupEdge;
  for (int i=0; i<(int)m._groupEdge.size(); i++)
  {
    _groupEdge[i] = new GROUP(* m._groupEdge[i]);
    _groupEdge[i]->setMesh(this);
    removeReference();
  }

  //_drivers = m._drivers;  //Recopie des drivers?
}

//================================================================================
/*!
 * \brief Destructor
 */
//================================================================================

GMESH::~GMESH()
{
  // if this is an automatic variable, be sure that destructor will be no more called
  // due to cyclic dependencies with child reference counters (groups and families)
  clearRefCouner();

  for (unsigned i=0;i<_familyNode.size();i++)
  {
    // if another object refers to a family and it survive now,
    // it sould not refer to a dead mesh
    _familyNode[i]->setMesh(0);
    _familyNode[i]->setMeshName( getName() );
    _familyNode[i]->removeReference();
  }
  _familyNode.clear();
  for (unsigned i=0;i<_familyCell.size();i++)
  {
    _familyCell[i]->setMesh(0);
    _familyCell[i]->setMeshName( getName() );
    _familyCell[i]->removeReference();
  }
  _familyCell.clear();
  for (unsigned i=0;i<_familyFace.size();i++)
  {
    _familyFace[i]->setMesh(0);
    _familyFace[i]->setMeshName( getName() );
    _familyFace[i]->removeReference();
  }
  _familyFace.clear();
  for (unsigned i=0;i<_familyEdge.size();i++)
  {
    _familyEdge[i]->setMesh(0);
    _familyEdge[i]->setMeshName( getName() );
    _familyEdge[i]->removeReference();
  }
  _familyEdge.clear();
  for (unsigned i=0;i<_groupNode.size();i++)
  {
    _groupNode[i]->setMesh(0);
    _groupNode[i]->setMeshName( getName() );
    _groupNode[i]->removeReference();
  }
  _groupNode.clear();
  for (unsigned i=0;i<_groupCell.size();i++)
  {
    _groupCell[i]->setMesh(0);
    _groupCell[i]->setMeshName( getName() );
    _groupCell[i]->removeReference();
  }
  _groupCell.clear();
  for (unsigned i=0;i<_groupFace.size();i++)
  {
    _groupFace[i]->setMesh(0);
    _groupFace[i]->setMeshName( getName() );
    _groupFace[i]->removeReference();
  }
  _groupFace.clear();
  for (unsigned i=0;i<_groupEdge.size();i++)
  {
    _groupEdge[i]->setMesh(0);
    _groupEdge[i]->setMeshName( getName() );
    _groupEdge[i]->removeReference();
  }
  _groupEdge.clear();

  map<medEntityMesh,SUPPORT*>::iterator it = _entitySupport.begin();
  for(;it!=_entitySupport.end();it++)
    if((*it).second != NULL)
    {
      (*it).second->setMesh(0);
      (*it).second->setMeshName( getName() );
      (*it).second->removeReference();
    }
  _entitySupport.clear();

  for (unsigned int index=0; index < _drivers.size(); index++ )
    if ( _drivers[index] ) delete _drivers[index];
  _drivers.clear();
}

//================================================================================
/*!
 * \brief Initialization of fields
 */
//================================================================================

void GMESH::init()
{
  const char* LOC = "GMESH::init(): ";
  BEGIN_OF_MED(LOC);

  _name          ="NOT DEFINED";
  _spaceDimension=MED_INVALID;
  
  END_OF_MED(LOC);
}

//================================================================================
/*!
 * \brief Copy operator
 */
//================================================================================

GMESH & GMESH::operator=(const GMESH &m)
{
  const char* LOC = "GMESH & GMESH::operator=(const GMESH &m) : ";
  BEGIN_OF_MED(LOC);
  MESSAGE_MED("Not yet implemented");
  END_OF_MED(LOC);

  return *this;
}

//================================================================================
/*!
 * \brief Comparison
 */
//================================================================================

bool GMESH::operator==(const GMESH& other) const
{
  return this==&other;
}

//================================================================================
/*!
 * \brief Print operator
 */
//================================================================================

ostream & MEDMEM::operator<<(ostream &os, const GMESH &myMesh)
{
  myMesh.printMySelf(os);
  return os;
}

/*! \if MEDMEM_ug
\addtogroup MESH_io
@{
\endif
*/
//================================================================================
/*!
 * \brief Add a %MESH driver of type %driverTypes (MED_DRIVER, ....) associated
 *        with file fileName. The meshname used in the file is  driverName.
 *        addDriver returns an integer handler.
 */
//================================================================================

int  GMESH::addDriver(driverTypes            driverType,
                      const std::string &    fileName,
                      const std::string &    driverName,
                      MED_EN::med_mode_acces access)
{
  const char* LOC = "GMESH::addDriver(driverTypes driverType, const string & fileName=\"Default File Name.med\",const string & driverName=\"Default Mesh Name\",MED_EN::med_mode_acces access) : ";
  BEGIN_OF_MED(LOC);

  GENDRIVER * driver;

  SCRUTE_MED(driverType);

  driver = DRIVERFACTORY::buildDriverForMesh(driverType,fileName,this,driverName,access) ;

  _drivers.push_back(driver);

  int current = _drivers.size()-1;

  _drivers[current]->setMeshName(driverName);

  END_OF_MED(LOC);

  return current;
}

//================================================================================
/*! Add an existing MESH driver. */
//================================================================================

int GMESH::addDriver(GENDRIVER & driver)
{
  const char* LOC = "GMESH::addDriver(GENDRIVER &) : ";
  BEGIN_OF_MED(LOC);

  // For the case where driver does not know about me since it has been created through
  // constructor witout parameters: create newDriver knowing me and get missing data
  // from driver using merge()
  //GENDRIVER * newDriver = driver.copy() ;
  GENDRIVER* newDriver = DRIVERFACTORY::buildDriverForMesh(driver.getDriverType(),
                                                           driver.getFileName(), this,
                                                           driver.getMeshName(),
                                                           driver.getAccessMode());
  _drivers.push_back(newDriver);

  int current = _drivers.size()-1;
  driver.setId(current);

  newDriver->merge( driver );
  newDriver->setId( current );

  return current;

  END_OF_MED(LOC);
}

//================================================================================
/*! Remove an existing MESH driver. */
//================================================================================

void GMESH::rmDriver (int index/*=0*/)
{
  const char * LOC = "GMESH::rmDriver (int index=0): ";
  BEGIN_OF_MED(LOC);

  if (index >= 0 && index < (int)_drivers.size() && _drivers[index])
  {
    delete _drivers[index];
    _drivers[index] = 0;
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "The index given is invalid, index must be between  0 and  |"
                                     << _drivers.size()
                                     ));
  END_OF_MED(LOC);
}

//================================================================================
/*!
 * \brief Read mesh from the file using given driver
 *  \param index - index of driver to use for reading
 */
//================================================================================

void GMESH::read(int index)
{
  const char * LOC = "GMESH::read(int index=0) : ";
  BEGIN_OF_MED(LOC);

  if (index >= 0 && index < (int)_drivers.size() && _drivers[index])
  {
    _drivers[index]->open();
    try
    {
      _drivers[index]->read();
    }
    catch ( const MEDEXCEPTION& ex )
    {
      _drivers[index]->close();
      throw ex;
    }
    _drivers[index]->close();
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "The index given is invalid, index must be between  0 and |"
                                     << _drivers.size()
                                     ));
  END_OF_MED(LOC);
}

//================================================================================
/*!
  Reads GMESH using the given driver.
*/
//================================================================================

void GMESH::read(const GENDRIVER & driver)
{
  // For the case where driver does not know about me since it has been created through
  // constructor witout parameters: create newDriver knowing me and get missing data
  // from driver using merge()
  auto_ptr<GENDRIVER> newDriver( DRIVERFACTORY::buildDriverForMesh(driver.getDriverType(),
                                                                   driver.getFileName(),
                                                                   this,
                                                                   driver.getMeshName(),
                                                                   driver.getAccessMode()));
  newDriver->merge( driver );

  newDriver->open();
  try
  {
    newDriver->read();
  }
  catch ( const MEDEXCEPTION& ex )
  {
    newDriver->close();
    throw ex;
  }
  newDriver->close();
}

//================================================================================
/*!
 * \brief Reads the GMESH
 *  \param driverType - type of driver to use for reading
 *  \param filename - file to read from
 *  \param meshname - name of a mesh to read
 */
//================================================================================

void GMESH::read(driverTypes        driverType,
                 const std::string& filename,
                 const std::string& meshname)
{
  auto_ptr<GENDRIVER> newDriver( DRIVERFACTORY::buildDriverForMesh(driverType, filename,
                                                                   this, meshname, RDONLY));
  newDriver->open();
  try
  {
    newDriver->read();
  }
  catch ( const MEDEXCEPTION& ex )
  {
    newDriver->close();
    throw ex;
  }
  newDriver->close();
}

//================================================================================
/*! Writes all the content of the MESH using driver referenced by the integer handle returned by a \a addDriver call.

  Example :
\verbatim
//...
// Attaching the driver to file "output.med", meshname "Mesh"
int driver_handle = mesh.addDriver(MED_DRIVER, "output.med", "Mesh");
// Writing the content of mesh to the file
mesh.write(driver_handle);
\endverbatim
*/
//================================================================================

void GMESH::write(int index) const
  {
    const char LOC[] = "GMESH::write(int index=0) : ";
    BEGIN_OF_MED(LOC);
    if( index > -1 && index < (int)_drivers.size() && _drivers[index] )
      {
        _drivers[index]->open();
        try
          {
            _drivers[index]->write();
          }
        catch ( const MEDEXCEPTION& ex )
          {
            _drivers[index]->close();
            throw ex;
          }
        _drivers[index]->close();
      }
    else
      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "The index given is invalid, index must be between  0 and |"
                                     << _drivers.size()
                                     ));
    END_OF_MED(LOC);
  }

//================================================================================
/*!
  Writes all the content of the GMESH using the given driver.
*/
//================================================================================

void GMESH::write(const GENDRIVER & driver, MED_EN::med_mode_acces medMode/*=MED_EN::WRONLY*/) const
  {

    // For the case where driver does not know about me since it has been created through
    // constructor witout parameters: create newDriver knowing me and get missing data
    // from driver using merge()
    auto_ptr<GENDRIVER> newDriver( DRIVERFACTORY::buildDriverForMesh(driver.getDriverType(),
                                                                   driver.getFileName(),
                                                                   const_cast<GMESH*>( this ),
                                                                   driver.getMeshName(),
                                                                   MED_EN::WRONLY));
  newDriver->merge( driver );
//   if ( newDriver->getDriverType() == MED_DRIVER )
//     newDriver->setAccessMode( MED_EN::med_mode_acces( getMedAccessMode( medMode )));

  newDriver->open();
  try
  {
    newDriver->write();
  }
  catch ( const MEDEXCEPTION& ex )
  {
    newDriver->close();
    throw ex;
  }
  newDriver->close();
}

//================================================================================
/*!
 * \brief Writes all the content of the GMESH
 *  \param driverType - type of driver to use for writing
 *  \param filename - file to write into
 *  \param meshname - mesh name
 *  \param medMode - file access mode
 */
//================================================================================

void GMESH::write(driverTypes        driverType,
                  const std::string& filename,
                  const std::string& meshname,
                  MED_EN::med_mode_acces medMode/*=MED_EN::WRONLY*/) const
  {
    unique_ptr<GENDRIVER> newDriver( DRIVERFACTORY::buildDriverForMesh(driverType, filename,
                                        const_cast<GMESH*> ( this ),
                                        meshname.empty() ? _name : meshname,
                                        MED_EN::WRONLY));

//   if ( newDriver->getDriverType() == MED_DRIVER )
//     newDriver->setAccessMode( MED_EN::med_mode_acces( getMedAccessMode( medMode ) ));

  newDriver->open();
  try
  {
    newDriver->write();
  }
  catch ( const MEDEXCEPTION& ex )
  {
    newDriver->close();
    throw ex;
  }
  newDriver->close();
}
/*! \if MEDMEM_ug @} \endif */

/*! \if MEDMEM_ug
\addtogroup MESH_general
@{
\endif
*/

//================================================================================
/*!
 * Sets the MESH name. Name should not exceed MED_TAILLE_NOM
   length defined in Med (i.e. 32 characters).
*/
//================================================================================

void GMESH::setName(std::string name)
{
  _name=name; //NOM interne à la classe
}

//================================================================================
/*!
 *Gets the MESH name.
 */
//================================================================================

string GMESH::getName() const
{
  return _name;
}

//================================================================================
/*! Set the MESH description */
/*! \ifnot MEDMEM_ug
Sets the MESH description. Description should not exceed MED_TAILLE_DESC length defined in Med (i.e. 200 characters).
\endif */
//================================================================================

void GMESH::setDescription(std::string description)
{
  _description = description; //NOM interne à la classe
}

//================================================================================
/*! Gets the MESH description. The string returned contains
  a short description of the mesh, which is stored for
  information purposes only.*/
//================================================================================

string GMESH::getDescription() const
{
  return _description;
}

//================================================================================
/*! Gets the dimension of the space in which the
  mesh is described (2 for planar meshes, 3 for volumes and
  3D surfaces) . */
//================================================================================

int GMESH::getSpaceDimension() const
{
  return _spaceDimension;
}

/*! \if MEDMEM_ug
The retrieval of general information about a mesh is illustrated in the following C++ example. Its Python equivalent can be found in \a MESHgeneral.py.
This example illustrates how to retrieve the name, description, mesh and space dimensions.

\example MESHgeneral.cxx
@}
\endif
*/

/*! \if MEDMEM_ug
\addtogroup MESH_families
@{
\endif
*/
//================================================================================
/*!
  Retrieves the number of families in the mesh for entity type \a entity
*/
//================================================================================

int GMESH::getNumberOfFamilies (MED_EN::medEntityMesh entity) const
{
  switch (entity) {
  case MED_EN::MED_NODE : return _familyNode.size();
  case MED_EN::MED_CELL : return _familyCell.size();
  case MED_EN::MED_FACE : return _familyFace.size();
  case MED_EN::MED_EDGE : return _familyEdge.size();
  default :
    throw MEDEXCEPTION("MESH::getNumberOfFamilies : Unknown entity");
  }
}
//================================================================================
/*! Returns the families of type \a entity present in the mesh as a vector of pointers */
//================================================================================

const vector<MEDMEM::FAMILY*> GMESH::getFamilies(MED_EN::medEntityMesh entity) const
{
  switch (entity) {
  case MED_EN::MED_NODE : return _familyNode;
  case MED_EN::MED_CELL : return _familyCell;
  case MED_EN::MED_FACE : return _familyFace;
  case MED_EN::MED_EDGE : return _familyEdge;
  default :
    throw MEDEXCEPTION("MESH::getFamilies : Unknown entity");
  }
}

//================================================================================
/*!
 * \brief Return a family
 *  \param entity - entity of the family
 *  \param i - index of the family
 *  \retval const MEDMEM::FAMILY* - pointer to the family
 */
//================================================================================

const MEDMEM::FAMILY* GMESH::getFamily(MED_EN::medEntityMesh entity, int i) const
{
  if (i<=0)
    throw MEDEXCEPTION("MESH::getFamily(i) : argument i must be > 0");
  const vector<FAMILY*>* Family;
  switch (entity) {
  case MED_EN::MED_NODE : Family = &_familyNode; break;
  case MED_EN::MED_CELL : Family = &_familyCell; break;
  case MED_EN::MED_FACE : Family = &_familyFace; break;
  case MED_EN::MED_EDGE : Family = &_familyEdge; break;
  default :
    throw MEDEXCEPTION("MESH::getFamilies : Unknown entity");
  }
  if (i>(int)Family->size())
    throw MEDEXCEPTION("MESH::getFamily(entity,i) : argument i must be <= _numberOfFamilies");
  return (*Family)[i-1];
}


//================================================================================
/*! Retrieves the number of groups in the mesh for entity type \a entity */
//================================================================================

int GMESH::getNumberOfGroups (MED_EN::medEntityMesh entity) const
{
  switch (entity) {
  case MED_EN::MED_NODE : return _groupNode.size();
  case MED_EN::MED_CELL : return _groupCell.size();
  case MED_EN::MED_FACE : return _groupFace.size();
  case MED_EN::MED_EDGE : return _groupEdge.size();
  default :
    throw MEDEXCEPTION("MESH::getNumberOfGroups : Unknown entity");
  }
}
//================================================================================
/*! Returns the groups of type \a entity present in the mesh as a vector of pointers. The GROUP class inheriting from the SUPPORT class, the
  methods that can be used on these groups are explained in the related section.*/
//================================================================================

const vector<GROUP*> GMESH::getGroups(MED_EN::medEntityMesh entity) const
{
  switch (entity) {
  case MED_EN::MED_NODE : return _groupNode;
  case MED_EN::MED_CELL : return _groupCell;
  case MED_EN::MED_FACE : return _groupFace;
  case MED_EN::MED_EDGE : return _groupEdge;
  default :
    throw MEDEXCEPTION("MESH::getGroups : Unknown entity");
  }
}

//================================================================================
/*!
 * \brief Return a group
 *  \param entity - entity of the group
 *  \param i - index of the group
 *  \retval const GROUP* - pointer to the group
 */
//================================================================================

const GROUP* GMESH::getGroup(MED_EN::medEntityMesh entity, int i) const
{
  const char * LOC = "GMESH::getGroup(medEntityMesh entity, int i) : ";
  if (i<=0)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"argument i must be > 0"));
  const vector<GROUP*>* Group = 0;
  switch (entity) {
  case MED_EN::MED_NODE : Group = &_groupNode; break;
  case MED_EN::MED_CELL : Group = &_groupCell; break;
  case MED_EN::MED_FACE : Group = &_groupFace; break;
  case MED_EN::MED_EDGE : Group = &_groupEdge; break;
  default :
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Unknown entity"));
  }
  if (i>(int)Group->size())
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"argument i="<<i<<" must be <= _numberOfGroups="<<Group->size()));
  return (*Group)[i-1];
}

//================================================================================
/*! Retrieves the group named \a name.
  The method browses all the entities in order to find the group.
  If two groups with the same name coexist, the first one found will be
  returned. If no group with the correct name is found, the method throws
  an exception.
*/
//================================================================================

const GROUP* GMESH::getGroup(const string& name) const  throw (MEDEXCEPTION)
{
  const vector<GROUP*>* group_vectors [4]={&_groupNode, &_groupEdge,&_groupFace,&_groupCell};
  for (int ientity=0;ientity<4;ientity++)
    for (unsigned igroup=0; igroup< group_vectors[ientity]->size();igroup++)
    {
      const vector<GROUP*>& group_vect = *group_vectors[ientity];
      GROUP* group=group_vect[igroup];
      if (group->getName()==name)
        return group;
    }
  cerr << "MESH::getGroup("<<name<<") : group "<<name <<" was not found"<<endl;
  throw MEDEXCEPTION("MESH::getGroup(name) : name not found");
}

//================================================================================
/*!
  Method returns a reference on a support defined on all the elements of an entity.
*/
//================================================================================

const SUPPORT * GMESH::getSupportOnAll(medEntityMesh entity) const throw(MEDEXCEPTION)
{
  const char * LOC = "GMESH::getSupportOnAll : " ;
  BEGIN_OF_MED(LOC) ;
  if(entity == MED_ALL_ENTITIES)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined on entity MED_ALL_ENTITIES !"));

  map<medEntityMesh,SUPPORT*>::const_iterator it =  _entitySupport.find(entity);

  // find support and return is if exists
  if(it != _entitySupport.end())
    return (*it).second;
  else
    {
      //build, store and return support
      GMESH* thisMesh = const_cast< GMESH* >( this );
      SUPPORT * aSupport = new SUPPORT;
      string aSuppName = "SupportOnAll_"+entNames[entity];
      aSupport->setName( aSuppName );
      aSupport->setMesh( thisMesh );
      aSupport->setEntity( entity );
      aSupport->setAll( true );
      aSupport->update();

      thisMesh->_entitySupport.insert(make_pair(entity,aSupport));
      thisMesh->removeReference();

      return aSupport;
    }
}

/*! \if MEDMEM_ug @} \endif */

/*! \if MEDMEM_ug
\addtogroup MESH_advanced
@{
\endif
*/
//================================================================================
/*!
  Return a SUPPORT pointer on the union of all SUPPORTs in Supports.
  You should delete this pointer after use to avoid memory leaks.
*/
//================================================================================

SUPPORT * GMESH::mergeSupports(const std::vector<SUPPORT *> Supports) throw (MEDEXCEPTION)
{
  const char * LOC = "GMESH::mergeSupports(const vector<SUPPORT *> ) : " ;
  BEGIN_OF_MED(LOC) ;

  SUPPORT * returnedSupport;
  string returnedSupportName;
  string returnedSupportDescription;
  char * returnedSupportNameChar;
  char * returnedSupportDescriptionChar;
  int size = Supports.size();

  if (size == 0)
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) <<
                                     " mergeSupports() does't accept zero size vector"));

  if (size == 1)
  {
    MESSAGE_MED(LOC <<" there is only one SUPPORT in the argument list, the method return a copy of this object !");
    SUPPORT * obj = const_cast <SUPPORT *> (Supports[0]);

    returnedSupport = new SUPPORT(*obj);

    int lenName = strlen((Supports[0]->getName()).c_str()) + 8 + 1;
    int lenDescription = strlen((Supports[0]->getDescription()).c_str()) + 8 + 1;

    returnedSupportNameChar = new char[lenName];
    returnedSupportDescriptionChar = new char[lenDescription];

    returnedSupportNameChar = strcpy(returnedSupportNameChar,"Copy of ");
    returnedSupportNameChar = strcat(returnedSupportNameChar,(Supports[0]->getName()).c_str());
    returnedSupportDescriptionChar = strcpy(returnedSupportDescriptionChar,"Copy of ");
    returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,
                                            (Supports[0]->getDescription()).c_str());

    returnedSupportName = string(returnedSupportNameChar);
    returnedSupportDescription = string(returnedSupportDescriptionChar);

    returnedSupport->setName(returnedSupportName);
    returnedSupport->setDescription(returnedSupportDescription);

    delete [] returnedSupportNameChar;
    delete [] returnedSupportDescriptionChar;
  }
  else
  {
    SUPPORT * obj = const_cast <SUPPORT *> (Supports[0]);
    returnedSupport = new SUPPORT(*obj);

    int lenName = strlen((Supports[0]->getName()).c_str()) + 9 + 1;
    int lenDescription = strlen((Supports[0]->getDescription()).c_str()) + 9 + 1;

    for(int i = 1;i<size;i++)
    {
      obj = const_cast <SUPPORT *> (Supports[i]);
      returnedSupport->blending(obj);

      if (i == (size-1))
      {
        lenName = lenName + 5 + strlen((Supports[i]->getName()).c_str());
        lenDescription = lenDescription + 5 +
          strlen((Supports[i]->getDescription()).c_str());
      }
      else
      {
        lenName = lenName + 2 + strlen((Supports[i]->getName()).c_str());
        lenDescription = lenDescription + 2 +
          strlen((Supports[i]->getDescription()).c_str());
      }
    }

    returnedSupportNameChar = new char[lenName];
    returnedSupportDescriptionChar = new char[lenDescription];

    returnedSupportNameChar = strcpy(returnedSupportNameChar,"Merge of ");
    returnedSupportDescriptionChar = strcpy(returnedSupportDescriptionChar,"Merge of ");

    returnedSupportNameChar = strcat(returnedSupportNameChar,(Supports[0]->getName()).c_str());
    returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,
                                            (Supports[0]->getDescription()).c_str());

    for(int i = 1;i<size;i++)
    {
      if (i == (size-1))
      {
        returnedSupportNameChar = strcat(returnedSupportNameChar," and ");
        returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar," and ");

        returnedSupportNameChar = strcat(returnedSupportNameChar,
                                         (Supports[i]->getName()).c_str());
        returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,
                                                (Supports[i]->getDescription()).c_str());
      }
      else
      {
        returnedSupportNameChar = strcat(returnedSupportNameChar,", ");
        returnedSupportNameChar = strcat(returnedSupportNameChar,
                                         (Supports[i]->getName()).c_str());

        returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,", ");
        returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,
                                                (Supports[i]->getDescription()).c_str());
      }
    }

    returnedSupportName = string(returnedSupportNameChar);
    returnedSupport->setName(returnedSupportName);

    returnedSupportDescription = string(returnedSupportDescriptionChar);
    returnedSupport->setDescription(returnedSupportDescription);

    delete [] returnedSupportNameChar;
    delete [] returnedSupportDescriptionChar;
  }

  END_OF_MED(LOC);
  return returnedSupport;
}

//================================================================================
/*!
  return a SUPPORT pointer on the intersection of all SUPPORTs in Supports.
  The (SUPPORT *) NULL pointer is returned if the intersection is empty.
  You should delete this pointer after use to avois memory leaks.
*/
//================================================================================

SUPPORT * GMESH::intersectSupports(const std::vector<SUPPORT *> Supports) throw (MEDEXCEPTION)
{
  const char* LOC = "MESH:::intersectSupports(const vector<SUPPORT *> ) : ";
  BEGIN_OF_MED(LOC);

  SUPPORT * returnedSupport;
  string returnedSupportName;
  string returnedSupportDescription;
  char * returnedSupportNameChar;
  char * returnedSupportDescriptionChar;
  int size = Supports.size();

  if (size == 1)
  {
    MESSAGE_MED(PREFIX_MED <<" there is only one SUPPORT in the argument list, the method return a copy of this object !");
    SUPPORT * obj = const_cast <SUPPORT *> (Supports[0]);

    returnedSupport = new SUPPORT(*obj);

    int lenName = strlen((Supports[0]->getName()).c_str()) + 8 + 1;
    int lenDescription = strlen((Supports[0]->getDescription()).c_str()) + 8 + 1;

    returnedSupportNameChar = new char[lenName];
    returnedSupportDescriptionChar = new char[lenDescription];

    returnedSupportNameChar = strcpy(returnedSupportNameChar,"Copy of ");
    returnedSupportNameChar = strcat(returnedSupportNameChar,(Supports[0]->getName()).c_str());
    returnedSupportDescriptionChar = strcpy(returnedSupportDescriptionChar,"Copy of ");
    returnedSupportDescriptionChar = strcat(returnedSupportDescriptionChar,
                                            (Supports[0]->getDescription()).c_str());

    returnedSupportName = string(returnedSupportNameChar);
    returnedSupportDescription = string(returnedSupportDescriptionChar);

    returnedSupport->setName(returnedSupportName);
    returnedSupport->setDescription(returnedSupportDescription);

    delete [] returnedSupportNameChar;
    delete [] returnedSupportDescriptionChar;
  }
  else
  {
    SUPPORT * obj = const_cast <SUPPORT *> (Supports[0]);
    returnedSupport = new SUPPORT(*obj);

    int lenName = strlen((Supports[0]->getName()).c_str()) + 16 + 1;
    int lenDescription = strlen((Supports[0]->getDescription()).c_str()) + 16 + 1;

    for(int i = 1;i<size;i++)
    {
      obj = const_cast <SUPPORT *> (Supports[i]);
      returnedSupport->intersecting(obj);

      if (i == (size-1))
      {
        lenName = lenName + 5 + strlen((Supports[i]->getName()).c_str());
        lenDescription = lenDescription + 5 +
          strlen((Supports[i]->getDescription()).c_str());
      }
      else
      {
        lenName = lenName + 2 + strlen((Supports[i]->getName()).c_str());
        lenDescription = lenDescription + 2 +
          strlen((Supports[i]->getDescription()).c_str());
      }
    }
    if(returnedSupport != (SUPPORT *) NULL)
    {
      returnedSupportNameChar = new char[lenName];
      returnedSupportDescriptionChar = new char[lenDescription];

      returnedSupportNameChar = strcpy(returnedSupportNameChar,
                                       "Intersection of ");
      returnedSupportDescriptionChar =
        strcpy(returnedSupportDescriptionChar,"Intersection of ");

      returnedSupportNameChar = strcat(returnedSupportNameChar,
                                       (Supports[0]->getName()).c_str());
      returnedSupportDescriptionChar =
        strcat(returnedSupportDescriptionChar,
               (Supports[0]->getDescription()).c_str());

      for(int i = 1;i<size;i++)
      {
        if (i == (size-1))
        {
          returnedSupportNameChar = strcat(returnedSupportNameChar,
                                           " and ");
          returnedSupportDescriptionChar =
            strcat(returnedSupportDescriptionChar," and ");

          returnedSupportNameChar =
            strcat(returnedSupportNameChar,
                   (Supports[i]->getName()).c_str());
          returnedSupportDescriptionChar =
            strcat(returnedSupportDescriptionChar,
                   (Supports[i]->getDescription()).c_str());
        }
        else
        {
          returnedSupportNameChar = strcat(returnedSupportNameChar,
                                           ", ");
          returnedSupportNameChar =
            strcat(returnedSupportNameChar,
                   (Supports[i]->getName()).c_str());

          returnedSupportDescriptionChar =
            strcat(returnedSupportDescriptionChar,", ");
          returnedSupportDescriptionChar =
            strcat(returnedSupportDescriptionChar,
                   (Supports[i]->getDescription()).c_str());
        }
      }

      returnedSupportName = string(returnedSupportNameChar);
      returnedSupport->setName(returnedSupportName);

      returnedSupportDescription = string(returnedSupportDescriptionChar);
      returnedSupport->setDescription(returnedSupportDescription);

      delete [] returnedSupportNameChar;
      delete [] returnedSupportDescriptionChar;
    }
  }

  END_OF_MED(LOC);
  return returnedSupport;
}

/*! \if MEDMEM_ug
  @}
\endif
*/
/*!
  Method created to factorize code. This method creates a new support on entity 'entity' (to deallocate) containing all the entities contained in
  elements 'listOfElt' of entity 'entity'.
*/
SUPPORT * GMESH::buildSupportOnElementsFromElementList(const list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION)
{
  const char* LOC = "GMESH::buildSupportOnElementsFromElementList : ";
  BEGIN_OF_MED(LOC);
  SUPPORT * mySupport = new SUPPORT();
  mySupport->setMesh((MESH *)this);
  mySupport->setName("Boundary");
  mySupport->setEntity( entity );
  mySupport->fillFromElementList(listOfElt);
  END_OF_MED(LOC);
  return mySupport ;
}

// internal helper type
struct _cell
{
  std::vector<int> groups;
  MED_EN::medGeometryElement geometricType;
  // to have geometricType good for nodal groups when MESH::getNumberOfTypes(MED_EN::MED_NODE)==0
  _cell():geometricType(MED_EN::MED_POINT1) {}
};

//================================================================================
/*!
 * Create families from groups.
 * This function is automaticaly called whenever we ask for families that are not up-to-date.
 * (The creation of families is delayed to the need of user.)
 * If a new created family hapen to already exist, we keep the old one.
 * (There is no way to know which family has change.)
 */
//================================================================================

void GMESH::createFamilies()
{
  int idFamNode = 0; // identifier for node families
  int idFamElement = 0; // identifier for cell, face or edge families

  // Main loop on mesh's entities
  for (medEntityMesh entity=MED_EN::MED_CELL; entity!=MED_ALL_ENTITIES; ++entity)
  {
    int numberofgroups = getNumberOfGroups(entity);
    if(!numberofgroups)
      continue; // no groups for this entity

    vector< vector<FAMILY*> > whichFamilyInGroup(numberofgroups); // this container is used to update groups at the end

    // make myFamilies points to the member corresponding to entity
    vector<FAMILY*> *myFamilies= NULL;
    switch ( entity )
    {
    case MED_EN::MED_CELL : myFamilies = & _familyCell; break;
    case MED_EN::MED_FACE : myFamilies = & _familyFace; break;
    case MED_EN::MED_EDGE : myFamilies = & _familyEdge; break;
    case MED_EN::MED_NODE : myFamilies = & _familyNode; break;
    }

    vector<GROUP*> myGroups=getGroups(entity); // get a copy of the groups ptr for the entity
    // get a copy of the (old) family ptrs before clearing
    vector<FAMILY*> myOldFamilies=getFamilies(entity);
    myFamilies->clear();


    // 1 - Create a vector containing for each cell (of the entity) an information structure
    //     giving geometric type and the groups it belong to

    med_int numberOfTypes=0;
    const medGeometryElement* geometricTypes = 0;
    if ( entity != MED_EN::MED_NODE)
    {
      numberOfTypes = getNumberOfTypes(entity);
      geometricTypes = getTypes(entity);
    }
    med_int numberOfCells=getNumberOfElements(entity, MED_ALL_ELEMENTS);  // total number of cells for that entity
    SCRUTE_MED(numberOfTypes);
    SCRUTE_MED(numberOfCells);
    vector< _cell > tab_cell(numberOfCells);
    vector< _cell >::iterator cell = tab_cell.begin();
    for(med_int t=0; t!=numberOfTypes; ++t)
    {
      int nbCellsOfType = getNumberOfElements(entity,geometricTypes[t]);
      for(int n=0; n!=nbCellsOfType; ++n, ++cell)
        cell->geometricType=geometricTypes[t];
    }

    // 2 - Scan cells in groups and update in tab_cell the container of groups a cell belong to

    for (unsigned g=0; g!=myGroups.size(); ++g)
    {
      // scan cells that belongs to the group
      const int* groupCells=myGroups[g]->getnumber()->getValue();
      int nbCells=myGroups[g]->getnumber()->getLength();
      for(int c=0; c!=nbCells; ++c)
        tab_cell[groupCells[c]-1].groups.push_back(g);
    }


    // 3 - Scan the cells vector, genarate family name, and create a map associating the family names
    //     whith the vector of contained cells

    map< string,vector<int> > tab_families;
    map< string,vector<int> >::iterator fam;
    for(int n=0; n!=numberOfCells; ++n)
    {
      ostringstream key; // to generate the name of the family
      key << "FAM";
      if(tab_cell[n].groups.empty()) // this cell don't belong to any group
        key << "_NONE" << entity;

      for(vector<int>::const_iterator it=tab_cell[n].groups.begin(); it!=tab_cell[n].groups.end(); ++it)
      {
        string groupName=myGroups[*it]->getName();
        if(groupName.empty())
          key << "_G" << *it;
        else
          key << "_" << groupName;
      }

      tab_families[key.str()].push_back(n+1); // fill the vector of contained cells associated whith the family
    }


    // 4 - Scan the family map, create MED Families, check if it already exist.

    for( fam=tab_families.begin(); fam!=tab_families.end(); ++fam)
    {
      vector<medGeometryElement> tab_types_geometriques;
      medGeometryElement geometrictype=MED_EN::MED_NONE;
      vector<int> tab_index_types_geometriques;
      vector<int> tab_nombres_elements;
      if ( fam->second.empty() )
        continue; // it is just a truncated long family name

      // scan family cells and fill the tab that are needed by the create a MED FAMILY
      for( unsigned i=0; i!=fam->second.size(); ++i)
      {
        int ncell=fam->second[i]-1;
        if(tab_cell[ncell].geometricType != geometrictype)
        {
          // new geometric type -> we store it and complete index tabs
          if(!tab_index_types_geometriques.empty())
            tab_nombres_elements.push_back(i+1-tab_index_types_geometriques.back());
          tab_types_geometriques.push_back( (geometrictype=tab_cell[ncell].geometricType));
          tab_index_types_geometriques.push_back(i+1);
        }
      }
      // store and complete index tabs for the last geometric type
      tab_nombres_elements.push_back(fam->second.size()+1-tab_index_types_geometriques.back());
      tab_index_types_geometriques.push_back(fam->second.size()+1);

      // family name sould not be longer than MED_TAILLE_NOM
      string famName = fam->first;
      if ( famName.size() > MED_NAME_SIZE ) {
        // try to cut off "FAM_" from the head
        if ( famName.size() - 4 <= MED_NAME_SIZE ) {
          famName = famName.substr(4);
        }
        else { // try to make a unique name by cutting off char by char from the tail
          famName = famName.substr(0, MED_NAME_SIZE);
          map< string,vector<int> >::iterator foundName = tab_families.find( famName );
          while ( !famName.empty() &&
                  ( foundName != tab_families.end() || famName[ famName.size()-1 ] == ' ' ))
          {
            famName = famName.substr( 0, famName.size() - 1 );
            foundName = tab_families.find( famName );
          }
        }
        tab_families[ famName ]; // add a new name in the table to assure uniqueness
      }

      // create a MED FAMILY and fill it with the tabs we constructed
      FAMILY* newFam = new FAMILY();
      newFam->setName(famName);
      newFam->setMesh(this);
      removeReference();
      newFam->setNumberOfGeometricType(tab_types_geometriques.size());
      newFam->setGeometricType(&tab_types_geometriques[0]); // we know the tab is not empy
      newFam->setNumberOfElements(&tab_nombres_elements[0]);
      newFam->setNumber(&tab_index_types_geometriques[0],&fam->second[0]);
      newFam->setEntity(entity);
      newFam->setAll( getNumberOfElements( entity, MED_ALL_ELEMENTS ) == fam->second.size() );

      int idFam = 0;

      switch ( entity )
      {
      case MED_EN::MED_NODE :
        ++idFamNode;
        idFam = idFamNode;
        break;
      case MED_EN::MED_CELL :
      case MED_EN::MED_FACE :
      case MED_EN::MED_EDGE :
        ++idFamElement;
        idFam = -idFamElement;
        break;
      }

      newFam->setIdentifier(idFam);

      // Update links between families and groups

      int ncell1=fam->second[0]-1;  // number of first cell in family
      int numberOfGroups=tab_cell[ncell1].groups.size(); // number of groups in the family
      if(numberOfGroups)
      {
        newFam->setNumberOfGroups(numberOfGroups);
        string * groupNames=new string[numberOfGroups];

        // iterate on the groups the family belongs to
        vector<int>::const_iterator it=tab_cell[ncell1].groups.begin();
        for(int ng=0 ; it!=tab_cell[ncell1].groups.end(); ++it, ++ng)
        {
          whichFamilyInGroup[*it].push_back(newFam);
          groupNames[ng]=myGroups[*it]->getName();
        }
        newFam->setGroupsNames(groupNames);
        delete [] groupNames;
      }

      MESSAGE_MED("  MESH::createFamilies() entity " << entity <<
                  " size " << myFamilies->size());

      myFamilies->push_back(newFam);
    }

    // delete old families
    for (unsigned int i=0;i<myOldFamilies.size();i++)
      myOldFamilies[i]->removeReference();

    // update references in groups
    for (unsigned int i=0;i<myGroups.size();i++)
    {
      myGroups[i]->setNumberOfFamilies(whichFamilyInGroup[i].size());
      myGroups[i]->setFamilies(whichFamilyInGroup[i]);
    }

    // re-scan the cells vector, and fill the family vector with cells.
    // creation of support, check if it already exist.
  }
}

//================================================================================
/*!
  Create groups from families.

  It is used to create groups that have only one family
  for meshes that come from codes that use families instead
  of groups to define a subregion.
*/
//================================================================================

void GMESH::createGroups()
{
  for (medEntityMesh entity=MED_EN::MED_CELL; entity!=MED_ALL_ENTITIES; ++entity)
  {
    // make myFamilies points to the member corresponding to entity
    vector<FAMILY*> *myFamilies= NULL;
    vector<GROUP*> *myGroups= NULL;
    switch ( entity )
    {
    case MED_EN::MED_CELL :
      myFamilies = & _familyCell;
      myGroups = & _groupCell;
      break;
    case MED_EN::MED_FACE :
      myFamilies = & _familyFace;
      myGroups = & _groupFace;
      break;
    case MED_EN::MED_EDGE :
      myFamilies = & _familyEdge;
      myGroups = & _groupEdge;
      break;
    case MED_EN::MED_NODE :
      myFamilies = & _familyNode;
      myGroups = & _groupNode;
      break;
    }


    for (unsigned i=0; i< myFamilies->size(); i++)
    {
      list <FAMILY*> fam_list;
      fam_list.push_back((*myFamilies)[i]);
      //creates a group with the family name and only one family
      GROUP* group=new GROUP((*myFamilies)[i]->getName(),fam_list);
      (*myGroups).push_back(group);
      removeReference();
    }
  }
}
