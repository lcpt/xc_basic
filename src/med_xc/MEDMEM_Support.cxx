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
 File Support.cxx
*/

#include "MEDMEM_Support.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_GMesh.hxx"
#include "MEDMEM_Meshing.hxx"

#include <set>
#include <algorithm>
#include <list>

using namespace std;
using namespace MED_EN;
using namespace MEDMEM;

#define MED_NBR_GEOMETRIE_MAILLE 15

/*!
\defgroup SUPPORT_general General information

\defgroup SUPPORT_creation Creation methods
The creation of a support requires a number of information
which is supplied to the MedMem library with the following methods.
When the support is defined on all elements, the creation method is 
very simple, for the element list is implicitly defined.

\defgroup SUPPORT_query Query methods

\defgroup SUPPORT_constructors Constructors

\defgroup SUPPORT_advanced Advanced methods

*/



/* This class is a generic class for family and group */

/*!
  Constructor.
*/
//--------------------------------------------------------------------------
SUPPORT::SUPPORT(): _name(""),  _description("None"), _mesh((GMESH*)NULL),
                    _entity(MED_EN::MED_CELL), _numberOfGeometricType(0),
                    _isOnAllElts(false),
                    _totalNumberOfElements(0),
                    _number((MEDSKYLINEARRAY*)NULL),
                    _number_fromfile(0)
  //--------------------------------------------------------------------------
{
  MESSAGE_MED("SUPPORT::SUPPORT()");
}

/*!
\addtogroup SUPPORT_constructors
@{
*/

/*!
  Constructor of a support lying on mesh \a Mesh. By default,
  the support lies on all elements of type \a Entity. 
  Partial support can be described using \a setpartial method.

\param Mesh Pointer to the mesh on which the support lies
\param Name Support name (should not exceed MED_TAILLE_NOM as defined in Med - i.e. 32 characters)
\param Entity Entity type of the support (MED_EN::MED_CELL,MED_EN::MED_FACE,MED_EN::MED_EDGE, MED_EN::MED_NODE)
*/
//--------------------------------------------------------------------------
// SUPPORT::SUPPORT(MESH* Mesh, string Name/*=""*/, medEntityMesh Entity/*=MED_EN::MED_CELL*/):
//   _name(Name), _description("None"), _mesh(Mesh), _entity(Entity),
//   _numberOfGeometricType(0), _isOnAllElts(true),
//   _totalNumberOfElements(0), _number((MEDSKYLINEARRAY*)NULL),_number_fromfile(0)
//   //--------------------------------------------------------------------------
// {
//   MESSAGE_MED("SUPPORT::SUPPORT(MESH*Mesh,string Name,medEntityMesh Entity)");
//   if(_mesh)
//     _mesh->addReference();
//   update() ;
// }

/*!
  Copy constructor.
*/
//--------------------------------------------------------------------------
SUPPORT::SUPPORT(const SUPPORT & m):_number_fromfile(0)
  //--------------------------------------------------------------------------
{
  const char* LOC = "SUPPORT::SUPPORT(SUPPORT & m) : ";
  BEGIN_OF_MED(LOC);

  _name = m._name ;
  _description = m._description ;
  _mesh = m._mesh ; // on recopie uniquement l'adresse
  if(_mesh)
    _mesh->addReference();
  _meshName = m._meshName;
  _entity = m._entity;
  _numberOfGeometricType = m._numberOfGeometricType;

  if (m._geometricType)
    _geometricType.set(_numberOfGeometricType,m._geometricType);

  _isOnAllElts = m._isOnAllElts;

  if (m._numberOfElements)
    _numberOfElements.set(_numberOfGeometricType,m._numberOfElements);

  _totalNumberOfElements = m._totalNumberOfElements;

  if ( m._number ) // m may be not filled SUPPORTClient
    _number = new MEDSKYLINEARRAY(* m._number);
  else
    _number = (MEDSKYLINEARRAY *) NULL;

  _profilNames=m._profilNames;

  END_OF_MED(LOC);
}

/*!
  @}
*/

/*!\ifnot MEDMEM_ug
  Affectation operator. operator = perform et deep copy except for attribute _mesh
\endif
*/
//--------------------------------------------------------------------------
SUPPORT & SUPPORT::operator=(const SUPPORT & m)
  //--------------------------------------------------------------------------
{
  const char* LOC = "SUPPORT::operator=(const SUPPORT & m) : ";
  BEGIN_OF_MED(LOC);

  if ( this == &m ) return *this;

  _name = m._name;
  _description = m._description;
  if(m._mesh!=_mesh)//setMesh not used here due to _meshName update is this...
    {
      if(_mesh)
        _mesh->removeReference();
      _mesh=m._mesh;
      if(_mesh)
        _mesh->addReference();
    }
  _entity = m._entity;
  _numberOfGeometricType = m._numberOfGeometricType;
  if (m._geometricType)
    _geometricType.set(_numberOfGeometricType,m._geometricType);
  _isOnAllElts = m._isOnAllElts;
  if (m._numberOfElements)
    _numberOfElements.set(_numberOfGeometricType,m._numberOfElements);
  _totalNumberOfElements = m._totalNumberOfElements;

  if (_number) delete _number;
  if  ( m._number ) // m may be not filled SUPPORTClient
    _number = new MEDSKYLINEARRAY(* m._number);
  else
    _number = (MEDSKYLINEARRAY *) NULL;

  _profilNames=m._profilNames;

  END_OF_MED(LOC);
  return *this;
}

/*!
  Destructor.
*/
//-----------------
SUPPORT::~SUPPORT()
  //-----------------
{
  MESSAGE_MED("Destructeur ~SUPPORT()");
  clearDataOnNumbers();
  if(_mesh)
    _mesh->removeReference();
}

/*!
  operator <<.
*/
//--------------------------------------------------
ostream & MEDMEM::operator<<(ostream &os, const SUPPORT &my)
  //--------------------------------------------------
{
  os << "Name : "<< my.getName() << endl ;
  os << "Description : "<< my.getDescription() << endl ;
  os << "Mesh ptr : ";
  if (my.getMesh() == NULL)
    os << " Mesh not defined." << endl ;
  else
    os << " Mesh defined." << endl;
  os << "MeshName : ";
  os << my.getMeshName() << endl ;
  os << "Entity : "<<entNames[my._entity] << endl;
  os << "Entity list : "<< endl;
  if ( my._isOnAllElts )
    os << "Is on all entities."<< endl;
  else {
    os << "Is not on all entities. "<< endl;
    if ( my._number )  // m may be not filled SUPPORTClient
      os << *my.getNumber(MED_ALL_ELEMENTS);
  }
  int numberoftypes = my._numberOfGeometricType ;
  os << "NumberOfTypes : "<<numberoftypes<<endl;
  PointerOf<medGeometryElement> types = my._geometricType;
  for (int j=0;j<numberoftypes;j++) {
    int numberOfElements = my._numberOfElements ? my._numberOfElements[j] : -1;
    os << "    On Type "<<geoNames[types[j]]
       <<" : there is(are) "<<numberOfElements<<" element(s) and " <<endl;
  }
  int nbProfilNames = my._profilNames.size();
  os<<"Number of profil names = "<<nbProfilNames<<endl;
  for(int j=0; j<nbProfilNames; j++) {
    os<<"    Profil Name N"<<j+1<<" = "<<my._profilNames[j]<<endl;
  }
  return os ;
}

/*!
  Updade the SUPPORT attributs with right MESH information.

  It has an effect only if SUPPORT is on all elements.

  No more need in future release.
*/
//-------------------
void SUPPORT::update()
//-------------------
{
  const char* LOC = "SUPPORT::update() : ";
  BEGIN_OF_MED(LOC);

  if (_isOnAllElts && _mesh)
    {
      if (_entity == MED_EN::MED_NODE)
        {
          // BEGIN Issue 0020804: [CEA 399] Memory corruption ... in MEDMEMCppTest
          //_numberOfGeometricType = 1;
          setNumberOfGeometricType(1);
          // END Issue 0020804

          // BEGIN Issue 0020633: [CEA] Pb with 3D field creation fron another
          // Use setGeometricType() in order to get _profilNames updated
          //_geometricType.set(1);
          //_geometricType[0]=MED_POINT1;
          const MED_EN::medGeometryElement type = MED_EN::MED_NONE;
          setGeometricType( & type );
          // END Issue 0020633: [CEA] Pb with 3D field creation fron another
          _numberOfElements.set(1);
          _numberOfElements[0]=_mesh->getNumberOfNodes();
          _totalNumberOfElements=_numberOfElements[0];
        }
      else
        { // we duplicate information from _mesh
          // BEGIN Issue 0020804: [CEA 399] Memory corruption ... in MEDMEMCppTest
          setNumberOfGeometricType(_mesh->getNumberOfTypes(_entity));
          // END Issue 0020804
          // BEGIN Issue 0020633: [CEA] Pb with 3D field creation fron another
          if ( const medGeometryElement *  allType = _mesh->getTypes(_entity))
            setGeometricType( allType );
          // END Issue 0020633: [CEA] Pb with 3D field creation fron another
          _numberOfElements.set(_numberOfGeometricType);
          _totalNumberOfElements=0;
          for (int i=0;i<_numberOfGeometricType;i++)
            {
              _numberOfElements[i]=_mesh->getNumberOfElements(_entity,_geometricType[i]) ;
              _totalNumberOfElements+=_numberOfElements[i];
            }
        }

      if (_totalNumberOfElements <= 0)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"We have found no element for this support !"));
      // set _number (issue 0021167)
      {
        vector<int> nums( _totalNumberOfElements );
        for ( unsigned i = 0; i < nums.size(); ++i )
          nums[i] = i+1;

        vector<int> index( _numberOfGeometricType + 1 );
        index[0] = 1;
        for ( int i = 0; i < _numberOfGeometricType; ++i )
          index[i+1] = index[i] + _numberOfElements[i];

        setNumber( & index[0], & nums[0] );
      }
    }
  END_OF_MED(LOC);
}

/*!
  Get the field value index (in fortran mode) from the support global number.
  Becareful, it doesn't take care of the field number of components
*/
//-------------------
int SUPPORT::getValIndFromGlobalNumber(const int number) const throw (MEDEXCEPTION)
//-------------------
{
  const char * LOC="getValIndFromGlobalNumber(const int number) : ";

  if (_isOnAllElts) return number;

  int nbOfEltsThis    = getNumberOfElements(MED_ALL_ELEMENTS);

  const int *eltsThis = _number->getValue();

  int iThis;
  bool found=false;

  for(iThis=0;iThis<nbOfEltsThis && !found;)
    if(eltsThis[iThis]==number)
    {
      found = true;
      int valInd = iThis+1;
      return valInd;
    }
    else
      iThis++;

  if(!found)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Can't find the global number |"
                                 << number << "| in Support |"
                                 << getName() << "|" ));

  // It should never arrive here !!
  return 0;
}

/*!
\addtogroup SUPPORT_advanced
@{

*/

/*!
  Blends the given SUPPORT mySupport into the calling object SUPPORT.
  Example :
\verbatim
SUPPORT mySupport ;
SUPPORT myOtherSupport ;
...
mySupport.blending(myOtherSupport) ;
\endverbatim
Support \a mySupport now contains a union of the elements originally
contained in \a mySupport and \a myOtherSupport.
*/
//-------------------
void SUPPORT::blending(const SUPPORT * mySupport) throw (MEDEXCEPTION)
  //-------------------
{
  const char * LOC="SUPPORT::blending(SUPPORT *) : ";
  BEGIN_OF_MED(LOC);
  if (_entity!=mySupport->getEntity())
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Entities are different !"));
  if(!(*_mesh == *mySupport->getMesh()))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Mesh are different !"));
  if(_isOnAllElts)
    return;
  if(mySupport->isOnAllElements())
  {
    *this=*mySupport;
    return;
  }
  if(mySupport->_totalNumberOfElements==0)
    return;
  const int *ids=getNumber(MED_ALL_ELEMENTS);
  set<int> idsSet(ids,ids+getNumberOfElements(MED_ALL_ELEMENTS));
  const int *idsMySupport=mySupport->getNumber(MED_ALL_ELEMENTS);
  int mySupportSize=mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  set<int>::iterator iter;
  for(int i=0;i<mySupportSize;i++)
    idsSet.insert(idsMySupport[i]);
  int size=idsSet.size();

  if(size!=0)
  {
    list<int> idsList;
    for(iter=idsSet.begin();iter!=idsSet.end();iter++)
      idsList.push_back(*iter);

    MESSAGE_MED(LOC << " size Set " << idsSet.size() << " size List " << idsList.size());

    if(_entity==MED_EN::MED_NODE)
      fillFromNodeList(idsList);
    else
      fillFromElementList(idsList);
  }
  else
    clearDataOnNumbers();
  END_OF_MED(LOC);
}
/*!  @}  */

/*! 
\addtogroup SUPPORT_creation
@{
*/

/*!
  This function allows the user to set a support not on all entities Entity,
  it should be used after setting the mesh and the entity attributes at least.

\param Description string describing the support for information purposes (should not exceed MED_TAILLE_DESC length - i.e. 200 characters)
\param NumberOfGeometricType number of geometric types contained in the support 
\param TotalNumberOfElements number of elements in the support
\param GeometricType array describing the geometric types (must be consistent with the entity that was passed as an argument to the support constructor)
\param NumberOfElements array describing the number of elements for each type
\param NumberValue array of IDs of the elements that constitute the group.

The following example refers to the mesh given in the mesh connectivity example.
It creates a group containing the two cells on the right (the quadratic triangle and the quadrangle on the right).

\verbatim
// creating SUPPORT on cells with one value per cell
right_group = new SUPPORT;
right_group->setMesh(mesh);
right_group->setEntity( MED_EN::MED_CELL );
right_group->setName("right group");

string description = "partial support";
int number_of_types=2;
int number_of_elements=2;
medGeometryElement geom_types[2]={MED_QUAD4, MED_TRIA6};
int number_of_elem_per_type[2]={1,1};
int number_value[2]={3,4};

//defining the region of the support
right_group->setpartial(description, number_of_types,
number_of_elements, geom_types,
number_of_elem_per_type, number_value);
\endverbatim

When MED_POLYGON or MED_POLYHEDRA elements are included in the support,
their global number should be given. For instance, on a mesh having ten MED_TRIA3 
and five MED_POLYGON, the number of the first polygonal element is 11. 
*/

//-------------------
void SUPPORT::setpartial(const std::string&         Description,
                         int                        NumberOfGeometricType,
                         int                        TotalNumberOfElements,
                         const medGeometryElement * GeometricType,
                         const int *                NumberOfElements,
                         const int *                NumberValue)
//-------------------
{
  const char * LOC = "SUPPORT::setpartial(string , int , int , medGeometryElement * , int * , int *) : " ;
  BEGIN_OF_MED(LOC) ;

  _isOnAllElts = false ;

  _description=Description;

  _numberOfGeometricType=NumberOfGeometricType;
  _geometricType.set(NumberOfGeometricType);
  _numberOfElements.set(NumberOfGeometricType);
  _totalNumberOfElements = TotalNumberOfElements;

  int * index = new int[_numberOfGeometricType+1];
  index[0]=1;
  int elemDim = -1;
  for (int i=0;i<_numberOfGeometricType;i++) {
    if(GeometricType[i]/100 != elemDim)
      {
        if(i==0)
          elemDim=GeometricType[i]/100;
        else if ( CELLMODEL_Map::retrieveCellModel( GeometricType[i] ).getDimension() !=
                  CELLMODEL_Map::retrieveCellModel( GeometricType[0] ).getDimension() )
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"unhomogeneous geometric types (dimension) !"));
      }
    _geometricType[i] = GeometricType[i] ;
    _numberOfElements[i] = NumberOfElements[i] ;
    index[i+1] = index[i]+NumberOfElements[i] ;
  }

  if (_number!=NULL) delete _number ;
  _number = new MEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfElements,index,NumberValue);

  delete[] index ;

  // PAL16854(Partial support on nodes):
  // giving a default value to profile names
  vector<string> prof_names( NumberOfGeometricType);
  for (int itype=0; itype < NumberOfGeometricType; itype++)
  {
    ostringstream typestr;
    typestr<<_name<<"_type"<<_geometricType[itype];
    prof_names[itype]=typestr.str();
  }
  setProfilNames(prof_names);

  END_OF_MED(LOC);
}

/*! @}  */

/*!
\ifnot MEDMEM_ug
This function allows the user to set a support not on all entities Entity,
it should be used after an initialisation of :
SUPPORT(GMESH* Mesh, string Name="", medEntityMesh Entity=MED_EN::MED_CELL) and
after calling  at least setGeometricType and perharps setEntity.
It allocates and initialises all the attributs of the class SUPPORT but
doesn't set a description, a SUPPORT name, a meshName and an associated GMESH.
\endif
*/

//-------------------
void SUPPORT::setpartial(MEDSKYLINEARRAY * number, bool shallowCopy) throw (MEDEXCEPTION)
  //-------------------
{
  const char * LOC = "SUPPORT::setpartial(MEDSKYLINEARRAY * number) : " ;
  BEGIN_OF_MED(LOC) ;

  if ( ! _geometricType )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"SUPPORT must contains"
                                 << " a geometric type list" )) ;

  _numberOfGeometricType = number->getNumberOf();

  _numberOfElements.set(_numberOfGeometricType);

  for (int i=0; i< _numberOfGeometricType; i++)
    _numberOfElements[i] =  number->getNumberOfI(i+1);

  _totalNumberOfElements = number->getLength();

  _isOnAllElts = false ;

  if (_number!=NULL) delete _number ;

  if ( shallowCopy )
    _number = number;
  else
    _number = new MEDSKYLINEARRAY(*number);

  END_OF_MED(LOC);
}

void SUPPORT::setpartial_fromfile(MEDSKYLINEARRAY * number, bool shallowCopy) throw (MEDEXCEPTION)
  //-------------------
{
  const char* LOC = "SUPPORT::setpartial_fromfile(MEDSKYLINEARRAY * number) : ";
  BEGIN_OF_MED(LOC);

  if ( shallowCopy )
    _number_fromfile = number;
  else
    _number_fromfile = new MEDSKYLINEARRAY(*number);

  END_OF_MED(LOC);
}

void SUPPORT::setProfilNames(const std::vector<std::string>& profilNames) throw (MEDEXCEPTION){

  const char * LOC = "SUPPORT::setProfilNames(vector<string> profilNames) : " ;
  BEGIN_OF_MED(LOC) ;

  if ( _isOnAllElts )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"SUPPORT shouldn't be on all elements"
                                 << " while setting profil name list" )) ;

  if ( ! _geometricType || _numberOfGeometricType==0 )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"SUPPORT must contains"
                                 << " a least one geometric type" )) ;

  if ( ! _number )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"SUPPORT must contains"
                                 << " a profil number list before setting"
                                 << " the associated profil name list" )) ;

  if ( ( (int)profilNames.size() != _number->getNumberOf() ) &&
       ( (int)profilNames.size() !=_numberOfGeometricType ) ) {
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The profil name list size : "<< profilNames.size()
                                 << " must be equal to the number of geometric type : " 
                                 <<  _numberOfGeometricType << " (_number->getNumberOf() : "
                                 << _number->getNumberOf() << " )"
                                 )) ;

  }

  _profilNames = profilNames;

  END_OF_MED(LOC);

}

vector<string> SUPPORT::getProfilNames() const throw (MEDEXCEPTION)
{
  return _profilNames;
}

/*!
\addtogroup SUPPORT_advanced
@{
*/

/*!
  This method gets the boundary elements of the mesh. The support has to be
  build using SUPPORT() followed by setMesh(GMESH*) setName(string) and
  setEntity(medEntityMesh) before using this method.
*/
//-------------------
void SUPPORT::getBoundaryElements() throw (MEDEXCEPTION)
  //-------------------
{
  const char * LOC = "SUPPORT::getBoundaryElements() : " ;
  BEGIN_OF_MED(LOC) ;

  if (_mesh == (GMESH*)NULL) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"You shlould use the appropriate SUPPORT Constructor before calling this method"));

  int spaceDimension = _mesh->getSpaceDimension();

  if (spaceDimension == 3)
    if (_entity != MED_EN::MED_FACE)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 3D mesh for entity "<<_entity<<" !"));
  if (spaceDimension == 2)
    if (_entity != MED_EN::MED_EDGE)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 2D mesh for entity "<<_entity<<" !"));

  setAll(false);

  const MESH* mesh = _mesh->convertInMESH();
  const_cast<CONNECTIVITY*>
    (mesh->getConnectivityptr())->calculateFullDescendingConnectivity(MED_EN::MED_CELL);
  const int * myConnectivityValue = mesh->getReverseConnectivity(MED_EN::MED_DESCENDING) ;
  const int * myConnectivityIndex = mesh->getReverseConnectivityIndex(MED_EN::MED_DESCENDING) ;
  int numberOf = mesh->getNumberOfElements(_entity,MED_ALL_ELEMENTS) ;
  list<int> myElementsList ;
  int size = 0 ;
  SCRUTE_MED(numberOf) ;
  for (int i=0 ; i<numberOf; i++)
    if (myConnectivityValue[myConnectivityIndex[i]] == 0) {
      SCRUTE_MED(i+1) ;
      myElementsList.push_back(i+1) ;
      size++ ;
    }
  SCRUTE_MED(size) ;
  // Well, we must know how many geometric type we have found
  int * myListArray = new int[size] ;
  int id = 0 ;
  list<int>::iterator myElementsListIt ;
  for (myElementsListIt=myElementsList.begin();myElementsListIt!=myElementsList.end();myElementsListIt++) {
    myListArray[id]=(*myElementsListIt) ;
    SCRUTE_MED(id);
    SCRUTE_MED(myListArray[id]);
    id ++ ;
  }

  int numberOfGeometricType ;
  medGeometryElement* geometricType ;
  int * geometricTypeNumber ;
  int * numberOfElements ;
  int * mySkyLineArrayIndex ;

  int numberOfType = mesh->getNumberOfTypes(_entity) ;
  if (numberOfType == 1) { // wonderfull : it's easy !
    numberOfGeometricType = 1 ;
    geometricType = new medGeometryElement[1] ;
    const medGeometryElement *  allType = mesh->getTypes(_entity);
    geometricType[0] = allType[0] ;
    geometricTypeNumber = new int[1] ; // not use, but initialized to nothing
    geometricTypeNumber[0] = 0 ;
    numberOfElements = new int[1] ;
    numberOfElements[0] = size ;
    mySkyLineArrayIndex = new int[2] ;
    mySkyLineArrayIndex[0]=1 ;
    mySkyLineArrayIndex[1]=1+size ;
  }
  else {// hemmm
    map<medGeometryElement,int> theType ;
    for (myElementsListIt=myElementsList.begin();myElementsListIt!=myElementsList.end();myElementsListIt++) {
      medGeometryElement myType = mesh->getElementType(_entity,*myElementsListIt) ;
      if (theType.find(myType) != theType.end() )
        theType[myType]+=1 ;
      else
        theType[myType]=1 ;
    }
    numberOfGeometricType = theType.size() ;
    geometricType = new medGeometryElement[numberOfGeometricType] ;
    geometricTypeNumber = new int[numberOfGeometricType] ; // not use, but initialized to nothing
    numberOfElements = new int[numberOfGeometricType] ;
    mySkyLineArrayIndex = new int[numberOfGeometricType+1] ;
    int index = 0 ;
    mySkyLineArrayIndex[0]=1 ;
    map<medGeometryElement,int>::iterator theTypeIt ;
    for (theTypeIt=theType.begin();theTypeIt!=theType.end();theTypeIt++) {
      geometricType[index] = (*theTypeIt).first ;
      geometricTypeNumber[index] = 0 ;
      numberOfElements[index] = (*theTypeIt).second ;
      mySkyLineArrayIndex[index+1]=mySkyLineArrayIndex[index]+numberOfElements[index] ;
      index++ ;
    }
  }
  MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY(numberOfGeometricType,size,mySkyLineArrayIndex,myListArray) ;

  setNumberOfGeometricType(numberOfGeometricType) ;
  setGeometricType(geometricType) ;
  setNumberOfElements(numberOfElements) ;

  _number = new MEDSKYLINEARRAY(numberOfGeometricType,size);

  _number->setIndex(mySkyLineArrayIndex);

  for (int i=0;i<size;i++)
  {
    _number->setIndexValue(i+1,myListArray[i]);
  }

  delete[] numberOfElements;
  delete[] geometricTypeNumber;
  delete[] geometricType;
  delete[] mySkyLineArrayIndex;
  delete[] myListArray;
  delete mySkyLineArray;

  mesh->removeReference();

  END_OF_MED(LOC);
}

/*!
  Intersects \a mySupport into the calling SUPPORT object.
  If A.intersecting(B) is called, on output, \f$ A \f$ contains \f$A \cap B\f$.
*/
//-------------------
void SUPPORT::intersecting(const SUPPORT * mySupport) throw (MEDEXCEPTION)
{
  const char * LOC="SUPPORT::intersecting(SUPPORT *) : ";
  BEGIN_OF_MED(LOC);
  if (_entity!=mySupport->getEntity())
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Entities are different !"));
  if(!(*_mesh == *mySupport->getMesh()))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Mesh are different !"));
  if(mySupport->isOnAllElements())
    return;
  if(_isOnAllElts)
  {
    *this=*mySupport;
    return;
  }
  if(_totalNumberOfElements==0)
    return;
  const int *ids=getNumber(MED_ALL_ELEMENTS);
  set<int> idsSet(ids,ids+getNumberOfElements(MED_ALL_ELEMENTS));
  const int *idsMySupport=mySupport->getNumber(MED_ALL_ELEMENTS);
  int mySupportSize=mySupport->getNumberOfElements(MED_ALL_ELEMENTS);
  set<int> idsSetMySupport(idsMySupport,idsMySupport+mySupportSize);
  set<int>::iterator iter;
  list<int> idsList;
  for(iter=idsSet.begin();iter!=idsSet.end();iter++)
    if(idsSetMySupport.find(*iter)!=idsSetMySupport.end())
      idsList.push_back(*iter);
  int size=idsSet.size();
  int sizeList = idsList.size();

  MESSAGE_MED(LOC << " size Set " << idsSet.size() << " size List " << idsList.size());

  if(size!=0 && sizeList != 0)
  {
    if(_entity==MED_EN::MED_NODE)
      fillFromNodeList(idsList);
    else
      fillFromElementList(idsList);
  }
  else
  {
    clearDataOnNumbers();
  }
  END_OF_MED(LOC);
}
/*!  @}  */

/*!
  Method that cleans up all the fields related to _numbers. Defined for code factorization.
*/
//--------------------------------------------------
void MEDMEM::SUPPORT::clearDataOnNumbers()
  //--------------------------------------------------
{
  _numberOfGeometricType=0;
  _totalNumberOfElements=0;

  if(_number)
  {
    delete _number;
    _number=(MEDSKYLINEARRAY *) NULL;
  }
  if(_number_fromfile)
    {
      delete _number_fromfile;
      _number_fromfile=0;
    }
}

/*!
  operator == This operator does not compare attributs _name and _description.
*/
//--------------------------------------------------
bool MEDMEM::SUPPORT::operator == (const SUPPORT &support) const
  //--------------------------------------------------
{

  const char* LOC = "bool SUPPORT::operator ==(const SUPPORT &support) const : ";
  BEGIN_OF_MED(LOC);

  bool operatorReturn = false;

  operatorReturn = (*_mesh == *support._mesh) && (_entity == support._entity) &&
    (_numberOfGeometricType == support._numberOfGeometricType) &&
    ((_isOnAllElts && support._isOnAllElts) || (!_isOnAllElts && !support._isOnAllElts)) &&
    (_totalNumberOfElements == support._totalNumberOfElements) &&
    (_profilNames.size() == support._profilNames.size());

  if (operatorReturn)
  {
    if (!_isOnAllElts)
    {
      for (int i=0; i<_numberOfGeometricType; i++)
      {
        operatorReturn = operatorReturn &&
          (_geometricType[i] == support._geometricType[i]) &&
          (_numberOfElements[i] == support._numberOfElements[i]);

        if (operatorReturn)
        {
          for (int j=0; j<_numberOfElements[i]; j++)
          {
            operatorReturn = operatorReturn &&
              (getNumber(_geometricType[i])[j] ==
               support.getNumber(_geometricType[i])[j]);
          }
        }
      }
    }
  }

  END_OF_MED(LOC);

  return operatorReturn;
}

void SUPPORT::changeElementsNbs(medEntityMesh entity, const int *renumberingFromOldToNew)
{
  if(entity != _entity)
    throw MEDEXCEPTION("SUPPORT::changeElementsNbs : Renumbering on a mismatch entity");
  list<int> newNbs;
  if(!_isOnAllElts)
  {
    const int *oldNbs=_number->getValue();
    for(int i=0;i<_totalNumberOfElements;i++)
      newNbs.push_back(renumberingFromOldToNew[oldNbs[i]-1]);
    newNbs.sort();
    fillFromElementList(newNbs);
  }
  else
    update();
}

/*!
  operator == + in case false a test if coordinates and connectivity of _mesh and support->_mesh are the same
*/
bool MEDMEM::SUPPORT::deepCompare(const SUPPORT &support) const
{
  bool operatorReturn =(_entity == support._entity) &&
    (_numberOfGeometricType == support._numberOfGeometricType) &&
    ( (_isOnAllElts && support._isOnAllElts) || (!_isOnAllElts  && !support._isOnAllElts) ) &&
    (_totalNumberOfElements == support._totalNumberOfElements);
  if (operatorReturn)
  {
    if (!_isOnAllElts)
    {
      for (int i=0; i<_numberOfGeometricType && operatorReturn; i++)
      {
        operatorReturn = (_geometricType[i] == support._geometricType[i]) &&
          (_numberOfElements[i] == support._numberOfElements[i]);
        if (operatorReturn)
        {
          for (int j=0; j<_numberOfElements[i]; j++)
          {
            operatorReturn = (getNumber(_geometricType[i])[j] ==
                              support.getNumber(_geometricType[i])[j]);
          }
        }
      }
    }
  }
  if(operatorReturn)
    operatorReturn = ( bool(_mesh) == bool(support._mesh));

  if(operatorReturn)
  {
    if(!(*_mesh == *support._mesh))
    {
      return _mesh->deepCompare(*support._mesh);
    }
  }
  return operatorReturn;
}

/*!
  States if this is included in other.
*/
bool MEDMEM::SUPPORT::belongsTo(const SUPPORT& other, bool deepCompare) const
{
  if(!(*_mesh == *other._mesh))
  {
    if(!deepCompare)
      return false;
    if(!_mesh->deepCompare(*other._mesh))
      return false;
  }
  if(_entity!=other._entity)
    return false;
  if(other._isOnAllElts)
    return true;
  if(_isOnAllElts && !other._isOnAllElts)
    return false;
  if(_numberOfGeometricType>other._numberOfGeometricType)
    return false;
  for(int i=0; i<_numberOfGeometricType; i++)
  {
    medGeometryElement curGeomType=_geometricType[i];
    int iOther=-1;
    for(int j=0; j<other._numberOfGeometricType; j++)
      if(other._geometricType[j]==curGeomType)
        iOther=j;
    if(iOther==-1)
      return false;
    if(_numberOfElements[i]>other._numberOfElements[iOther])
      return false;
    const int *numbers1=_number->getI(i+1);
    const int *numbers2=other._number->getI(iOther+1);
    for (int k=0; k<_numberOfElements[i]; k++)
    {
      bool found=false;
      for(int l=0;l<other._numberOfElements[iOther] && !found;l++)
      {
        if(numbers1[k]==numbers2[l])
          found=true;
      }
      if(!found)
        return false;
    }
  }
  return true;
}
/*!
  Method used to sort array of id.
*/
int compareId(const void *x, const void *y);
int compareId(const void *x, const void *y)
{
  const int *x1=(const int *)x;
  const int *y1=(const int *)y;
  if(*x1<*y1)
    return -1;
  else if(*x1>*y1)
    return 1;
  else
    return 0;
}

/*!
  performs a common operation : Sub builds a sorted int array that is obtained by supression of all ids contained
  in array defined by (idsToSuppress,lgthIdsToSuppress) from array [start ... end]
  Example sub(0,7,{1,2,5},3) => {0,3,4,6,7} - WARNING returned list should be deallocated !
*/
list<int> *MEDMEM::SUPPORT::sub(int start,int end,const int *idsToSuppress,int lgthIdsToSuppress)
{
  int size=end-start+1;
  int sizeRet=size-lgthIdsToSuppress;
  list<int> *ret;
  if(sizeRet<0)
    throw MEDEXCEPTION("MEDMEM::SUPPORT::sub");
  else if(sizeRet==0)
  {
    return 0;
  }
  if(idsToSuppress==0)
  {
    ret=new list<int>;
    for(int l=0;l<size;l++)
      ret->push_back(start+l);
    return ret;
  }
  ret=new list<int>;
  int *temp=new int[lgthIdsToSuppress+1];
  memcpy(temp,idsToSuppress,sizeof(int)*lgthIdsToSuppress);
  temp[lgthIdsToSuppress] = -1;
  qsort(temp,lgthIdsToSuppress,sizeof(int),compareId);
  int k=0;
  for(int i=start;i<=end;i++)
    if(temp[k]!=i)
      ret->push_back(i);
    else
      k++;
  delete [] temp;
  return ret;
}

/*!
  performs a common operation : Sub builds a sorted int array that is obtained by supression of all ids contained
  in array defined by (idsToSuppress,lgthIdsToSuppress) from array [start ... end]
  Example sub({1,3,4,5,6,7,9},7,{1,2,5},3) => {3,4,6,7,9}  - WARNING returned list should be deallocated !
*/
list<int> *MEDMEM::SUPPORT::sub(const int *ids,int lgthIds,const int *idsToSuppress,int lgthIdsToSuppress)
{
  list<int> *ret;
  int i,j=0;
  if(lgthIds<0)
    throw MEDEXCEPTION("MEDMEM::SUPPORT::sub");
  else if(lgthIds==0)
    return 0;
  ret=new list<int>;
  int *temp1=new int[lgthIds];
  memcpy(temp1,ids,sizeof(int)*lgthIds);
  qsort(temp1,lgthIds,sizeof(int),compareId);
  int *temp2=new int[lgthIdsToSuppress];
  memcpy(temp2,idsToSuppress,sizeof(int)*lgthIdsToSuppress);
  qsort(temp2,lgthIdsToSuppress,sizeof(int),compareId);
  for(i=0;i<lgthIds;)
  {
    if(j>=lgthIdsToSuppress)
      ret->push_back(temp1[i++]);
    else if(temp1[i]>temp2[j])
      j++;
    else if(temp1[i]<temp2[j])
      ret->push_back(temp1[i++]);
    else
      i++;
  }
  delete [] temp1;
  delete [] temp2;
  return ret;
}

/*!
  returns a new SUPPORT (responsability to caller to destroy it)
  that is the complement to "this", lying on the same entity than "this".
*/
SUPPORT *MEDMEM::SUPPORT::getComplement() const
{
  SUPPORT *ret;
  const int nbOfElt=_mesh->getNumberOfElements(_entity,MED_ALL_ELEMENTS);
  int nbOfEltInSupp=getNumberOfElements(MED_ALL_ELEMENTS);
  if(_isOnAllElts || nbOfElt==nbOfEltInSupp)
  {
    ret=new SUPPORT;
    ret->setMesh(_mesh);
    ret->setEntity(_entity);
    string name="Complement of ";
    name+=_name;
    ret->setName(name);
    return ret;
  }
  const int *nbs=_number->getValue();
  list<int> *ids=sub(1,nbOfElt,nbs,nbOfEltInSupp);
  if(_entity==MED_EN::MED_NODE)
    ret=_mesh->buildSupportOnNodeFromElementList(*ids,_entity);
  else
    ret=_mesh->buildSupportOnElementsFromElementList(*ids,_entity);
  delete ids;
  return ret;
}

/*!
  returns a new support the user should delete.
*/
SUPPORT *MEDMEM::SUPPORT::substract(const SUPPORT& other) const throw (MEDEXCEPTION)
{
  const char * LOC = "SUPPORT *MEDMEM::subtract(const SUPPORT * other) : ";
  BEGIN_OF_MED(LOC);
  SUPPORT *ret;
  if (_entity!=other.getEntity())
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Entities are different !"));
  if(!(*_mesh == *other.getMesh()))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Mesh are different !"));
  if(other._isOnAllElts)
  {
    ret=new SUPPORT;
    ret->setMesh(_mesh);
    ret->setEntity(_entity);
    return ret;
  }
  if(_isOnAllElts)
    return other.getComplement();
  int nbOfEltInThis=getNumberOfElements(MED_ALL_ELEMENTS);
  const int *nbsThis=_number->getValue();
  int nbOfEltInOther=other.getNumberOfElements(MED_ALL_ELEMENTS);
  const int *nbsOther=other._number->getValue();
  list<int> *ids=sub(nbsThis,nbOfEltInThis,nbsOther,nbOfEltInOther);
  if(_entity==MED_EN::MED_NODE)
    ret=_mesh->buildSupportOnNodeFromElementList(*ids,_entity);
  else
    ret=_mesh->buildSupportOnElementsFromElementList(*ids,_entity);
  delete ids;
  return ret;
  END_OF_MED(LOC);
}

/*!
  returns a new support the user has to delete. Entity is either MED_EN::MED_NODE to obtain node elements lying on boundary of "this"
  or MED_EN::MED_FACE,MED_EN::MED_EDGE (depends on the this->_mesh dimension).
*/
SUPPORT *MEDMEM::SUPPORT::getBoundaryElements(medEntityMesh Entity) const throw (MEDEXCEPTION)
{
  const char * LOC = "SUPPORT *MEDMEM::SUPPORT::getBoundaryElements(medEntityMesh Entity) : ";
  BEGIN_OF_MED(LOC);
  int spaceDimension=_mesh->getSpaceDimension();
  medEntityMesh baseEntity=Entity;
  if (spaceDimension == 3)
    {
      if (Entity!=MED_EN::MED_FACE)
        {
          if(Entity==MED_EN::MED_NODE)
            baseEntity=MED_EN::MED_FACE;
          else
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 3D mesh for entity "<<Entity<<" !"));
        }
    }
  if (spaceDimension == 2)
    {
      if (Entity!=MED_EN::MED_EDGE)
        {
          if(Entity==MED_EN::MED_NODE)
            baseEntity=MED_EN::MED_EDGE;
          else
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not defined in 2D mesh for entity "<<Entity<<" !"));
        }
    }
  if(_isOnAllElts)
    return _mesh->getBoundaryElements(Entity);

  const MESH* mesh = _mesh->convertInMESH();
  const int * myConnectivityValue=mesh->getReverseConnectivity(MED_EN::MED_DESCENDING);
  const int * myConnectivityIndex=mesh->getReverseConnectivityIndex(MED_EN::MED_DESCENDING);
  int numberOf=mesh->getNumberOfElements(baseEntity,MED_ALL_ELEMENTS);
  const int *ids=_number->getValue();
  set<int> idsSet(ids,ids+_totalNumberOfElements);
  list<int> myElementsList;
  for (int i=0;i<numberOf;i++)
  {
    int nbOfDP1EntitySharing=0;
    if(idsSet.find(myConnectivityValue[myConnectivityIndex[i]-1])!=idsSet.end())
      nbOfDP1EntitySharing++;
    if(idsSet.find(myConnectivityValue[myConnectivityIndex[i]])!=idsSet.end())
      nbOfDP1EntitySharing++;
    if(nbOfDP1EntitySharing==1)
      myElementsList.push_back(i+1);
  }
  mesh->removeReference();

  if(Entity==MED_EN::MED_NODE)
  {
    return mesh->buildSupportOnNodeFromElementList(myElementsList,baseEntity);
  }
  else
  {
    return mesh->buildSupportOnElementsFromElementList(myElementsList,baseEntity);
  }
}

/*!
 * \brief Builds a nodal SUPPORT basing on nodes of this one
 */

SUPPORT* SUPPORT::buildSupportOnNode() const throw (MEDEXCEPTION)
{
  const char * LOC = "SUPPORT *MEDMEM::SUPPORT::buildSupportOnNode() : ";
  if ( !getMesh() )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"This SUPPORT has no mesh"));

  SUPPORT* nodalSupport = 0;
  if ( isOnAllElements() )
    {
      nodalSupport = const_cast<SUPPORT*>( getMesh()->getSupportOnAll( MED_EN::MED_NODE ));
      nodalSupport->addReference();
    }
  else
    {
      if ( !_numberOfElements )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No element numbers in a partial support"));
      string name("Support On Node built from ");
      name += getName();

      nodalSupport = new SUPPORT;
      nodalSupport->setMesh( getMesh());
      nodalSupport->setName( name );
      nodalSupport->setEntity( MED_EN::MED_NODE );
      nodalSupport->setEntity( getEntity() );

      const int * nums = _number->getValue();
      list<int> elems( nums, nums + _totalNumberOfElements );
      getMesh()->fillSupportOnNodeFromElementList( elems, nodalSupport );
    }
  return nodalSupport;
}

/*!
  Method that fills this and updates all its attributes in order to lye on the the listOfNode.
*/
void MEDMEM::SUPPORT::fillFromNodeList(const list<int>& listOfNode) throw (MEDEXCEPTION)
{
  setEntity(MED_EN::MED_NODE);
  clearDataOnNumbers();
  int size=listOfNode.size();
  int totalNbInMesh=_mesh->getNumberOfElements(_entity,MED_ALL_ELEMENTS);
  if(totalNbInMesh==size)
  {
    _isOnAllElts=true;
    update();
    return;
  }
  else
    _isOnAllElts=false;
  int numberOfGeometricType=1;
  medGeometryElement* geometricType=new medGeometryElement[1];
  geometricType[0]= MED_EN::MED_NONE;
  int *numberOfElements=new int[1];
  numberOfElements[0]=size;
  int *mySkyLineArrayIndex=new int[2];
  mySkyLineArrayIndex[0]=1;
  mySkyLineArrayIndex[1]=1+numberOfElements[0];
  int *tab=new int[numberOfElements[0]];
  int i=0;
  for(list<int>::const_iterator iter2=listOfNode.begin();iter2!=listOfNode.end();iter2++)
    tab[i++]=*iter2;
  MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY(1,numberOfElements[0],mySkyLineArrayIndex,tab,true);
  setNumberOfGeometricType(numberOfGeometricType);
  setGeometricType(geometricType);
  setNumberOfElements(numberOfElements);
  setNumber(mySkyLineArray);

  delete[] numberOfElements;
  delete[] geometricType;
}

/*
  Method created to factorize code. This method fills the current SUPPORT on entity 'entity' containing all the entities contained in
  elements 'listOfElt' of entity 'entity'. Warning this method should be called after both the attributes this->_mesh and this->_entity are correctly set.
*/
void MEDMEM::SUPPORT::fillFromElementList(const list<int>& listOfElt) throw (MEDEXCEPTION)
{
  clearDataOnNumbers();
  int size=listOfElt.size();
  int totalNbInMesh=_mesh->getNumberOfElements(_entity,MED_ALL_ELEMENTS);
  if(totalNbInMesh==size)
  {
    _isOnAllElts=true;
    update();
    return;
  }
  else
    _isOnAllElts=false;
  // Well, we must know how many geometric type we have found
  int * myListArray = new int[size] ;
  int id = 0 ;
  list<int>::const_iterator myElementsListIt ;
  for (myElementsListIt=listOfElt.begin();myElementsListIt!=listOfElt.end();myElementsListIt++)
    myListArray[id++]=(*myElementsListIt) ;
  int numberOfGeometricType ;
  medGeometryElement* geometricType ;
  int * numberOfElements ;
  int * mySkyLineArrayIndex ;

  int numberOfType = _mesh->getNumberOfTypes(_entity) ;
  if (numberOfType == 1)
  {
    numberOfGeometricType = 1 ;
    geometricType = new medGeometryElement[1] ;
    geometricType[0] = _mesh->getTypes(_entity)[0];
    numberOfElements = new int[1] ;
    numberOfElements[0] = size ;
    mySkyLineArrayIndex = new int[2] ;
    mySkyLineArrayIndex[0]=1 ;
    mySkyLineArrayIndex[1]=1+size ;
  }
  else // hemmm
  {
    map<medGeometryElement,int> theType ;
    for (myElementsListIt=listOfElt.begin();myElementsListIt!=listOfElt.end();myElementsListIt++)
    {
      medGeometryElement myType = _mesh->getElementType(_entity,*myElementsListIt) ;
      if (theType.find(myType) != theType.end() )
        theType[myType]+=1 ;
      else
        theType[myType]=1 ;
    }
    numberOfGeometricType = theType.size() ;
    geometricType = new medGeometryElement[numberOfGeometricType] ;
    numberOfElements = new int[numberOfGeometricType] ;
    mySkyLineArrayIndex = new int[numberOfGeometricType+1] ;
    int index = 0 ;
    mySkyLineArrayIndex[0]=1 ;
    map<medGeometryElement,int>::iterator theTypeIt ;
    for (theTypeIt=theType.begin();theTypeIt!=theType.end();theTypeIt++)
    {
      geometricType[index] = (*theTypeIt).first ;
      numberOfElements[index] = (*theTypeIt).second ;
      mySkyLineArrayIndex[index+1]=mySkyLineArrayIndex[index]+numberOfElements[index] ;
      index++ ;
    }
  }
  MEDSKYLINEARRAY * mySkyLineArray = new MEDSKYLINEARRAY(numberOfGeometricType,size,mySkyLineArrayIndex,myListArray,/*shallowCopy=*/true) ;
  setNumberOfGeometricType(numberOfGeometricType) ;
  setGeometricType(geometricType) ;
  setNumberOfElements(numberOfElements) ;
  setNumber(mySkyLineArray) ;

  delete[] numberOfElements;
  delete[] geometricType;
}

/*! \ifdef SUPPORT_advanced

\brief creates a MESH that contains only the elements in the current support.

The output mesh has no group, nor elements of connectivity lesser than that of the present support. Nodes are renumbered so that they are numberd from 1 to N in the new mesh. The order of the elements in the new mesh corresponds to that of the elements in the original support.
*/
MESH* SUPPORT::makeMesh() const
{
  const char* LOC = "SUPPORT::makeMesh(): ";
  if ( !_mesh )
    throw MED_EXCEPTION(STRING(LOC)<<" NULL mesh in support");
  if ( _entity == MED_EN::MED_NODE )
    throw MED_EXCEPTION(STRING(LOC)<<" unavailable for support on nodes");

  //Creating the new mesh

  MESHING* newmesh = new MESHING();
  newmesh->setName( STRING("MeshFromSupport_") << getName() );

  // set types info
  const medGeometryElement* types = getTypes();
  int nb_types = _numberOfGeometricType;
  newmesh->setNumberOfTypes   ( nb_types, MED_EN::MED_CELL );
  newmesh->setTypes           ( types, MED_EN::MED_CELL );
  newmesh->setNumberOfElements( _numberOfElements, MED_EN::MED_CELL);

  // browsing through elements to create a mapping between
  // the new nodes and the old nodes and to create nodal connectivity

  const MESH* mesh = _mesh->convertInMESH();
  const medGeometryElement* all_mesh_types = mesh->getTypes( _entity );
  const int *                    num_index = mesh->getGlobalNumberingIndex( _entity );

  map<int,int> oldnodes; // map old to new nodes
  if ( types[nb_types-1] == MED_POLYHEDRA )
    oldnodes.insert( make_pair( -1, -1 )); // for face separators
  int newid=1;
  for (int itype=0; itype < _numberOfGeometricType;itype++)
  {
    medGeometryElement type = types[itype];
    int nbelems = getNumberOfElements(type);

    // get connectivity info
    int shift = 1; // to pass from elem number to array index
    const int* conn = mesh->getConnectivity(MED_EN::MED_NODAL,_entity,MED_ALL_ELEMENTS);
    const int* index = mesh->getConnectivityIndex(MED_EN::MED_NODAL,_entity);
    int t = 0;
    while ( type != all_mesh_types[t] ) ++t;
    shift+= num_index[ t ] - num_index[0];
    index+= num_index[ t ] - num_index[0];

    // make and set new connectivity
    if ( _isOnAllElts  && _entity == MED_EN::MED_CELL )
    {
      newmesh->setConnectivity( MED_EN::MED_CELL, type, conn, index );
    }
    else // partial support or support of sub-entities
    {
      vector<int> new_conn, new_index;
      new_conn.reserve ( nbelems * (type % 100) );
      new_index.reserve( nbelems + 1 );
      new_index.push_back(1);

      const int * nums = _isOnAllElts ? 0 : getNumber( type );

      for (int i=0; i<nbelems; i++)
      {
        int ielem = nums ? nums[i]-shift : i;
        const int* elem_node = conn + index[ ielem   ] - 1;
        const int* nodes_end = conn + index[ ielem+1 ] - 1;
        for ( ; elem_node < nodes_end; ++elem_node )
        {
          // make new connectivity
          map<int,int>::iterator old_new = oldnodes.insert(make_pair( *elem_node, newid )).first;
          new_conn.push_back( old_new->second );
          if ( old_new->second == newid )
            newid++;
        }
        new_index.push_back( new_index.back() + index[ ielem+1 ] - index[ ielem ] );
      }
      // set new connectivity
      newmesh->setConnectivity( MED_EN::MED_CELL, type, & new_conn[0], & new_index[0] );
    }
  }

  //definition of coordinates

  int nb_nodes, spacedim = mesh->getSpaceDimension();  
  const double*oldcoords = mesh->getCoordinates(MED_EN::MED_FULL_INTERLACE);
  PointerOf<double> newcoords;

  if ( _isOnAllElts && _entity == MED_EN::MED_CELL )
  {
    nb_nodes = mesh->getNumberOfNodes();
    newcoords.set( oldcoords );
  }
  else
  {
    nb_nodes = oldnodes.size();
    newcoords.set( nb_nodes * spacedim);
    for (std::map<int,int>::const_iterator iter=oldnodes.begin(); iter!=oldnodes.end();iter++)
      std::copy( oldcoords+(iter->first-1 )*spacedim,
                 oldcoords+(iter->first   )*spacedim,
                 newcoords+(iter->second-1)*spacedim);
  }
  newmesh->setCoordinates(spacedim, nb_nodes, newcoords,
                          mesh->getCoordinatesSystem(), MED_EN::MED_FULL_INTERLACE);
  newmesh->setCoordinatesNames ( mesh->getCoordinatesNames() );
  newmesh->setCoordinatesUnits ( mesh->getCoordinatesUnits() );

  mesh->removeReference();

  return newmesh;
}
/*! 
  @}
*/

/*! set the reference _mesh to Mesh */
//--------------------------------------
void SUPPORT::setMesh(const GMESH *Mesh) const
  //--------------------------------------
{
  if(_mesh!=Mesh)
    {
      if(_mesh)
        _mesh->removeReference();
      _mesh=Mesh;
      _meshName = "";
      if(_mesh)
        _mesh->addReference();
    }
}

/*! returns the mesh name  */
//------------------------------------
string SUPPORT::getMeshName() const
  //------------------------------------
{
  if (_mesh)
    return _mesh->getName();
  else
    return _meshName;
}

/*!\if MEDMEM_ug 
\addtogroup SUPPORT_query
@{
\endif
*/

/*!
  This method returns the number of all elements of the type GeometricType.

  If isOnAllElements is false, it returns the number of elements in the
  support otherwise it returns number of elements in the mesh.

  Example : number of MED_TRIA3 or MED_ALL_ELEMENTS elements
  in support.

  Note : If SUPPORT is defined on MED_EN::MED_NODE, use MED_ALL_ELEMENTS as
         medGeometryElement GeometricType and it will return the number
         of nodes in the support (or in the mesh).
*/
//-----------------------------------------------------------------------------
int SUPPORT::getNumberOfElements(MED_EN::medGeometryElement GeometricType) const
  throw (MEDEXCEPTION)
//-----------------------------------------------------------------------------
{
  if (GeometricType==MED_EN::MED_ALL_ELEMENTS)
    return _totalNumberOfElements;
  for (int i=0;i<_numberOfGeometricType;i++)
    if (_geometricType[i]==GeometricType)
      return ( _totalNumberOfElements < 1 ) ? 0 : _numberOfElements[i];
  throw MEDEXCEPTION("Support::getNumberOfElements : Geometric type not found !") ;
}

  /*! Returns the total number of elements in the support. */
//-----------------------------------------------------------------------------
const int * SUPPORT::getNumberOfElements() const throw (MEDEXCEPTION) {
//-----------------------------------------------------------------------------
  return _numberOfElements;
}

/*!
  Returns index of element number.

  Note : See getConnectivityIndex for details.
*/
//-------------------------------------------
const int * SUPPORT::getNumberIndex() const
//-------------------------------------------
  throw (MEDEXCEPTION)
{
  /*  issue 0021167: [CEA 448] Supports management on all elements
  if (_isOnAllElts)
    throw MEDEXCEPTION("Support::getNumberIndex : Not defined, support is on all entity !") ;
  */
  if ( !_number )
    throw MEDEXCEPTION("Support::getNumberIndex : numbers not set !") ;
  return _number->getIndex() ;
}
/*! \if MEDMEM_ug
@}
\endif */

//---------------------------------------------------------------------
MEDSKYLINEARRAY * SUPPORT::getnumber() const
  throw (MEDEXCEPTION)
//---------------------------------------------------------------------
{
  if (_number==NULL)
    throw MEDEXCEPTION("Support::getnumber : Not defined !") ;
  return _number ;
}

//---------------------------------------------------------------------
MEDSKYLINEARRAY * SUPPORT::getnumberFromFile() const
  throw (MEDEXCEPTION)
//---------------------------------------------------------------------
{
  if (_number_fromfile==NULL)
    throw MEDEXCEPTION("Support::getnumberFromFile : Not defined !") ;
  return _number_fromfile ;
}

/*!
  Returns an array which contains all number of given medGeometryElement.

  Numbering is global, ie numbers are bounded by 1 and
  GMESH::getNumberOfElement(entity,MED_ALL_ELEMENTS) and not by 1 and
  GMESH::getNumberOfElement(entity,geomElement).

  Note : If SUPPORT is defined on MED_EN::MED_NODE, use MED_EN::MED_NONE
  medGeometryElement type.
*/
//---------------------------------------------------------------------
const int * SUPPORT::getNumber(MED_EN::medGeometryElement GeometricType) const
  throw (MEDEXCEPTION)
//---------------------------------------------------------------------
{
  /*  issue 0021167: [CEA 448] Supports management on all elements
  if (_isOnAllElts)
    throw MEDEXCEPTION("Support::getNumber : Not defined, support is on all entity !") ;
  */
  if (!_number)
  {
    if ( _isOnAllElts )
      //update();
      throw MEDEXCEPTION("Support::getNumber : not updated (update()) SUPPORT on all elements !") ;
    else if ( _totalNumberOfElements > 0 )
      throw MEDEXCEPTION("Support::getNumber : wrong support, _number not defined !") ;
    else
      return NULL;
  }
  if (GeometricType==MED_EN::MED_ALL_ELEMENTS)
    return _number->getValue() ;
  for (int i=0;i<_numberOfGeometricType;i++)
    if (_geometricType[i]==GeometricType)
      return _number->getI(i+1) ;
  throw MEDEXCEPTION("Support::getNumber : GeometricType not found !") ;
}

//---------------------------------------------------------------------
const int * SUPPORT::getNumberFromFile(MED_EN::medGeometryElement GeometricType) const
  throw (MEDEXCEPTION)
//---------------------------------------------------------------------
{
//   if (_isOnAllElts)
//     throw MEDEXCEPTION("Support::getNumberFromFile : Not defined, support is on all entity !") ;
  if (GeometricType==MED_EN::MED_ALL_ELEMENTS)
    return _number_fromfile->getValue() ;
  for (int i=0;i<_numberOfGeometricType;i++)
    if (_geometricType[i]==GeometricType)
      return _number_fromfile->getI(i+1) ;
  throw MEDEXCEPTION("Support::getNumberFromFile : GeometricType not found !") ;
}

/*! set the meshName if there is ni reference _mesh to Mesh */
//--------------------------------------
void SUPPORT::setMeshName(const std::string & meshName)
//--------------------------------------
{
  if (_mesh)
    throw MEDEXCEPTION("SUPPORT::setMeshName(const string & meshName) : Setting meshName is not possible when an associated mesh is set !") ;

  _meshName=meshName;
}

/*! set the attribute _entity to Entity */
//------------------------------------------
void SUPPORT::setEntity(MED_EN::medEntityMesh Entity)
{
  _entity=Entity;
  // 0021199: [CEA 458] MEDMEM::SUPPORT : geometric type when support is on node
  // set geometric type -> MED_EN::MED_NONE
  if ( _entity == MED_EN::MED_NODE )
    {
      _numberOfGeometricType = 1;
      const MED_EN::medGeometryElement nodeType = MED_EN::MED_NONE;
      _geometricType.set(0);
      setGeometricType( &nodeType );
    }
}

/*! set the attribute _numberOfGeometricType to NumberOfGeometricType */
//---------------------------------------------------------------------
void SUPPORT::setNumberOfGeometricType(int NumberOfGeometricType)
//---------------------------------------------------------------------
{
  _numberOfGeometricType=NumberOfGeometricType;

  _geometricType.set(0);
  _numberOfElements.set(0);
  _profilNames.resize( NumberOfGeometricType, "" );
}

/*! set the attribute _geometricType to geometricType */
//---------------------------------------------------------------------
void SUPPORT::setGeometricType(const MED_EN::medGeometryElement *GeometricType)
//---------------------------------------------------------------------
{
  if (!_geometricType)
    {
      _geometricType.set(_numberOfGeometricType, GeometricType);
      // 0021199: [CEA 458] MEDMEM::SUPPORT : geometric type when support is on node
      // geometric type must be MED_NONE
      if ( _entity == MED_EN::MED_NODE && _numberOfGeometricType == 1 && _geometricType[0] != MED_EN::MED_NONE )
        throw MEDEXCEPTION("SUPPORT::setGeometricType(), valid type for MED_EN::MED_NODE is MED_EN::MED_NONE ");
    }
  if (_profilNames.empty() || _profilNames[0].empty())
    {
      // giving a default value to profile names
      vector<string> prof_names( _numberOfGeometricType);
      string name = healName( _name );
      for (int itype=0; itype < _numberOfGeometricType; itype++)
        {
          ostringstream typestr;
          typestr<<name<<"_type"<<_geometricType[itype];
          prof_names[itype]=typestr.str();
        }
      _profilNames=prof_names;
    }
}

/*!
  Set the attribute _numberOfElements to NumberOfElements and
  calculate the total number of elements.
*/
//----------------------------------------------------------
void SUPPORT::setNumberOfElements(const int *NumberOfElements)
//----------------------------------------------------------
{
  if (_numberOfElements == NULL)
    {
      if (_numberOfGeometricType)
        _numberOfElements.set(_numberOfGeometricType,NumberOfElements);
      else
        _numberOfElements.set(0);
    }
  _totalNumberOfElements = 0 ;
  for (int i=0;i<_numberOfGeometricType;i++)
    _totalNumberOfElements+=_numberOfElements[i];
}

/*! set the attribute _number to Number */
//---------------------------------------------------
void SUPPORT::setNumber(MEDSKYLINEARRAY * Number)
//---------------------------------------------------
{
  /*  issue 0021167: [CEA 448] Supports management on all elements
  if ( _isOnAllElts )
    throw MEDEXCEPTION("SUPPORT::setNumber(MEDSKYLINEARRAY * Number) Support is on all elements") ;
  */
  if (_number != NULL) delete _number ;
  _number=Number;
}

/*! set the attribute _number with index and value arrays */
//---------------------------------------------------
void SUPPORT::setNumber(const int * index, const int* value, bool shallowCopy)
//---------------------------------------------------
{
  if (_number != NULL) delete _number ;
  _number= new MEDSKYLINEARRAY(_numberOfGeometricType,_totalNumberOfElements,index,value,shallowCopy);
}
