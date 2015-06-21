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

/*
 File Mesh.cxx
*/

#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Mesh.hxx"

#include "MEDMEM_Support.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_CellModel.hxx"
#include "VolSurfFormulae.hxx"
#include "MEDMEM_DriverFactory.hxx"

#include "PointLocator.hxx"

#include <cmath>
#include <map>
#include <sstream>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

#define MED_NOPDT -1
#define MED_NONOR -1

// Block defining groups for the MEDMEM_ug documentation
/*!
\defgroup MESH_constructors MESH Constructors

The MESH class provides only two constructors : a copy constructor and
a constructor enabling creation from file reading. The creation of
a user-defined mesh implies the use of the MESHING class.

\defgroup MESH_advanced MESH Advanced features
These functions provide access to high-level manipulation of the meshes, giving
information about the cells or extracting supports from the mesh.

\defgroup MESH_connectivity MESH Connectivity information
These methods are related to the extraction of connectivity information
from the mesh.

\defgroup MESH_nodes MESH Nodes information
These methods are related to the extraction of information about the mesh nodes.

*/

void MESH::init()
{
  const char* LOC = "MESH::init(): ";
  BEGIN_OF_MED(LOC);

  GMESH::init();

  delete _coordinate;
  _coordinate   = (COORDINATE   *) NULL;
  delete _connectivity;
  _connectivity = (CONNECTIVITY *) NULL;

  _numberOfNodes = MED_INVALID;

  _arePresentOptionnalNodesNumbers = 0;

  END_OF_MED(LOC);
}

/*! Create an empty MESH. */
MESH::MESH():GMESH(),_coordinate(NULL),_connectivity(NULL)
{
  init();
}

/*! \if MEDMEM_ug
  \addtogroup MESH_constructors
  @{
\endif
*/
/*!
  Copy constructor
*/
MESH::MESH(MESH &m): GMESH(m)
{
  if (m._coordinate != NULL)
    _coordinate = new COORDINATE(* m._coordinate);
  else
    _coordinate = (COORDINATE *) NULL;

  if (m._connectivity != NULL)
    _connectivity = new CONNECTIVITY(* m._connectivity);
  else
    _connectivity = (CONNECTIVITY *) NULL;

  _numberOfNodes = m._numberOfNodes;

  _arePresentOptionnalNodesNumbers = m._arePresentOptionnalNodesNumbers;
  _optionnalToCanonicNodesNumbers = m._optionnalToCanonicNodesNumbers;
}

/*!
\if MEDMEM_ug
@}
\endif
*/

MESH::~MESH()
{
  MESSAGE_MED("MESH::~MESH() : Destroying the Mesh");

  if (_coordinate != ((COORDINATE *) NULL)) delete _coordinate ;
  if (_connectivity != ((CONNECTIVITY *) NULL)) delete _connectivity ;
  _coordinate = 0;
  _connectivity = NULL;
}

MESH & MESH::operator=(const MESH &m)
{
  const char* LOC = "MESH & MESH::operator=(const MESH &m) : ";
  BEGIN_OF_MED(LOC);

  _name = m._name;
  _description = m._description;

  if ( _coordinate   ) delete _coordinate;
  _coordinate   = m._coordinate   ? new COORDINATE  ( *m._coordinate   ) : 0;
  if ( _connectivity ) delete _connectivity;
  _connectivity = m._connectivity ? new CONNECTIVITY( *m._connectivity ) : 0;

  _spaceDimension = m._spaceDimension;
  _numberOfNodes  = m._numberOfNodes;

  _arePresentOptionnalNodesNumbers = m._arePresentOptionnalNodesNumbers;
  _optionnalToCanonicNodesNumbers  = m._optionnalToCanonicNodesNumbers;

  vector<FAMILY*>*        fams[4] = { &_familyNode, &_familyCell, &_familyFace, &_familyEdge};
  const vector<FAMILY*>* mfams[4] = { &m._familyNode,&m._familyCell,&m._familyFace,&m._familyEdge };
  for ( int i = 0; i < 4; ++i )
  {
    for( size_t f = 0; f < fams[i]->size(); ++f )
      fams[i]->at(f)->removeReference();
    fams[i]->clear();
    fams[i]->reserve( mfams[i]->size() );
    for ( size_t f = 0; f < mfams[i]->size(); ++f )
    {
      if ( mfams[i]->at(f) )
      {
        fams[i]->push_back( new FAMILY( *mfams[i]->at(f) ));
        fams[i]->back()->setMesh( this );
      }
    }
  }
  vector<GROUP*>*        groups[4] = { &_groupNode, &_groupCell, &_groupFace, &_groupEdge };
  const vector<GROUP*>* mgroups[4] = { &m._groupNode, &m._groupCell, &m._groupFace, &m._groupEdge };
  for ( int i = 0; i < 4; ++i )
  {
    for (size_t g = 0; g < groups[i]->size(); ++g )
      groups[i]->at(g)->removeReference();
    groups[i]->clear();
    groups[i]->reserve( mgroups[i]->size() );
    for ( size_t g = 0; g < mgroups[i]->size(); ++g )
    {
      if ( mgroups[i]->at(g) )
      {
        groups[i]->push_back( new GROUP( *mgroups[i]->at(g) ));
        groups[i]->back()->setMesh( this );
      }
    }
  }

  for ( size_t drv = 0; drv < _drivers.size(); ++drv )
    delete _drivers[drv];
  _drivers.clear();
  _drivers.reserve( m._drivers.size());
  for ( size_t drv = 0; drv < m._drivers.size(); ++drv )
    if ( m._drivers[drv] )
      _drivers.push_back( m._drivers[drv]->copy() );

  return *this;
}

bool MESH::operator==(const MESH& other) const
{
  const char LOC[] = "MESH::operator==";
  BEGIN_OF_MED(LOC);
  return this==&other;
}

/*!\if MEDMEM_ug
\addtogroup MESH_constructors
@{
\endif
*/
/*!
  Creates a %MESH object using a %MESH driver of type %driverTypes (MED_DRIVER, GIBI_DRIVER, ...) associated with file \a fileName. As several meshes can coexist in the same file (notably in MED files) , the constructor takes a third argument that specifies the name of the mesh.
  The constructor will throw an exception if the file does not exist, has no reading permissions or if the mesh does not exist in the file.
*/
MESH::MESH(driverTypes driverType, const string &  fileName/*=""*/, const string &  driverName/*=""*/) throw (MEDEXCEPTION):_coordinate(0),_connectivity(NULL)
{
  const char * LOC = "MESH::MESH(driverTypes driverType, const string &  fileName="", const string &  driverName/="") : ";
  BEGIN_OF_MED(LOC);

  int current;
  init();
  GENDRIVER *myDriver=DRIVERFACTORY::buildDriverForMesh(driverType,fileName,this,driverName,RDONLY);
  current = addDriver(*myDriver);
  delete myDriver;
  try
  {
    _drivers[current]->open();
    _drivers[current]->read();
    _drivers[current]->close();
  }
  catch ( MED_EXCEPTION& e )
  {
    if ( _drivers[current] )
      _drivers[current]->close(), delete _drivers[current];
    _drivers[current] = 0;
    throw e;
  }

  END_OF_MED(LOC);
}
/*!\if MEDMEM_ug
  @}
\endif
*/

/*!
  Returns true if mesh \a other has same
  coordinates (to 1E-15 precision ) and same connectivity as the calling object.
  Information like name or description is not taken into account
  for the comparison.
*/

bool MESH::deepCompare(const GMESH& gother) const
{
  if ( gother.getIsAGrid() != getIsAGrid())
    return false;
  const MESH& other = static_cast<const MESH&>( gother );

  int size1=getSpaceDimension()*getNumberOfNodes();
  int size2=other.getSpaceDimension()*other.getNumberOfNodes();
  if(size1!=size2)
    return false;

  const COORDINATE* CRD = other.getCoordinateptr();
  if( (!CRD && _coordinate) || (CRD && !(_coordinate)) ) {
    return false;
  }

  bool ret=true;
  if( _coordinate ) {
    const double* coord1=getCoordinates(MED_EN::MED_FULL_INTERLACE);
    const double* coord2=other.getCoordinates(MED_EN::MED_FULL_INTERLACE);
    for(int i=0;i<size1 && ret;i++) {
      ret=(fabs(coord1[i]-coord2[i])<1e-15);
    }
  }
  if(ret) {
    const CONNECTIVITY* CNV = other.getConnectivityptr();
    if( (!CNV && _connectivity) || (CNV && !(_connectivity)) ) {
      return false;
    }
    if(_connectivity) {
      return _connectivity->deepCompare(*other._connectivity);
    }
  }
  return ret;
}

/*!
 * \brief print my contents
 */
ostream & ::MEDMEM::operator<<(ostream &os, const MESH &myMesh)
{
  myMesh.printMySelf(os);
  return os;
}
void MESH::printMySelf(ostream &os) const
{
  const MESH &myMesh = *this;
  int spacedimension = myMesh.getSpaceDimension();
  int meshdimension  = myMesh.getMeshDimension();
  int numberofnodes  = myMesh.getNumberOfNodes();

  if ( spacedimension == MED_INVALID ) {
    os << " Empty mesh ...";
    return;
  }

  os << "Space Dimension : " << spacedimension << endl << endl;

  os << "Mesh Dimension : " << meshdimension << endl << endl;

  if(myMesh.getCoordinateptr()) {
    const double * coordinates = myMesh.getCoordinates(MED_EN::MED_FULL_INTERLACE);

    os << "SHOW NODES COORDINATES : " << endl;
    os << "Name :" << endl;
    const string * coordinatesnames = myMesh.getCoordinatesNames();
    if ( coordinatesnames ) {
      for(int i=0; i<spacedimension ; i++)
      {
        os << " - " << coordinatesnames[i] << endl;
      }
    }
    os << "Unit :" << endl;
    const string * coordinatesunits = myMesh.getCoordinatesUnits();
    if ( coordinatesunits ) {
      for(int i=0; i<spacedimension ; i++)
      {
        os << " - " << coordinatesunits[i] << endl;
      }
    }
    for(int i=0; i<numberofnodes ; i++)
    {
      os << "Nodes " << i+1 << " : ";
      for (int j=0; j<spacedimension ; j++)
        os << coordinates[i*spacedimension+j] << " ";
      os << endl;
    }
  }

  if(myMesh.getConnectivityptr()) {
    os << endl << "SHOW CONNECTIVITY  :" << endl;
    os << *myMesh._connectivity << endl;
  }

  medEntityMesh entity;
  os << endl << "SHOW FAMILIES :" << endl << endl;
  for (int k=1; k<=4; k++)
  {
    if (k==1) entity = MED_EN::MED_NODE;
    if (k==2) entity = MED_EN::MED_CELL;
    if (k==3) entity = MED_EN::MED_FACE;
    if (k==4) entity = MED_EN::MED_EDGE;
    int numberoffamilies = myMesh.getNumberOfFamilies(entity);
    os << "NumberOfFamilies on "<< entNames[entity] <<" : "<<numberoffamilies<<endl;
    using namespace MED_EN;
    for (int i=1; i<numberoffamilies+1;i++)
    {
      os << * myMesh.getFamily(entity,i) << endl;
    }
  }

  os << endl << "SHOW GROUPS :" << endl << endl;
  for (int k=1; k<=4; k++)
  {
    if (k==1) entity = MED_EN::MED_NODE;
    if (k==2) entity = MED_EN::MED_CELL;
    if (k==3) entity = MED_EN::MED_FACE;
    if (k==4) entity = MED_EN::MED_EDGE;
    int numberofgroups = myMesh.getNumberOfGroups(entity);
    os << "NumberOfGroups on "<< entNames[entity] <<" : "<<numberofgroups<<endl;
    using namespace MED_EN;
    for (int i=1; i<numberofgroups+1;i++)
    {
      os << * myMesh.getGroup(entity,i) << endl;
    }
  }
}

/*! \if MEDMEM_ug
\addtogroup MESH_general
@{
\endif
*/
/*! Gets the dimension of the mesh (2 for 2D- and 3D-surfaces, 3 for volumes). */
int MESH::getMeshDimension() const
{
  int dim = -1;
  if ( _connectivity )
    for ( int i = 0; i < _connectivity->getNumberOfTypes(MED_EN::MED_CELL); ++i )
      if ( _connectivity->getCellsTypes(MED_EN::MED_CELL)[i].getDimension() > dim )
        dim = _connectivity->getCellsTypes(MED_EN::MED_CELL)[i].getDimension();
  return dim;
}
/*! \if MEDMEM_ug @} \endif */

/*!
  Get global number of element which have same connectivity than connectivity argument.

  It do not take care of connectivity order (3,4,7,10 is same as 7,3,10,4).

  Return -1 if not found.
*/
int MESH::getElementNumber(MED_EN::medConnectivity ConnectivityType,
                           MED_EN::medEntityMesh Entity,
                           MED_EN::medGeometryElement Type,
                           int * connectivity) const
{
  const char* LOC="MESH::getElementNumber " ;
  BEGIN_OF_MED(LOC) ;

  int numberOfValue ; // size of connectivity array
  CELLMODEL myType(Type) ;
  if (ConnectivityType==MED_EN::MED_DESCENDING)
    numberOfValue = myType.getNumberOfConstituents(1) ; // faces (3D) or edges (2D)
  else
    numberOfValue = myType.getNumberOfNodes() ; // nodes

  const int * myReverseConnectivityValue = getReverseConnectivity(ConnectivityType,Entity) ;
  const int * myReverseConnectivityIndex = getReverseConnectivityIndex(ConnectivityType,Entity) ;

  // First node or face/edge
  int indexBegin = myReverseConnectivityIndex[connectivity[0]-1] ;
  int indexEnd = myReverseConnectivityIndex[connectivity[0]] ;

  // list to put cells numbers
  list<int> cellsList ;
  list<int>::iterator itList ;
  for (int i=indexBegin; i<indexEnd; i++)
    cellsList.push_back(myReverseConnectivityValue[i-1]) ;

  // Foreach node or face/edge in cell, we search cells which are in common.
  // At the end, we must have only one !

  for (int i=1; i<numberOfValue; i++) {
    int connectivity_i = connectivity[i] ;
    for (itList=cellsList.begin();itList!=cellsList.end();/*itList++*/) {
      bool find = false ;
      for (int j=myReverseConnectivityIndex[connectivity_i-1]; j<myReverseConnectivityIndex[connectivity_i]; j++) {
        if ((*itList)==myReverseConnectivityValue[j-1]) {
          find=true ;
          break ;
        }
      }
      if (!find)
        itList=cellsList.erase(itList++);
      else
        itList++;
    }
  }

  if (cellsList.size()>1) // we have more than one cell
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Found more than one element !")) ;

  if (cellsList.size()==0)
    return -1;

  END_OF_MED(LOC);

  return cellsList.front() ;
}

/*!
\addtogroup MESH_advanced
@{
The methods described in this section are algorithms that perform a computation
and return a result in the form of a SUPPORT or a FIELD to the user. For large meshes,
as the returned information is not stored in the mesh but is rather computed, the
computation time can be large.
*/

/*!
  Returns a support which reference all elements on the boundary of mesh.
  For a d-dimensional mesh, a boundary element is defined as a d-1 dimension
  element that is referenced by only one element in the full descending connectivity.

  This method can also return the list of nodes that belong to the boundary elements.

  WARNING: This method can recalculate descending connectivity from partial to full form,
  so that partial SUPPORT on d-1 dimension elements becomes invalid.

  \param Entity entity on which the boundary is desired. It has to be either \a MED_EN::MED_NODE or the
  d-1 dimension entity type (MED_EN::MED_FACE in 3D, MED_EN::MED_EDGE in 2D).
*/
SUPPORT * MESH::getBoundaryElements(MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION)
{
  const char LOC[]= "MESH::getBoundaryElements : " ;
  BEGIN_OF_MED(LOC) ;
  // some test :
  // actually we could only get face (in 3D) and edge (in 2D)
  medEntityMesh entityToParse=Entity;
  if(_spaceDimension == 3)
    if (Entity != MED_EN::MED_FACE)
      {
        if(Entity==MED_EN::MED_NODE)
          entityToParse=MED_EN::MED_FACE;
        else
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 3D mesh for entity "<<Entity<<" !"));
      }
  if(_spaceDimension == 2)
    if(Entity != MED_EN::MED_EDGE)
      {
        if(Entity==MED_EN::MED_NODE)
          entityToParse=MED_EN::MED_EDGE;
        else
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 2D mesh for entity "<<Entity<<" !"));
      }

  // assure that descending connectivity is full
  if ( !_connectivity )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<< "no connectivity defined in MESH !"));
  _connectivity->calculateFullDescendingConnectivity(MED_EN::MED_CELL);

  const int * myConnectivityValue = getReverseConnectivity(MED_EN::MED_DESCENDING) ;
  const int * myConnectivityIndex = getReverseConnectivityIndex(MED_EN::MED_DESCENDING) ;
  int numberOf = getNumberOfElements(entityToParse,MED_ALL_ELEMENTS) ;
  list<int> myElementsList;

  for (int i=0 ; i<numberOf; i++)
    if (myConnectivityValue[myConnectivityIndex[i]] == 0)
      myElementsList.push_back(i+1);

  if ( myElementsList.empty() && numberOf != 0 )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No boundary elements found by reverse descending connectivity for entity "<<Entity<<" !"));

  if(Entity==MED_EN::MED_NODE)
    return buildSupportOnNodeFromElementList(myElementsList,entityToParse);
  else
    return buildSupportOnElementsFromElementList(myElementsList,entityToParse);
}
/*!
  @}
*/

/*!
  Method that do the same thing as buildSupportOnNodeFromElementList except that a SUPPORT is not created.
*/
void MESH::fillSupportOnNodeFromElementList(const list<int>& listOfElt, SUPPORT *supportToFill) const throw (MEDEXCEPTION)
{
  MED_EN::medEntityMesh entity=supportToFill->getEntity();
  supportToFill->setAll(false);
  supportToFill->setMesh((MESH *)this);

  set<int> nodes;
  if ( entity == MED_EN::MED_NODE )
    {
      supportToFill->fillFromNodeList(listOfElt);
    }
  else
    {
      int lgth;
      for(list<int>::const_iterator iter=listOfElt.begin();iter!=listOfElt.end();iter++)
        {
          const int *conn=_connectivity->getConnectivityOfAnElement(MED_EN::MED_NODAL,entity,*iter,lgth);
          nodes.insert( conn, conn+lgth );
        }
      list<int> nodesList( nodes.begin(), nodes.end() );
      supportToFill->fillFromNodeList( nodesList );
    }
}

/*!
  Method created to factorize code. This method creates a new support on NODE (to deallocate) containing all the nodes id contained in elements 'listOfElt' of
  entity 'entity'.
*/
SUPPORT *MESH::buildSupportOnNodeFromElementList(const list<int>& listOfElt,MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION)
{
  SUPPORT * mySupport = new SUPPORT;
  mySupport->setMesh((MESH *)this);
  mySupport->setName("Boundary");
  mySupport->setEntity( entity );
  fillSupportOnNodeFromElementList(listOfElt,mySupport);
  return mySupport;
}

/*!
\addtogroup MESH_advanced
@{
*/
/*! Retrieves the volume of all the elements contained in \a Support. This method returns
  a FIELD structure based on this support. It only works on MED_EN::MED_CELL for 3D meshes.
*/
FIELD<double, FullInterlace>* MESH::getVolume(const SUPPORT *Support, bool isAbs) const throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getVolume(SUPPORT*) : ";
  BEGIN_OF_MED(LOC);

  // Support must be on 3D elements

  // Make sure that the MESH class is the same as the MESH class attribut
  // in the class Support
  const GMESH* myMesh = Support->getMesh();
  if (this != myMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh !"));

  int dim_space = getSpaceDimension();
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  int nb_type;
  const medGeometryElement* types;
  int nb_entity_type;
  // !!!! WARNING : use of nodal global numbering in the mesh !!!!
  const int *global_connectivity= NULL;
  nb_type = Support->getNumberOfTypes();
  int length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  types = Support->getTypes();
  int index;
  FIELD<double, FullInterlace>* Volume =
    new FIELD<double, FullInterlace>(Support,1);
  Volume->setName("VOLUME");
  Volume->setDescription("cells volume");
  Volume->setComponentName(1,"volume");
  Volume->setComponentDescription(1,"desc-comp");

  string MEDComponentUnit = myMesh->getCoordinatesUnits()[0]+"*"+myMesh->getCoordinatesUnits()[1]+"*"+myMesh->getCoordinatesUnits()[2];

  Volume->setMEDComponentUnit(1,MEDComponentUnit);

  Volume->setIterationNumber(0);
  Volume->setOrderNumber(0);
  Volume->setTime(0.0);

  typedef  MEDMEM_ArrayInterface<double,FullInterlace,NoGauss>::Array ArrayNoGauss;
  ArrayNoGauss  *volume = Volume->getArrayNoGauss();

  index = 1;
  const double * coord = getCoordinates(MED_EN::MED_FULL_INTERLACE);

  for (int i=0;i<nb_type;i++)
  {
    medGeometryElement type = types[i] ;
    double xvolume;
    nb_entity_type = Support->getNumberOfElements(type);
    if(type != MED_EN::MED_POLYHEDRA)
    {
      if (onAll)
      {
        global_connectivity = getConnectivity(MED_EN::MED_NODAL,support_entity,type);
      }
      else
      {
        const int * supp_number = Support->getNumber(type);
        const int * connectivity = getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
        const int * connectivityIndex = getConnectivityIndex(MED_EN::MED_NODAL,support_entity);
        int * global_connectivity_tmp = new int[(type%100)*nb_entity_type];

        for (int k_type = 0; k_type<nb_entity_type; k_type++) {
          for (int j_ent = 0; j_ent<(type%100); j_ent++) {
            global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[connectivityIndex[supp_number[k_type]-1]+j_ent-1];
          }
        }
        global_connectivity = global_connectivity_tmp ;
      }
    }

    switch (type)
    {
    case MED_EN::MED_TETRA4 : case MED_EN::MED_TETRA10 :
      {
        for (int tetra=0;tetra<nb_entity_type;tetra++)
        {
          int tetra_index = (type%100)*tetra;
          int N1 = global_connectivity[tetra_index]-1;
          int N2 = global_connectivity[tetra_index+1]-1;
          int N3 = global_connectivity[tetra_index+2]-1;
          int N4 = global_connectivity[tetra_index+3]-1;
          xvolume=INTERP_KERNEL::calculateVolumeForTetra(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,coord+dim_space*N4);
          if(isAbs)
            xvolume=fabs(xvolume);
          volume->setIJ(index,1,xvolume) ;
          index++;
        }
        break;
      }
    case MED_EN::MED_PYRA5 : case MED_EN::MED_PYRA13 :
      {
        for (int pyra=0;pyra<nb_entity_type;pyra++)
        {
          int pyra_index = (type%100)*pyra;
          int N1 = global_connectivity[pyra_index]-1;
          int N2 = global_connectivity[pyra_index+1]-1;
          int N3 = global_connectivity[pyra_index+2]-1;
          int N4 = global_connectivity[pyra_index+3]-1;
          int N5 = global_connectivity[pyra_index+4]-1;
          xvolume=INTERP_KERNEL::calculateVolumeForPyra(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,coord+dim_space*N4,coord+dim_space*N5);
          if(isAbs)
            xvolume=fabs(xvolume);
          volume->setIJ(index,1,xvolume) ;
          index++;
        }
        break;
      }
    case MED_EN::MED_PENTA6 : case MED_EN::MED_PENTA15 :
      {
        for (int penta=0;penta<nb_entity_type;penta++)
        {
          int penta_index = (type%100)*penta;
          int N1 = global_connectivity[penta_index]-1;
          int N2 = global_connectivity[penta_index+1]-1;
          int N3 = global_connectivity[penta_index+2]-1;
          int N4 = global_connectivity[penta_index+3]-1;
          int N5 = global_connectivity[penta_index+4]-1;
          int N6 = global_connectivity[penta_index+5]-1;
          xvolume=INTERP_KERNEL::calculateVolumeForPenta(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,coord+dim_space*N4,coord+dim_space*N5,coord+dim_space*N6);
          if(isAbs)
            xvolume=fabs(xvolume);
          volume->setIJ(index,1,xvolume) ;
          index++;
        }
        break;
      }
    case MED_EN::MED_HEXA8 : case MED_EN::MED_HEXA20 :
      {
        for (int hexa=0;hexa<nb_entity_type;hexa++)
        {
          int hexa_index = (type%100)*hexa;

          int N1 = global_connectivity[hexa_index]-1;
          int N2 = global_connectivity[hexa_index+1]-1;
          int N3 = global_connectivity[hexa_index+2]-1;
          int N4 = global_connectivity[hexa_index+3]-1;
          int N5 = global_connectivity[hexa_index+4]-1;
          int N6 = global_connectivity[hexa_index+5]-1;
          int N7 = global_connectivity[hexa_index+6]-1;
          int N8 = global_connectivity[hexa_index+7]-1;
          xvolume=INTERP_KERNEL::calculateVolumeForHexa(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,coord+dim_space*N4,coord+dim_space*N5,coord+dim_space*N6,coord+dim_space*N7,coord+dim_space*N8);
          if(isAbs)
            xvolume=fabs(xvolume);
          volume->setIJ(index,1,xvolume) ;
          index++;
        }
        break;
      }
    case MED_POLYHEDRA:
      {
        double bary[3];
        if(onAll)
        {
          for (int polyhs=0;polyhs<nb_entity_type;polyhs++)
          {
            int lgthNodes,iPts,iFaces,iPtsInFace;
            int offsetWithClassicType=getGlobalNumberingIndex(support_entity)[i]-1;
            int *nodes=_connectivity->getNodesOfPolyhedron(offsetWithClassicType+polyhs+1,lgthNodes);
            int nbOfFaces,*nbOfNodesPerFaces;
            int **nodes1=_connectivity->getNodesPerFaceOfPolyhedron(offsetWithClassicType+polyhs+1,nbOfFaces,nbOfNodesPerFaces);
            double **pts=new double * [lgthNodes];
            double ***pts1=new double ** [nbOfFaces];
            for(iPts=0;iPts<lgthNodes;iPts++)
              pts[iPts]=(double *)(coord+3*(nodes[iPts]-1));
            for(iFaces=0;iFaces<nbOfFaces;iFaces++)
            {
              pts1[iFaces]=new double* [nbOfNodesPerFaces[iFaces]];
              for(iPtsInFace=0;iPtsInFace<nbOfNodesPerFaces[iFaces];iPtsInFace++)
                pts1[iFaces][iPtsInFace]=(double *)(coord+3*(nodes1[iFaces][iPtsInFace]-1));
            }
            delete [] nodes1;
            INTERP_KERNEL::calculateBarycenterDyn((const double **)pts,lgthNodes,3,bary);
            delete [] nodes;
            delete [] pts;
            if(isAbs)
              xvolume=INTERP_KERNEL::calculateVolumeForPolyhAbs((const double ***)pts1,nbOfNodesPerFaces,nbOfFaces,bary);
            else
              xvolume=INTERP_KERNEL::calculateVolumeForPolyh((const double ***)pts1,nbOfNodesPerFaces,nbOfFaces,bary);
            delete [] nbOfNodesPerFaces;
            for(iFaces=0;iFaces<nbOfFaces;iFaces++)
              delete [] pts1[iFaces];
            delete [] pts1;
            volume->setIJ(index,1,xvolume) ;
            index++;
          }
        }
        else
        {
          const int * supp_number = Support->getNumber(MED_EN::MED_POLYHEDRA);
          for (int polyhs=0;polyhs<nb_entity_type;polyhs++)
          {
            int lgthNodes,iPts,iFaces,iPtsInFace;
            int *nodes=_connectivity->getNodesOfPolyhedron(supp_number[polyhs],lgthNodes);
            int nbOfFaces,*nbOfNodesPerFaces;
            int **nodes1=_connectivity->getNodesPerFaceOfPolyhedron(supp_number[polyhs],nbOfFaces,nbOfNodesPerFaces);
            double **pts=new double * [lgthNodes];
            double ***pts1=new double ** [nbOfFaces];
            for(iPts=0;iPts<lgthNodes;iPts++)
              pts[iPts]=(double *)(coord+3*(nodes[iPts]-1));
            for(iFaces=0;iFaces<nbOfFaces;iFaces++)
            {
              pts1[iFaces]=new double* [nbOfNodesPerFaces[iFaces]];
              for(iPtsInFace=0;iPtsInFace<nbOfNodesPerFaces[iFaces];iPtsInFace++)
                pts1[iFaces][iPtsInFace]=(double *)(coord+3*(nodes1[iFaces][iPtsInFace]-1));
            }
            delete [] nodes1;
            INTERP_KERNEL::calculateBarycenterDyn((const double **)pts,lgthNodes,3,bary);
            delete [] nodes;
            delete [] pts;
            if(isAbs)
              xvolume=INTERP_KERNEL::calculateVolumeForPolyhAbs((const double ***)pts1,nbOfNodesPerFaces,nbOfFaces,bary);
            else
              xvolume=INTERP_KERNEL::calculateVolumeForPolyh((const double ***)pts1,nbOfNodesPerFaces,nbOfFaces,bary);
            delete [] nbOfNodesPerFaces;
            for(iFaces=0;iFaces<nbOfFaces;iFaces++)
              delete [] pts1[iFaces];
            delete [] pts1;
            volume->setIJ(index,1,xvolume) ;
            index++;
          }
        }
        break;
      }
    default :
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Volumes on it !"));
      break;
    }

    if (!onAll && type!=MED_EN::MED_POLYHEDRA)
      delete [] global_connectivity ;
  }

  return Volume;
}
/*! Retrieves the area of all the elements contained in \a Support. This method returns
  a FIELD structure based on this support. It only works on MED_EN::MED_CELL for 2D meshes or MED_EN::MED_FACE
  for 3D meshes.
*/

FIELD<double, FullInterlace>* MESH::getArea(const SUPPORT * Support) const throw (MEDEXCEPTION)
{
  const char LOC[]= "MESH::getArea(SUPPORT*) : ";
  BEGIN_OF_MED(LOC);

  // Support must be on 2D elements

  // Make sure that the MESH class is the same as the MESH class attribut
  // in the class Support
  const GMESH* myMesh = Support->getMesh();
  if (this != myMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh !"));

  int dim_space = getSpaceDimension();
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  int nb_type;
  const medGeometryElement* types;
  int nb_entity_type;
  // !!!! WARNING : use of nodal global numbering in the mesh !!!!
  const int *global_connectivity= NULL;

  nb_type = Support->getNumberOfTypes();
  int length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  types = Support->getTypes();

  int index;
  FIELD<double, FullInterlace>* Area;

  Area = new FIELD<double, FullInterlace>(Support,1);
  Area->setName("AREA");
  Area->setDescription("cells or faces area");

  Area->setComponentName(1,"area");
  Area->setComponentDescription(1,"desc-comp");

  /*  string MEDComponentUnit = myMesh->getCoordinatesUnits()[0]+"*"+myMesh->getCoordinatesUnits()[1];*/

  string MEDComponentUnit="trucmuch";

  Area->setMEDComponentUnit(1,MEDComponentUnit);

  Area->setIterationNumber(0);
  Area->setOrderNumber(0);
  Area->setTime(0.0);

  double *area = (double *)Area->getValue();

  const double * coord = getCoordinates(MED_EN::MED_FULL_INTERLACE);
  index = 0;

  for (int i=0;i<nb_type;i++)
  {
    medGeometryElement type = types[i] ;
    nb_entity_type = Support->getNumberOfElements(type);
    const int *global_connectivityIndex = 0;
    if(type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
    {
      global_connectivityIndex = getConnectivityIndex(MED_EN::MED_NODAL,support_entity);
      if (onAll)
      {
        global_connectivity = getConnectivity(MED_EN::MED_NODAL,support_entity,type);
      }
      else
      {
        const int * supp_number = Support->getNumber(type);
        const int * connectivity = getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
        int * global_connectivity_tmp = new int[(type%100)*nb_entity_type];

        for (int k_type = 0; k_type<nb_entity_type; k_type++) {
          for (int j_ent = 0; j_ent<(type%100); j_ent++) {
            global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[global_connectivityIndex[supp_number[k_type]-1]+j_ent-1];
          }
        }
        global_connectivity = global_connectivity_tmp ;
      }
    }
    switch (type)
    {
    case MED_EN::MED_TRIA3 : case MED_EN::MED_TRIA6 :
      {
        for (int tria=0;tria<nb_entity_type;tria++)
        {
          int tria_index = (type%100)*tria;

          int N1 = global_connectivity[tria_index]-1;
          int N2 = global_connectivity[tria_index+1]-1;
          int N3 = global_connectivity[tria_index+2]-1;

          area[index]=INTERP_KERNEL::calculateAreaForTria(coord+(dim_space*N1),
                                                          coord+(dim_space*N2),
                                                          coord+(dim_space*N3),dim_space);
          index++;
        }
        break;
      }
    case MED_EN::MED_QUAD4 : case MED_EN::MED_QUAD8 :
      {
        for (int quad=0;quad<nb_entity_type;quad++)
        {
          int quad_index = (type%100)*quad;

          int N1 = global_connectivity[quad_index]-1;
          int N2 = global_connectivity[quad_index+1]-1;
          int N3 = global_connectivity[quad_index+2]-1;
          int N4 = global_connectivity[quad_index+3]-1;

          area[index]=INTERP_KERNEL::calculateAreaForQuad(coord+dim_space*N1,
                                                          coord+dim_space*N2,
                                                          coord+dim_space*N3,
                                                          coord+dim_space*N4,dim_space);
          index++;
        }
        break;
      }
    case MED_POLYGON :
      {
        if(onAll)
        {
          int offsetWithClassicType=getGlobalNumberingIndex(support_entity)[i]-1;
          const int * connectivity = _connectivity->getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
          const int * connectivity_index = _connectivity->getConnectivityIndex(MED_EN::MED_NODAL,support_entity) + offsetWithClassicType;
          for (int polygs=0;polygs<nb_entity_type;polygs++)
          {
            int size=connectivity_index[polygs+1]-connectivity_index[polygs];
            double **pts=new double * [size];
            for(int iPts=0;iPts<size;iPts++)
              pts[iPts]=(double *)(coord+dim_space*(connectivity[connectivity_index[polygs]+iPts-1]-1));
            area[index] = INTERP_KERNEL::calculateAreaForPolyg((const double **)pts,size,dim_space);
            delete [] pts;
            index++;
          }
        }
        else
        {
          const int * supp_number = Support->getNumber(MED_EN::MED_POLYGON);
          const int * connectivity = _connectivity->getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
          const int * connectivity_index = _connectivity->getConnectivityIndex(MED_EN::MED_NODAL,support_entity);
          for (int polygs=0;polygs<nb_entity_type;polygs++)
          {
            int size=connectivity_index[supp_number[polygs]]-connectivity_index[supp_number[polygs]-1];
            double **pts=new double * [size];
            for(int iPts=0;iPts<size;iPts++)
              pts[iPts]=(double *)(coord+dim_space*(connectivity[connectivity_index[supp_number[polygs]-1]+iPts-1]-1));
            area[index]=INTERP_KERNEL::calculateAreaForPolyg((const double **)pts,size,dim_space);
            delete [] pts;
            index++;
          }
        }
        break;
      }
    default :
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Areas on it !"));
      break;
    }

    if (!onAll)
      if(type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
        delete [] global_connectivity ;
  }
  return Area;
}
/*! Retrieves the length of all the elements contained in \a Support. This method returns
  a FIELD structure based on this support. It only works on MED_EN::MED_EDGE supports.
*/
FIELD<double, FullInterlace>* MESH::getLength(const SUPPORT * Support) const throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getLength(SUPPORT*) : ";
  BEGIN_OF_MED(LOC);

  // Support must be on 1D elements

  // Make sure that the MESH class is the same as the MESH class attribut
  // in the class Support
  const GMESH* myMesh = Support->getMesh();
  if (this != myMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh !"));

  int dim_space = getSpaceDimension();
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  int nb_type;
  const medGeometryElement* types;
  int nb_entity_type;
  // !!!! WARNING : use of nodal global numbering in the mesh !!!!
  const int* global_connectivity;

  nb_type = Support->getNumberOfTypes();
  int length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  types = Support->getTypes();

  int index;
  FIELD<double, FullInterlace>* Length;

  Length = new FIELD<double, FullInterlace>(Support,1);

  typedef  MEDMEM_ArrayInterface<double,FullInterlace,NoGauss>::Array ArrayNoGauss;
  ArrayNoGauss * length = Length->getArrayNoGauss();

  const double * coord = getCoordinates(MED_EN::MED_FULL_INTERLACE);
  index = 1;

  for (int i=0;i<nb_type;i++)
  {
    medGeometryElement type = types[i] ;
    double xlength;

    if (onAll)
    {
      nb_entity_type = getNumberOfElements(support_entity,type);
      global_connectivity = getConnectivity(MED_EN::MED_NODAL,support_entity,type);
    }
    else
    {
      nb_entity_type = Support->getNumberOfElements(type);
      const int * supp_number = Support->getNumber(type);
      const int * connectivity = getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
      const int * connectivityIndex = getConnectivityIndex(MED_EN::MED_NODAL,support_entity);
      int* global_connectivity_tmp = new int[(type%100)*nb_entity_type];

      for (int k_type = 0; k_type<nb_entity_type; k_type++) {
        for (int j_ent = 0; j_ent<(type%100); j_ent++) {
          global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[connectivityIndex[supp_number[k_type]-1]+j_ent-1];
        }
      }
      global_connectivity = global_connectivity_tmp ;
    }

    switch (type)
    {
    case MED_EN::MED_SEG2 : case MED_EN::MED_SEG3 :
      {
        for (int edge=0;edge<nb_entity_type;edge++)
        {
          int edge_index = (type%100)*edge;

          int N1 = global_connectivity[edge_index]-1;
          int N2 = global_connectivity[edge_index+1]-1;

          double x1 = coord[dim_space*N1];
          double x2 = coord[dim_space*N2];

          double y1 = coord[dim_space*N1+1];
          double y2 = coord[dim_space*N2+1];

          double z1, z2 ; z1 = 0.0 ; z2 = 0.0 ; if (dim_space==3) {
            z1 = coord[dim_space*N1+2]; z2 = coord[dim_space*N2+2];}

          xlength =  sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) +
                          (z1 - z2)*(z1 - z2));

          length->setIJ(index,1,xlength) ;
          index++;
        }
        break;
      }
    default :
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Lengths on it !"));
      break;
    }

    if (!onAll) delete [] global_connectivity ;
  }

  return Length;
}

/*! Retrieves the normal for all elements contained in SUPPORT \a Support.
  The method is only functional for 2D supports for 3D meshes and 1D supports
  for 2D meshes. It returns
  a FIELD for which the number of components is equal to the dimension of the
  mesh and which represents coordinates of the vector normal to the element.
  The direction of the vector is undetermined.
*/
FIELD<double, FullInterlace>* MESH::getNormal(const SUPPORT * Support) const throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getNormal(SUPPORT*) : ";
  BEGIN_OF_MED(LOC);

  // Support must be on 2D or 1D elements

  // Make sure that the MESH class is the same as the MESH class attribut
  // in the class Support
  const GMESH* myMesh = Support->getMesh();
  if (this != myMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh : pointeur problem !"));

  int dim_space = getSpaceDimension();
  int mesh_dim=getMeshDimension();
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  if( support_entity!=MED_EN::MED_EDGE && (mesh_dim!=1 || support_entity!=MED_EN::MED_CELL) && ( mesh_dim!=2 || support_entity!=MED_EN::MED_CELL ) && ( mesh_dim!=3 || support_entity!=MED_EN::MED_FACE ))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"incompatible mesh dimension and entity"));
  int nb_type;
  const medGeometryElement* types;
  int nb_entity_type;
  // !!!! WARNING : use of nodal global numbering in the mesh !!!!
  const int *global_connectivity= NULL;

  nb_type = Support->getNumberOfTypes();
  int length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  types = Support->getTypes();

  int index;

  FIELD<double, FullInterlace>* Normal =
    new FIELD<double, FullInterlace>(Support,dim_space);
  Normal->setName("NORMAL");
  Normal->setDescription("cells or faces normal");
  for (int k=1;k<=dim_space;k++) {
    Normal->setComponentName(k,"normal");
    Normal->setComponentDescription(k,"desc-comp");
    Normal->setMEDComponentUnit(k,"unit");
  }

  Normal->setIterationNumber(MED_NOPDT);
  Normal->setOrderNumber(MED_NONOR);
  Normal->setTime(0.0);
  double *normal = (double *)Normal->getValue();

  const double * coord = getCoordinates(MED_EN::MED_FULL_INTERLACE);
  index = 0;

  for (int i=0;i<nb_type;i++)
  {
    medGeometryElement type = types[i] ;
    nb_entity_type = Support->getNumberOfElements(type);

    // Make sure we trying to get Normals on
    // TRIA3 or TRIA6 or QUAD4 QUAD8 (2D) cells on a 3D mesh
    // or on SEG2 or SEG3 (1D) cells on a 2D mesh

    if ( (((type==MED_EN::MED_TRIA3) || (type==MED_EN::MED_TRIA6) ||
           (type==MED_EN::MED_QUAD4) || (type==MED_EN::MED_QUAD8) || (type==MED_POLYGON)) &&
          (dim_space != 3)) || (((type==MED_EN::MED_SEG2) || (type==MED_EN::MED_SEG3)) &&
                                (dim_space != 2)) )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh : dimension problem !"));
    if(type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
    {
      if (onAll)
      {
        global_connectivity = getConnectivity(MED_EN::MED_NODAL,support_entity,type);
      }
      else
      {
        const int * supp_number = Support->getNumber(type);
        const int * connectivity = getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
        const int * connectivityIndex = getConnectivityIndex(MED_EN::MED_NODAL,support_entity);
        int * global_connectivity_tmp = new int[(type%100)*nb_entity_type];

        for (int k_type = 0; k_type<nb_entity_type; k_type++) {
          for (int j_ent = 0; j_ent<(type%100); j_ent++) {
            global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[connectivityIndex[supp_number[k_type]-1]+j_ent-1];
          }
        }

        global_connectivity = global_connectivity_tmp ;
      }
    }

    switch (type)
    {
    case MED_EN::MED_TRIA3 : case MED_EN::MED_TRIA6 :
      {
        for (int tria=0;tria<nb_entity_type;tria++)
        {
          int tria_index = (type%100)*tria;
          int N1 = global_connectivity[tria_index]-1;
          int N2 = global_connectivity[tria_index+1]-1;
          int N3 = global_connectivity[tria_index+2]-1;
          INTERP_KERNEL::calculateNormalForTria(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,normal+3*index);
          index++;
        }
        break;
      }
    case MED_EN::MED_QUAD4 : case MED_EN::MED_QUAD8 :
      {
        for (int quad=0;quad<nb_entity_type;quad++)
        {
          int quad_index = (type%100)*quad;
          int N1 = global_connectivity[quad_index]-1;
          int N2 = global_connectivity[quad_index+1]-1;
          int N3 = global_connectivity[quad_index+2]-1;
          int N4 = global_connectivity[quad_index+3]-1;
          INTERP_KERNEL::calculateNormalForQuad(coord+dim_space*N1,coord+dim_space*N2,coord+dim_space*N3,coord+dim_space*N4,normal+3*index);
          index++;
        }
        break;
      }
    case MED_EN::MED_SEG2 : case MED_EN::MED_SEG3 :
      {
        double xnormal1, xnormal2;
        for (int edge=0;edge<nb_entity_type;edge++)
        {
          int edge_index = (type%100)*edge;

          int N1 = global_connectivity[edge_index]-1;
          int N2 = global_connectivity[edge_index+1]-1;

          double x1 = coord[dim_space*N1];
          double x2 = coord[dim_space*N2];

          double y1 = coord[dim_space*N1+1];
          double y2 = coord[dim_space*N2+1];

          xnormal1 = -(y2-y1);
          xnormal2 = x2-x1;

          normal[2*index] = xnormal1 ;
          normal[2*index+1] = xnormal2 ;

          index++;
        }
        break;
      }
    case MED_POLYGON :
      {
        if(onAll)
        {
          int offsetWithClassicType=getGlobalNumberingIndex(support_entity)[i]-1;
          const int * connectivity = _connectivity->getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
          const int * connectivity_index = _connectivity->getConnectivityIndex(MED_EN::MED_NODAL,support_entity) + offsetWithClassicType;
          for (int polygs=0;polygs<nb_entity_type;polygs++)
          {
            int size=connectivity_index[polygs+1]-connectivity_index[polygs];
            double **pts=new double * [size];
            for(int iPts=0;iPts<size;iPts++)
              pts[iPts]=(double *)(coord+dim_space*(connectivity[connectivity_index[polygs]+iPts-1])-1);
            INTERP_KERNEL::calculateNormalForPolyg((const double **)pts,size,normal+3*index);
            delete [] pts;
            index++;
          }
        }
        else
        {
          const int * supp_number = Support->getNumber(MED_EN::MED_POLYGON);
          const int * connectivity = _connectivity->getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
          const int * connectivity_index = _connectivity->getConnectivityIndex(MED_EN::MED_NODAL,support_entity);
          for (int polygs=0;polygs<nb_entity_type;polygs++)
          {
            int size=connectivity_index[supp_number[polygs]]-connectivity_index[supp_number[polygs]-1];
            double **pts=new double * [size];
            for(int iPts=0;iPts<size;iPts++)
              pts[iPts]=(double *)(coord+dim_space*(connectivity[connectivity_index[supp_number[polygs]-1]+iPts-1])-1);
            INTERP_KERNEL::calculateNormalForPolyg((const double **)pts,size,normal+3*index);
            delete [] pts;
            index++;
          }
        }
        break;
      }
    default :
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get Normals on it !"));
      break;
    }
    if (!onAll && type!=MED_EN::MED_POLYGON)
      delete [] global_connectivity ;
  }
  END_OF_MED(LOC);

  return Normal;
}
/*!Returns the barycenter for each element in the support. The barycenter positions are returned
  as a field with a number of components equal to the mesh dimension.
  The barycenter computed by this method is actually the barycenter of the set of nodes of the elements, each having the same weight.
*/
FIELD<double, FullInterlace>* MESH::getBarycenter(const SUPPORT * Support) const throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getBarycenter(SUPPORT*) : ";
  const GMESH* myMesh = Support->getMesh();
  if (this != myMesh)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"no compatibility between *this and SUPPORT::_mesh !"));

  int dim_space = getSpaceDimension();
  medEntityMesh support_entity = Support->getEntity();
  bool onAll = Support->isOnAllElements();

  int nb_type;
  const medGeometryElement* types;
  int nb_entity_type;
  const int *global_connectivity= NULL;

  nb_type = Support->getNumberOfTypes();
  int length_values = Support->getNumberOfElements(MED_ALL_ELEMENTS);
  types = Support->getTypes();

  FIELD<double, FullInterlace>* Barycenter;
  Barycenter = new FIELD<double, FullInterlace>(Support,dim_space);
  Barycenter->setName("BARYCENTER");
  Barycenter->setDescription("cells or faces barycenter");

  for (int k=0;k<dim_space;k++) {
    int kp1 = k + 1;
    Barycenter->setComponentName(kp1,myMesh->getCoordinatesNames()[k]);
    Barycenter->setComponentDescription(kp1,"desc-comp");
    Barycenter->setMEDComponentUnit(kp1,myMesh->getCoordinatesUnits()[k]);
  }
  Barycenter->setIterationNumber(0);
  Barycenter->setOrderNumber(0);
  Barycenter->setTime(0.0);
  double *barycenter=(double *)Barycenter->getValue();
  const double * coord = getCoordinates(MED_EN::MED_FULL_INTERLACE);
  int index=0;
  for (int i=0;i<nb_type;i++)
  {
    medGeometryElement type = types[i] ;
    nb_entity_type = Support->getNumberOfElements(type);
    if(type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA )
    {
      if (onAll)
      {
        global_connectivity = getConnectivity(MED_EN::MED_NODAL,support_entity,type);
      }
      else
      {
        const int * supp_number = Support->getNumber(type);
        const int * connectivity = getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
        int * global_connectivity_tmp = new int[(type%100)*nb_entity_type];

        for (int k_type = 0; k_type<nb_entity_type; k_type++) {
          for (int j_ent = 0; j_ent<(type%100); j_ent++) {
            const int *global_connectivityIndex = getConnectivityIndex(MED_EN::MED_NODAL,support_entity);
            global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[global_connectivityIndex[supp_number[k_type]-1]+j_ent-1];
          }
        }
        global_connectivity = global_connectivity_tmp;
      }
    }

    switch (type)
    {
    case MED_EN::MED_TETRA4 : case MED_EN::MED_TETRA10 :
      {
        for (int tetra =0;tetra<nb_entity_type;tetra++)
        {
          int tetra_index = (type%100)*tetra;

          int N1 = global_connectivity[tetra_index]-1;
          int N2 = global_connectivity[tetra_index+1]-1;
          int N3 = global_connectivity[tetra_index+2]-1;
          int N4 = global_connectivity[tetra_index+3]-1;
          double *pts[4];
          pts[0]=(double *)coord+dim_space*N1;
          pts[1]=(double *)coord+dim_space*N2;
          pts[2]=(double *)coord+dim_space*N3;
          pts[3]=(double *)coord+dim_space*N4;
          INTERP_KERNEL::calculateBarycenter<4,3>((const double **)pts,barycenter+3*index);
          index++;
        }
        break;
      }
    case MED_EN::MED_PYRA5 : case MED_EN::MED_PYRA13 :
      {
        for (int pyra=0;pyra<nb_entity_type;pyra++)
        {
          int pyra_index = (type%100)*pyra;

          int N1 = global_connectivity[pyra_index]-1;
          int N2 = global_connectivity[pyra_index+1]-1;
          int N3 = global_connectivity[pyra_index+2]-1;
          int N4 = global_connectivity[pyra_index+3]-1;
          int N5 = global_connectivity[pyra_index+4]-1;
          double *pts[5];
          pts[0]=(double *)coord+dim_space*N1;
          pts[1]=(double *)coord+dim_space*N2;
          pts[2]=(double *)coord+dim_space*N3;
          pts[3]=(double *)coord+dim_space*N4;
          pts[4]=(double *)coord+dim_space*N5;
          INTERP_KERNEL::calculateBarycenter<5,3>((const double **)pts,barycenter+3*index);
          index++;
        }
        break;
      }
    case MED_EN::MED_PENTA6 : case MED_EN::MED_PENTA15 :
      {
        for (int penta=0;penta<nb_entity_type;penta++)
        {
          int penta_index = (type%100)*penta;

          int N1 = global_connectivity[penta_index]-1;
          int N2 = global_connectivity[penta_index+1]-1;
          int N3 = global_connectivity[penta_index+2]-1;
          int N4 = global_connectivity[penta_index+3]-1;
          int N5 = global_connectivity[penta_index+4]-1;
          int N6 = global_connectivity[penta_index+5]-1;
          double *pts[6];
          pts[0]=(double *)coord+dim_space*N1;
          pts[1]=(double *)coord+dim_space*N2;
          pts[2]=(double *)coord+dim_space*N3;
          pts[3]=(double *)coord+dim_space*N4;
          pts[4]=(double *)coord+dim_space*N5;
          pts[5]=(double *)coord+dim_space*N6;
          INTERP_KERNEL::calculateBarycenter<6,3>((const double **)pts,barycenter+3*index);
          index++;
        }
        break;
      }
    case MED_EN::MED_HEXA8 : case MED_EN::MED_HEXA20 :
      {
        for (int hexa=0;hexa<nb_entity_type;hexa++)
        {
          int hexa_index = (type%100)*hexa;

          int N1 = global_connectivity[hexa_index]-1;
          int N2 = global_connectivity[hexa_index+1]-1;
          int N3 = global_connectivity[hexa_index+2]-1;
          int N4 = global_connectivity[hexa_index+3]-1;
          int N5 = global_connectivity[hexa_index+4]-1;
          int N6 = global_connectivity[hexa_index+5]-1;
          int N7 = global_connectivity[hexa_index+6]-1;
          int N8 = global_connectivity[hexa_index+7]-1;
          double *pts[8];
          pts[0]=(double *)coord+dim_space*N1;
          pts[1]=(double *)coord+dim_space*N2;
          pts[2]=(double *)coord+dim_space*N3;
          pts[3]=(double *)coord+dim_space*N4;
          pts[4]=(double *)coord+dim_space*N5;
          pts[5]=(double *)coord+dim_space*N6;
          pts[6]=(double *)coord+dim_space*N7;
          pts[7]=(double *)coord+dim_space*N8;
          INTERP_KERNEL::calculateBarycenter<8,3>((const double **)pts,barycenter+3*index);
          index++;
        }
        break;
      }
    case MED_EN::MED_TRIA3 : case MED_EN::MED_TRIA6 :
      {
        for (int tria=0;tria<nb_entity_type;tria++)
        {
          int tria_index = (type%100)*tria;
          int N1 = global_connectivity[tria_index]-1;
          int N2 = global_connectivity[tria_index+1]-1;
          int N3 = global_connectivity[tria_index+2]-1;
          double *pts[3];
          pts[0]=(double *)coord+dim_space*N1;
          pts[1]=(double *)coord+dim_space*N2;
          pts[2]=(double *)coord+dim_space*N3;
          if (dim_space==2)
            INTERP_KERNEL::calculateBarycenter<3,2>((const double **)pts,barycenter+2*index);
          else
            INTERP_KERNEL::calculateBarycenter<3,3>((const double **)pts,barycenter+3*index);
          index++;
        }
        break;
      }
    case MED_EN::MED_QUAD4 : case MED_EN::MED_QUAD8 :
      {
        for (int quad=0;quad<nb_entity_type;quad++)
        {
          int quad_index = (type%100)*quad;
          int N1 = global_connectivity[quad_index]-1;
          int N2 = global_connectivity[quad_index+1]-1;
          int N3 = global_connectivity[quad_index+2]-1;
          int N4 = global_connectivity[quad_index+3]-1;
          double *pts[4];
          pts[0]=(double *)coord+dim_space*N1;
          pts[1]=(double *)coord+dim_space*N2;
          pts[2]=(double *)coord+dim_space*N3;
          pts[3]=(double *)coord+dim_space*N4;
          if (dim_space==2)
            INTERP_KERNEL::calculateBarycenter<4,2>((const double **)pts,barycenter+2*index);
          else
            INTERP_KERNEL::calculateBarycenter<4,3>((const double **)pts,barycenter+3*index);
          index++;
        }
        break;
      }
    case MED_EN::MED_SEG2 : case MED_EN::MED_SEG3 :
      {
        for (int edge=0;edge<nb_entity_type;edge++)
        {
          int edge_index = (type%100)*edge;
          int N1 = global_connectivity[edge_index]-1;
          int N2 = global_connectivity[edge_index+1]-1;
          double *pts[2];
          pts[0]=(double *)coord+dim_space*N1;
          pts[1]=(double *)coord+dim_space*N2;
          if (dim_space==2)
            INTERP_KERNEL::calculateBarycenter<2,2>((const double **)pts,barycenter+2*index);
          else
            INTERP_KERNEL::calculateBarycenter<2,3>((const double **)pts,barycenter+3*index);
          index++;
        }
        break;
      }
    case MED_POLYGON :
      {
        if(onAll)
        {
          int offsetWithClassicType=getGlobalNumberingIndex(support_entity)[i]-1;
          const int * connectivity = _connectivity->getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
          const int * connectivity_index = _connectivity->getConnectivityIndex(MED_EN::MED_NODAL,support_entity) + offsetWithClassicType;
          for (int polygs=0;polygs<nb_entity_type;polygs++)
          {
            int size=connectivity_index[polygs+1]-connectivity_index[polygs];
            double **pts=new double * [size];
            for(int iPts=0;iPts<size;iPts++)
              pts[iPts]=(double *)coord+dim_space*(connectivity[connectivity_index[polygs]+iPts-1]-1);
            INTERP_KERNEL::calculateBarycenterDyn((const double **)pts,size,dim_space,barycenter+dim_space*index);
            delete [] pts;
            index++;
          }
        }
        else
        {
          const int * supp_number = Support->getNumber(MED_EN::MED_POLYGON);
          const int * connectivity = _connectivity->getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
          const int * connectivity_index = _connectivity->getConnectivityIndex(MED_EN::MED_NODAL,support_entity);
          for (int polygs=0;polygs<nb_entity_type;polygs++)
          {
            int localPolygsNbP1=supp_number[polygs];
            int size=connectivity_index[localPolygsNbP1]-connectivity_index[localPolygsNbP1-1];
            double **pts=new double * [size];
            for(int iPts=0;iPts<size;iPts++)
              pts[iPts]=(double *)coord+dim_space*(connectivity[connectivity_index[localPolygsNbP1-1]+iPts-1]-1);
            INTERP_KERNEL::calculateBarycenterDyn((const double **)pts,size,dim_space,barycenter+dim_space*index);
            delete [] pts;
            index++;
          }
        }
        break;
      }
    case MED_EN::MED_POLYHEDRA:
      {
        if(onAll)
        {
          for (int polyhs=0;polyhs<nb_entity_type;polyhs++)
          {
            int lgthNodes;
            int offsetWithClassicType=getGlobalNumberingIndex(support_entity)[i]-1;
            int *nodes=_connectivity->getNodesOfPolyhedron(offsetWithClassicType+polyhs+1,lgthNodes);
            double **pts=new double * [lgthNodes];
            for(int iPts=0;iPts<lgthNodes;iPts++)
              pts[iPts]=(double *)coord+3*(nodes[iPts]-1);
            INTERP_KERNEL::calculateBarycenterDyn((const double **)pts,lgthNodes,3,barycenter+3*index);
            delete [] pts;
            delete [] nodes;
            index++;
          }
        }
        else
        {
          const int * supp_number = Support->getNumber(MED_EN::MED_POLYHEDRA);
          for (int polyhs=0;polyhs<nb_entity_type;polyhs++)
          {
            int lgthNodes;
            int *nodes=_connectivity->getNodesOfPolyhedron(supp_number[polyhs],lgthNodes);
            double **pts=new double * [lgthNodes];
            for(int iPts=0;iPts<lgthNodes;iPts++)
              pts[iPts]=(double *)coord+3*(nodes[iPts]-1);
            INTERP_KERNEL::calculateBarycenterDyn((const double **)pts,lgthNodes,3,barycenter+3*index);
            delete [] pts;
            delete [] nodes;
            index++;
          }
        }
        break;
      }
    default :
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad Support to get a barycenter on it (in fact unknown type) !"));
      break;
    }

    if (!onAll)
      if(type != MED_EN::MED_POLYGON && type != MED_EN::MED_POLYHEDRA)
        delete [] global_connectivity;
  }
  return Barycenter;
}
/*!
  @}
*/

bool MESH::isEmpty() const
{
  bool notempty = _name != "NOT DEFINED"                || _coordinate != NULL           || _connectivity != NULL ||
    _spaceDimension !=MED_INVALID || 
    _numberOfNodes !=MED_INVALID  || _groupNode.size() != 0   ||
    _familyNode.size() != 0       || _groupCell.size() != 0   ||
    _familyCell.size() != 0       || _groupFace.size() != 0   ||
    _familyFace.size() != 0       || _groupEdge.size() != 0   ||
    _familyEdge.size() != 0;
  return !notempty;
}

//================================================================================
/*!
 * \brief Check nature of GMESH
 */
//================================================================================

bool MESH::getIsAGrid() const
{
  return false;
}

/*!
 * \brief Implement pure virtual method used to get MESH from GMESH
 */
const MESH* MESH::convertInMESH() const
{
  this->addReference();
  return this;
}

/*!
\addtogroup MESH_advanced
@{
*/

/*!
  Retrieves the skin of support \a Support3D. This method is only available in 3D.
  On output, it returns a MED_EN::MED_FACE support with the skin of all elements contained in support.
  The skin is defined as the list of faces that are compnents of only one volume in the input
  support.

  WARNING: This method can recalculate descending connectivity from partial to full form,
  so that partial SUPPORT on MED_EN::MED_FACE on this mesh becomes invalid.
*/
SUPPORT * MESH::getSkin(const SUPPORT * Support3D) throw (MEDEXCEPTION)
{
  const char * LOC = "MESH::getSkin : " ;
  BEGIN_OF_MED(LOC) ;
  // some test :
  if (this != Support3D->getMesh())
    throw MEDEXCEPTION(STRING(LOC) <<  "no compatibility between *this and SUPPORT::_mesh !");
  if (getMeshDimension() != 3 || Support3D->getEntity() != MED_EN::MED_CELL)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Defined on 3D cells only"));

  // well, all rigth !
  SUPPORT * mySupport = new SUPPORT;
  mySupport->setMesh((MESH *)this);
  mySupport->setName("Skin");
  mySupport->setEntity( MED_EN::MED_FACE );

  list<int> myElementsList;
  int i,j, size = 0;

  // assure that descending connectivity is full
  if ( !_connectivity )
    throw MEDEXCEPTION(STRING(LOC) << "no connectivity defined in MESH !");
  _connectivity->calculateFullDescendingConnectivity(MED_EN::MED_CELL);
  if (Support3D->isOnAllElements())
  {
    const int* value = getReverseConnectivity(MED_EN::MED_DESCENDING);
    const int* index = getReverseConnectivityIndex(MED_EN::MED_DESCENDING);

    int nbFaces = getNumberOfElements(MED_EN::MED_FACE,MED_ALL_ELEMENTS);
    for (int i = 0; i < nbFaces; i++)
    {
      //a face is in skin if it is linked to one element or if one of the elements
      //it is linked to is "virtual"
      if ((index[i+1]-index[i]==1) || (value[index[i]-1]==0) || (value[index[i]]==0)) {
        myElementsList.push_back( i+1 );
        size++;
      }
    }
  }
  else
  {
    map<int,int> FaceNbEncounterNb;

    int * myConnectivityValue = const_cast <int *>
      (getConnectivity(MED_EN::MED_DESCENDING,MED_EN::MED_CELL, MED_ALL_ELEMENTS));
    int * myConnectivityIndex = const_cast <int *> (getConnectivityIndex(MED_EN::MED_DESCENDING, MED_EN::MED_CELL));
    int * myCellNbs = const_cast <int *> (Support3D->getnumber()->getValue());
    int nbCells = Support3D->getnumber()->getLength();
    for (i=0; i<nbCells; ++i)
    {
      int cellNb = myCellNbs [ i ];
      int faceFirst = myConnectivityIndex[ cellNb-1 ];
      int faceLast  = myConnectivityIndex[ cellNb ];
      for (j = faceFirst; j < faceLast; ++j)
      {
        int faceNb = abs( myConnectivityValue [ j-1 ] );
        if (FaceNbEncounterNb.find( faceNb ) == FaceNbEncounterNb.end())
          FaceNbEncounterNb[ faceNb ] = 1;
        else
          FaceNbEncounterNb[ faceNb ] ++;
      }
    }
    map<int,int>::iterator FaceNbEncounterNbItr;
    for (FaceNbEncounterNbItr = FaceNbEncounterNb.begin();
         FaceNbEncounterNbItr != FaceNbEncounterNb.end();
         FaceNbEncounterNbItr ++)
      if ((*FaceNbEncounterNbItr).second == 1)
      {
        myElementsList.push_back( (*FaceNbEncounterNbItr).first) ;
        size++ ;
      }
  }
  // Well, we must know how many geometric type we have found
  int * myListArray = new int[size] ;
  int id = 0 ;
  list<int>::iterator myElementsListIt ;
  for (myElementsListIt=myElementsList.begin();myElementsListIt!=myElementsList.end();myElementsListIt++) {
    myListArray[id]=(*myElementsListIt) ;
    id ++ ;
  }

  int numberOfGeometricType ;
  medGeometryElement* geometricType ;
  int * geometricTypeNumber ;
  int * numberOfEntities ;
  int * mySkyLineArrayIndex ;

  int numberOfType = getNumberOfTypes(MED_EN::MED_FACE) ;
  if (numberOfType == 1) // wonderfull : it's easy !
    {
      numberOfGeometricType = 1 ;
      geometricType = new medGeometryElement[1] ;
      const medGeometryElement *  allType = getTypes(MED_EN::MED_FACE);
      geometricType[0] = allType[0] ;
      geometricTypeNumber = new int[1] ; // not use, but initialized to nothing
      geometricTypeNumber[0] = 0 ;
      numberOfEntities = new int[1] ;
      numberOfEntities[0] = size ;
      mySkyLineArrayIndex = new int[2] ;
      mySkyLineArrayIndex[0]=1 ;
      mySkyLineArrayIndex[1]=1+size ;
    }
  else // hemmm
    {
      map<medGeometryElement,int> theType ;
      for (myElementsListIt=myElementsList.begin();myElementsListIt!=myElementsList.end();myElementsListIt++)
        {
          medGeometryElement myType = getElementType(MED_EN::MED_FACE,*myElementsListIt) ;
          if (theType.find(myType) != theType.end() )
            theType[myType]+=1 ;
          else
            theType[myType]=1 ;
        }
      numberOfGeometricType = theType.size() ;
      geometricType = new medGeometryElement[numberOfGeometricType] ;
      geometricTypeNumber = new int[numberOfGeometricType] ; // not use, but initialized to nothing
      numberOfEntities = new int[numberOfGeometricType] ;
      mySkyLineArrayIndex = new int[numberOfGeometricType+1] ;
      int index = 0 ;
      mySkyLineArrayIndex[0]=1 ;
      map<medGeometryElement,int>::iterator theTypeIt ;
      for (theTypeIt=theType.begin();theTypeIt!=theType.end();theTypeIt++)
        {
          geometricType[index] = (*theTypeIt).first ;
          geometricTypeNumber[index] = 0 ;
          numberOfEntities[index] = (*theTypeIt).second ;
          mySkyLineArrayIndex[index+1]=mySkyLineArrayIndex[index]+numberOfEntities[index] ;
          index++ ;
        }
    }
  MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY(numberOfGeometricType,size,mySkyLineArrayIndex,myListArray) ;

  mySupport->setNumberOfGeometricType(numberOfGeometricType) ;
  mySupport->setGeometricType(geometricType) ;
  mySupport->setNumberOfElements(numberOfEntities) ;
  mySupport->setNumber(mySkyLineArray) ;

  delete[] numberOfEntities;
  delete[] geometricTypeNumber;
  delete[] geometricType;
  delete[] mySkyLineArrayIndex;
  delete[] myListArray;

  END_OF_MED(LOC);
  return mySupport ;

}



int MESH::getElementContainingPoint(const double *coord)
{
  if(_spaceDimension!=3 && _spaceDimension!=2)
    throw MEDEXCEPTION("MESH::getElementContainingPoint : invalid _spaceDimension must be equal to 2 or 3 !!!");
  PointLocator loc(*this);
  std::list<int> li=loc.locate(coord);
  if(li.empty())
    return -1;
  return li.front();
}

//! Converts MED_EN::MED_CELL connectivity to polyhedra connectivity
//! Converts MED_EN::MED_FACE connectivity to polygon connectivity
//! Wil work only for 3D meshes with nodal connectivity

void MESH::convertToPoly()
{
  if (getMeshDimension()!=3) return;

  CONNECTIVITY* newpolygonconnectivity = 0;
  CONNECTIVITY* newpolyhedraconnectivity = new CONNECTIVITY(MED_EN::MED_CELL);

  {
    ////////////////////////////////////////////:
    // First step : Treating polygons connectivity
    ///////////////////////////////////////////

    int oldnbface = getNumberOfElements(MED_EN::MED_FACE,MED_EN::MED_ALL_ELEMENTS);
    int nbTypes = oldnbface > 0 ? 1 : 0;
    newpolygonconnectivity = new CONNECTIVITY(nbTypes, MED_EN::MED_FACE);
    if ( nbTypes > 0 )
    {
      medGeometryElement type = MED_POLYGON;
      newpolygonconnectivity->setGeometricTypes( &type, MED_EN::MED_FACE );

      const int count[] = { 1, oldnbface + 1 };
      newpolygonconnectivity->setCount( count, MED_EN::MED_FACE );

      const int* oldconn = getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_FACE, MED_EN::MED_ALL_ELEMENTS);
      const int* oldconnindex= getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_FACE);
      newpolygonconnectivity->setNodal( oldconn, MED_EN::MED_FACE, type, oldconnindex );

      newpolygonconnectivity->setNumberOfNodes( getNumberOfNodes() );
      newpolygonconnectivity->setEntityDimension( 2 );
    }
  }
  ///////////////////////////////////////////
  // 2nd step : Treating polyhedra connectivity
  //////////////////////////////////////////
  {
    vector<int> newconn;
    vector<int> newindex(1,1);

    int nboldtypes=getNumberOfTypes(MED_EN::MED_CELL);
    const MED_EN::medGeometryElement* oldtypes = getTypes(MED_EN::MED_CELL);
    const int* oldconn = getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
    const int* oldconnindex= getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
    int oldnbelem = getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);

    for (int itype=0; itype<nboldtypes; itype++)
    {
      MED_EN::medGeometryElement type = oldtypes[itype];
      int nb_elems=getNumberOfElements(MED_EN::MED_CELL,type);
      if ( type == MED_POLYHEDRA )
      {
        const int* oldpolyindex = oldconnindex + getGlobalNumberingIndex( MED_EN::MED_CELL )[itype] - 1;
        int oldpolyconnsize = oldpolyindex[nb_elems] - oldpolyindex[0];
        newconn.insert( newconn.end(), oldconn, oldconn + oldpolyconnsize );
        int delta = newindex.back() - oldpolyindex[0];
        for (int ielem=0; ielem<nb_elems; ielem++)
          newindex.push_back( delta + oldpolyindex[ ielem+1 ]);
      }
      else
      {
        MEDMEM::CELLMODEL cellmodel(type);
        int nbfacespertype = cellmodel.getNumberOfConstituents(1);
        int nbnodespertype = cellmodel.getNumberOfNodes();
        for (int ielem=0; ielem<nb_elems; ielem++)
        {
          for (int iface=0; iface< nbfacespertype; iface++)
          {
            //local conn contains the local nodal connectivity for the iface-th face of type
            const int* local_conn = cellmodel.getNodesConstituent(1,iface+1);
            medGeometryElement facetype = cellmodel.getConstituentType(1,iface+1);
            int nbface_nodes=facetype%100;
            for ( int inode=0; inode<nbface_nodes;inode++)
              newconn.push_back(oldconn[local_conn[inode]-1]);
            if ( iface != nbfacespertype-1 )
              newconn.push_back(-1);
          }
          newindex.push_back( newconn.size() + 1 );
          oldconn += nbnodespertype;
        }
      }
    }
    int nbTypes = oldnbelem > 0 ? 1 : 0;
    if ( newpolyhedraconnectivity ) delete newpolyhedraconnectivity;
    newpolyhedraconnectivity = new CONNECTIVITY(nbTypes, MED_EN::MED_CELL);
    if ( nbTypes > 0 )
    {
      medGeometryElement type = MED_POLYHEDRA;
      newpolyhedraconnectivity->setGeometricTypes( &type, MED_EN::MED_CELL );

      const int count[] = { 1, oldnbelem + 1 };
      newpolyhedraconnectivity->setCount( count, MED_EN::MED_CELL );

      newpolyhedraconnectivity->setNodal( &newconn[0], MED_EN::MED_CELL, type, &newindex[0] );

      newpolyhedraconnectivity->setNumberOfNodes( getNumberOfNodes() );
      newpolyhedraconnectivity->setEntityDimension( 3 );
    }
  }

  delete _connectivity;

  _connectivity=newpolyhedraconnectivity;
  _connectivity->setConstituent(newpolygonconnectivity);

}

vector< vector<double> > MESH::getBoundingBox() const
{
  const double *myCoords=_coordinate->getCoordinates(MED_EN::MED_FULL_INTERLACE);
  vector< vector<double> > ret(2);
  int i,j;
  ret[0].resize(_spaceDimension);
  ret[1].resize(_spaceDimension);
  for(i=0;i<_spaceDimension;i++)
  {
    ret[0][i]=1.e300;
    ret[1][i]=-1.e300;
  }
  for(i=0;i<_coordinate->getNumberOfNodes();i++)
  {
    for(j=0;j<_spaceDimension;j++)
    {
      double tmp=myCoords[i*_spaceDimension+j];
      if(tmp<ret[0][j])
        ret[0][j]=tmp;
      if(tmp>ret[1][j])
        ret[1][j]=tmp;
    }
  }
  return ret;
}
