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

#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_CellModel.hxx"

#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_ModulusArray.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Utilities.hxx"
#include <iomanip>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

// Enlarge the vector if necessary to insert the element
static inline void insert_vector(vector<int> &Vect, int Indice, int Element)
{
  if (Indice >=(int) Vect.capacity())
    Vect.reserve(Indice + 1000);

  if (Indice >=(int) Vect.size())
    Vect.resize(Indice+1);

  Vect[Indice] = Element;
}

void mergeOrderedTabs(const int *tab1, int lgth1, const int *tab2, int lgth2, int *result, int& lgth);

void mergeOrderedTabs(const int *tab1, int lgth1, const int *tab2, int lgth2, int *result, int& lgth)
{
  int cpt[2]={0,0};
  lgth=0;
  unsigned char switcher=0;
  const int *tabS[2]={tab1,tab2};
  while(cpt[0]<lgth1 && cpt[1]<lgth2)
  {
    if(tabS[1-switcher][cpt[1-switcher]]<tabS[switcher][cpt[switcher]])
      cpt[1-switcher]++;
    else if(tabS[1-switcher][cpt[1-switcher]]>tabS[switcher][cpt[switcher]])
      switcher=1-switcher;
    else
    {
      int tmp=tabS[switcher][cpt[switcher]];
      cpt[switcher]++; cpt[1-switcher]++;
      result[lgth++]=tmp;
    }
  }
}

/*!
  Default Constructor. /n
  Default for Entity is MED_EN::MED_CELL and type of Connectivity is MED_NODAL */
//--------------------------------------------------------------//
CONNECTIVITY::CONNECTIVITY(medEntityMesh Entity /* =MED_EN::MED_CELL */) :
  //--------------------------------------------------------------//
  _entity(Entity),
  _typeConnectivity(MED_EN::MED_NODAL),
  _numberOfTypes(0),
  _geometricTypes((medGeometryElement*)NULL),
  _type((CELLMODEL*)NULL),
  _entityDimension(0),
  _numberOfNodes(0),
  _count((int*)NULL),
  _nodal((MEDSKYLINEARRAY*)NULL),
  _descending((MEDSKYLINEARRAY*)NULL),
  _reverseNodalConnectivity((MEDSKYLINEARRAY*)NULL),
  _reverseDescendingConnectivity((MEDSKYLINEARRAY*)NULL),
  _neighbourhood((MEDSKYLINEARRAY*)NULL),
  _constituent((CONNECTIVITY*)NULL),
  _isDescendingConnectivityPartial(false)
{
  const char LOC[]= "CONNECTIVITY(medEntityMesh Entity=MED_EN::MED_CELL)";
  BEGIN_OF_MED(LOC);
  MESSAGE_MED("CONNECTIVITY(medEntityMesh Entity=MED_EN::MED_CELL)");
  _count = new int[1];
  _count[0]=1;
  END_OF_MED(LOC);
}

/*!
  Constructor. /n
  Default for Entity is MED_EN::MED_CELL */
//------------------------------------------------------------------------------//
CONNECTIVITY::CONNECTIVITY(int numberOfTypes,medEntityMesh Entity /* =MED_EN::MED_CELL */):
  //------------------------------------------------------------------------------//
  _entity(Entity),
  _typeConnectivity(MED_EN::MED_NODAL),
  _numberOfTypes(numberOfTypes),
  _entityDimension(0),
  _numberOfNodes(0),
  _nodal((MEDSKYLINEARRAY*)NULL),
  _descending((MEDSKYLINEARRAY*)NULL),
  _reverseNodalConnectivity((MEDSKYLINEARRAY*)NULL),
  _reverseDescendingConnectivity((MEDSKYLINEARRAY*)NULL),
  _neighbourhood((MEDSKYLINEARRAY*)NULL),
  _constituent((CONNECTIVITY*)NULL),
  _isDescendingConnectivityPartial(false)
{
  MESSAGE_MED("CONNECTIVITY(int numberOfTypes,medEntityMesh Entity=MED_EN::MED_CELL)");
  _geometricTypes = new medGeometryElement[numberOfTypes];
  _type = new CELLMODEL[numberOfTypes];
  _count = new int[numberOfTypes+1];
  _count[0]=1;
  if ( numberOfTypes )
    _count[ numberOfTypes-1 ] = 0; // to know whether _count is set or not
}

/*!
  Copy Constructor.
*/
//------------------------------------------------------//
CONNECTIVITY::CONNECTIVITY (const CONNECTIVITY & m):
  //----------------------------------------------------//
  _entity           (m._entity),
  _typeConnectivity (m._typeConnectivity),
  _numberOfTypes    (m._numberOfTypes),
  _entityDimension  (m._entityDimension),
  _numberOfNodes    (m._numberOfNodes),
  _isDescendingConnectivityPartial(m._isDescendingConnectivityPartial)
{
  if (m._geometricTypes != NULL)
  {
    _geometricTypes = new medGeometryElement[_numberOfTypes];
    memcpy(_geometricTypes,m._geometricTypes,_numberOfTypes*sizeof(medGeometryElement));
  }
  else
    _geometricTypes = (medGeometryElement *) NULL;

  if (m._type != NULL)
  {
    _type = new CELLMODEL[_numberOfTypes];
    for (int i=0;i<_numberOfTypes;i++)
      _type[i] = CELLMODEL(m._type[i]);
  }
  else
    _type = (CELLMODEL *) NULL;

  if (m._count != NULL)
  {
    _count = new int[_numberOfTypes+1];
    memcpy(_count,m._count,(_numberOfTypes+1)*sizeof(int));
  }
  else
    _count = (int *) NULL;

  if (m._nodal != NULL)
    _nodal = new MEDSKYLINEARRAY(* m._nodal);
  else
    _nodal = (MEDSKYLINEARRAY *) NULL;

  if (m._descending != NULL)
    _descending = new MEDSKYLINEARRAY(* m._descending);
  else
    _descending = (MEDSKYLINEARRAY *) NULL;

  if (m._reverseNodalConnectivity != NULL)
    _reverseNodalConnectivity = new MEDSKYLINEARRAY(* m._reverseNodalConnectivity);
  else
    _reverseNodalConnectivity = (MEDSKYLINEARRAY *) NULL;

  if (m._reverseDescendingConnectivity != NULL)
    _reverseDescendingConnectivity = new MEDSKYLINEARRAY(* m._reverseDescendingConnectivity);
  else
    _reverseDescendingConnectivity = (MEDSKYLINEARRAY *) NULL;

  if (m._neighbourhood != NULL)
    _neighbourhood = new MEDSKYLINEARRAY(* m._neighbourhood);
  else
    _neighbourhood = (MEDSKYLINEARRAY *) NULL;

  if (m._constituent != NULL)
    _constituent = new CONNECTIVITY(* m._constituent);
  else
    _constituent = (CONNECTIVITY *) NULL;
}

/*!
  Destructor./n
  desallocates existing pointers */
//----------------------------//
CONNECTIVITY::~CONNECTIVITY()
//----------------------------//
{
  MESSAGE_MED("Destructeur de CONNECTIVITY()");

  if (_geometricTypes != NULL)
    delete [] _geometricTypes;
  if (_type != NULL)
    delete [] _type;
  if (_count != NULL)
    delete [] _count;
  if (_nodal != NULL)
    delete _nodal;
  if (_descending != NULL)
    delete _descending;
  if (_reverseNodalConnectivity != NULL)
    delete _reverseNodalConnectivity;
  if (_reverseDescendingConnectivity != NULL)
    delete _reverseDescendingConnectivity;
  if (_constituent != NULL)
    delete _constituent;
}

/*!
  set _constituent to Constituent
  be aware desallocation of _constituent is done by CONNECTIVITY:~CONNECTIVITY
  throws an exception if Constituent = MED_EN::MED_CELL
  A DOCUMENTER
*/
//----------------------------------------------------------//
void CONNECTIVITY::setConstituent(CONNECTIVITY * Constituent)
  throw (MEDEXCEPTION)
//----------------------------------------------------------//
{
  medEntityMesh Entity = Constituent->getEntity();
  if (Entity == MED_EN::MED_CELL)
    throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setConstituent : could not set constituent on MED_EN::MED_CELL !"));

  if ((Entity == MED_EN::MED_EDGE)&(_entityDimension == 3))
  {
    if (_constituent == NULL)
      throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setConstituent : Entity not found !"));
    _constituent->setConstituent(Constituent);
  }
  else
  {
    delete _constituent;
    _constituent = Constituent;
  }
}

/*!
   Return _constituent of given entity
*/
//----------------------------------------------------------//
const CONNECTIVITY *CONNECTIVITY::getConstituent(MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION)
//----------------------------------------------------------//
{
  if(Entity==MED_EN::MED_FACE && _entityDimension==3)
    return _constituent;
  if(Entity==MED_EN::MED_EDGE && _entityDimension==2)
    return _constituent;
  if(Entity==MED_EN::MED_EDGE && _entityDimension==3)
    {
      if ( _constituent )
        return _constituent->getConstituent(MED_EN::MED_EDGE);
      else
        throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::getConstituent : FACE connectivity not defined!"));
    }
  throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::getConstituent : Invalid request !"));
}

/*! Duplicated Types array in CONNECTIVITY object. */
//--------------------------------------------------------------------//
void CONNECTIVITY::setGeometricTypes(const medGeometryElement * Types,
                                     const medEntityMesh Entity)
  throw (MEDEXCEPTION)
//--------------------------------------------------------------------//
{
  if (Entity == _entity)
    for (int i=0; i<_numberOfTypes; i++)
    {
      _geometricTypes[i] = Types[i];
      _type[i] = CELLMODEL(_geometricTypes[i]);
      if ( _type[i].getDimension() > _entityDimension )
        _entityDimension = _type[i].getDimension();
    }
  else
  {
    if (_constituent == NULL)
      throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setGeometricTypes : Entity not found !"));
    _constituent->setGeometricTypes(Types,Entity);
  }
}

/*! Set number of elements of each type of given entity */
//--------------------------------------------------------------------//
void CONNECTIVITY::setCount(const int *         Count,
                            const medEntityMesh Entity)
  throw (MEDEXCEPTION)
//--------------------------------------------------------------------//
{
  if (Entity == _entity)
  {
    // this exception was added at 1.18.2.2.2.4 for NPAL17043: "Correction of
    // MEDMEM CPPUNIT tests. Integrated a work of V. Bergeaud and A. Geay."
    // and removed for PAL19744: "regression in MEDMEM_Connectivity.cxx"
    // Commenting this exception at least looks safe as the case
    // _numberOfTypes==0 is previewed here
    //if (_numberOfTypes==0)
    //  throw MEDEXCEPTION("Number of Types was not set before setting counts");
    int * index = new int[Count[_numberOfTypes]];
    index[0]=1;
    _count[0]=1;
    for (int i=0; i<_numberOfTypes; i++)
    {
      _count[i+1] = Count[i+1];
      int NumberOfNodesPerElement = _type[i].getNumberOfNodes() ;
      if ( _geometricTypes[i] != MED_POLYGON && _geometricTypes[i] != MED_POLYHEDRA )
        for (int j=_count[i]; j<_count[i+1]; j++)
          index[j] = index[j-1]+NumberOfNodesPerElement;
      else
        index[_count[_numberOfTypes]-1] = index[_count[_numberOfTypes-1]-1];
    }
    // allocate _nodal
    if (_nodal != NULL) delete _nodal;
    if (_numberOfTypes != 0)
    {
      _nodal = new MEDSKYLINEARRAY(_count[_numberOfTypes]-1,index[_count[_numberOfTypes]-1]-1);
      _nodal->setIndex(index);
    }
    else
      _nodal=0;
    delete [] index;
  }
  else
  {
    if (_constituent == NULL)
      throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setCount : Entity not found !"));
    _constituent->setCount(Count,Entity);
  }
}

//--------------------------------------------------------------------//
void CONNECTIVITY::setNodal(const int *              Connectivity,
                            const medEntityMesh      Entity,
                            const medGeometryElement Type,
                            const int *              PolyConnectivityIndex)
  throw (MEDEXCEPTION)
//--------------------------------------------------------------------//
{
  if (_entity == Entity)
  {
    // find geometric type
    bool find = false;
    for (int i=0; i<_numberOfTypes; i++)
      if (_geometricTypes[i] == Type)
      {
        if ( _count[i+1] == 0 )
          throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setNodal : "
                                       "number of elements by type must be set before!"));
        if ( _count[i+1] < _count[i] )
          throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setNodal : "
                                       "invalid number of elements by type!"));

        find = true;
        if ( Type == MED_POLYGON || Type == MED_POLYHEDRA )
        {
          if (! PolyConnectivityIndex )
            throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setNodal : PolyConnectivityIndex must be provided for poly- elements !"));
          if ( _numberOfTypes == 1 )
          {
            delete _nodal;
            _nodal = new MEDSKYLINEARRAY(_count[_numberOfTypes]-1,
                                         PolyConnectivityIndex[_count[_numberOfTypes]-1]-1,
                                         PolyConnectivityIndex, Connectivity);
          }
          else
          {
            // append poly connectivity to the present one
            int nbpoly = getNumberOf( Entity, Type );
            int polyconnlen = PolyConnectivityIndex[nbpoly] - PolyConnectivityIndex[0];
            int newconnlen = _nodal->getLength() + polyconnlen;
            // create new MEDSKYLINEARRAY
            int * newconn  = new int[ newconnlen ];
            int * newindex = new int[ _nodal->getNumberOf() + 1 ];
            MEDSKYLINEARRAY* newnodal = new MEDSKYLINEARRAY(_count[_numberOfTypes]-1,
                                                            newconnlen,newindex,newconn,
                                                            /*shallowCopy=*/true);
            // copy old values
            memcpy(newconn, _nodal->getValue(),sizeof(int)*(_nodal->getLength()));
            memcpy(newindex,_nodal->getIndex(),sizeof(int)*(_nodal->getNumberOf() - nbpoly + 1));
            // append new ones
            memcpy(newconn+_nodal->getLength(),Connectivity, sizeof(int)*polyconnlen);
            int* polyindex = newindex + _count[i] - 1;
            int delta = polyindex[0] - PolyConnectivityIndex[0];
            for( int j=0;j<nbpoly; j++)
              polyindex[j+1] = PolyConnectivityIndex[j+1] + delta;
            delete _nodal;
            _nodal = newnodal;
          }
        }
        else
        {
          const int* index = _nodal->getIndex();
          for( int j=_count[i];j<_count[i+1]; j++)
            _nodal->setI(j,Connectivity+index[j]-index[_count[i]]);
        }
      }
    if (!find)
      throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setNodal : geometric type not found"));
  }
  else
  {
    if (_constituent == NULL)
      throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::setNodal : Entity not found !"));
    _constituent->setNodal(Connectivity,Entity,Type,PolyConnectivityIndex);
  }
}

/*! A DOCUMENTER */
//------------------------------------------------------------------------------------------//
void CONNECTIVITY::calculateConnectivity(medConnectivity ConnectivityType, medEntityMesh Entity)
//------------------------------------------------------------------------------------------//
{
  MESSAGE_MED("CONNECTIVITY::calculateConnectivity");

  // a temporary limitation due to calculteDescendingConnectivity function !!!
  if ((_entityDimension==3) & (Entity==MED_EN::MED_EDGE))
    throw MEDEXCEPTION("CONNECTIVITY::calculateConnectivity : Could not build EDGE !");

  if (Entity==_entity)
  {
    if (ConnectivityType==MED_EN::MED_NODAL)
      calculateNodalConnectivity();
    else
      if (Entity==MED_EN::MED_CELL)
        calculateDescendingConnectivity();
      else
        throw MEDEXCEPTION("CONNECTIVITY::calculateConnectivity : Could not build DESCENDING !");
  }
  if (Entity!=_entity) {
    calculateDescendingConnectivity();
    if (_entityDimension == 2 || _entityDimension == 3)
      _constituent->calculateConnectivity(ConnectivityType,Entity);
  }
}

//------------------------------------------------------------//
void CONNECTIVITY::updateFamily(const vector<FAMILY*>& myFamilies)
//------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::updateFamily(vector<FAMILY*>) ";
  int numberOfFamilies = myFamilies.size();
  if (numberOfFamilies == 0 || _constituent == NULL)
    return;
  // does we do an update ?
  if ((_constituent != NULL) && (_descending != NULL))
    return;
  if (myFamilies[0]->getEntity() != _constituent->getEntity())
    return;

  CONNECTIVITY * oldConstituent = _constituent;
  _constituent = (CONNECTIVITY *)NULL;
  if (oldConstituent->_nodal==NULL)
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"We have no nodal connectivity of sub cell"));

  const int       oldNumberOfFace = oldConstituent->_nodal->getNumberOf();
  const int * oldConstituentValue = oldConstituent->_nodal->getValue();
  const int * oldConstituentIndex = oldConstituent->_nodal->getIndex();

  int * renumberingFromOldToNew = new int [oldNumberOfFace+1];

  calculateDescendingConnectivity();//perform calculation of descending connectivity to update all connectivities after having taken into account of direction of faces...
  _constituent->calculateReverseNodalConnectivity();//getting d-1 nodal connectivity to get new face numbers from nodes numbers...

  const int * reverseFaceNodal      = _constituent->getReverseNodalConnectivity();
  const int * reverseFaceNodalIndex = _constituent->getReverseNodalConnectivityIndex();

  CELLMODEL * aCELLMODEL = &oldConstituent->_type[0];
  //int newNumberOfFaces = _constituent->getNumberOf(_constituent->getEntity(),MED_ALL_ELEMENTS);
  for ( int iOldFace = 0; iOldFace < oldNumberOfFace; iOldFace++ )
  {
    // Retrieving new number of iOldFace face...

    const int *nodesOfCurrentFaceOld=oldConstituentValue+oldConstituentIndex[iOldFace]-1;
    int    nbOfNodesOfCurrentFaceOld=oldConstituentIndex[iOldFace+1]-oldConstituentIndex[iOldFace];

    // get all faces (newFaceNb) around the first node of iOldFace
    int sizeOfNewFaceNb=reverseFaceNodalIndex[nodesOfCurrentFaceOld[0]]-reverseFaceNodalIndex[nodesOfCurrentFaceOld[0]-1];
    int *newFaceNb=new int[sizeOfNewFaceNb];
    memcpy(newFaceNb,reverseFaceNodal+reverseFaceNodalIndex[nodesOfCurrentFaceOld[0]-1]-1,sizeOfNewFaceNb*sizeof(int));//all faces candidates are in newFaceNb !!!

    // loop on the rest nodes of iOldFace to find, among newFaceNb, a sole face including all nodes
    for(int curNode=1;curNode<nbOfNodesOfCurrentFaceOld && sizeOfNewFaceNb>1;curNode++)
    {
      const int *newFacesNbForCurNode=reverseFaceNodal+reverseFaceNodalIndex[nodesOfCurrentFaceOld[curNode]-1]-1;
      int sizeOfNewFacesNbForCurNode=reverseFaceNodalIndex[nodesOfCurrentFaceOld[curNode]]-reverseFaceNodalIndex[nodesOfCurrentFaceOld[curNode]-1];
      for(int i=0; i<sizeOfNewFaceNb; )
      {
        bool found=false;
        for(int j=0; j < sizeOfNewFacesNbForCurNode && !found; j++)
          found = (newFacesNbForCurNode[j]==newFaceNb[i]);

        if(found)
          i++;
        else
          newFaceNb[i]=newFaceNb[--sizeOfNewFaceNb]; // set the last face at place of not found one
      }
    }
    if(sizeOfNewFaceNb!=1)
      throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"unexisting face specified!!!"));
    renumberingFromOldToNew[iOldFace]=newFaceNb[0];
    delete [] newFaceNb;
    //end of retrieving a new number of a face...

    // Compare nodes of a new face and those of an old one;
    // for the second order elements, only vertex nodes are compared
    int nbOfNodesOfCurrentFaceNew;
    const int *nodesOfCurrentFaceNew=_constituent->getConnectivityOfAnElement(MED_EN::MED_NODAL,_constituent->getEntity(), renumberingFromOldToNew[iOldFace], nbOfNodesOfCurrentFaceNew );
    if ( aCELLMODEL && aCELLMODEL->getNumberOfNodes() != nbOfNodesOfCurrentFaceOld )
    {
      // type changed, find a corresponding CELLMODEL
      int iType = 2; // 1-st type is already used at loop beginning
      while ( iOldFace + 1 >= oldConstituent->_count[ iType ]) // check next type
        ++iType;
      if ( oldConstituent->_numberOfTypes >= iType &&
           oldConstituent->_type[ iType-1 ].getNumberOfNodes() > 0 )
        aCELLMODEL = &oldConstituent->_type[ iType-1 ];
      else
        aCELLMODEL = 0;
    }
    int nbOfVertices = aCELLMODEL ? aCELLMODEL->getNumberOfVertexes() : nbOfNodesOfCurrentFaceOld;
    MEDMODULUSARRAY modulusArrayOld(nbOfVertices,nbOfNodesOfCurrentFaceOld,nodesOfCurrentFaceOld);
    int nbOfVerticesNew = std::min( nbOfVertices, nbOfNodesOfCurrentFaceNew );
    MEDMODULUSARRAY modulusArrayNew(nbOfVerticesNew,nbOfNodesOfCurrentFaceNew,nodesOfCurrentFaceNew);
    int retCompareNewOld=modulusArrayNew.compare(modulusArrayOld);
    if(retCompareNewOld==0)
      throw MED_EXCEPTION(LOCALIZED(STRING(LOC)<<"Uncompatible given user face with calculated existing faces"));
    if(retCompareNewOld==-1)
      invertConnectivityForAFace(renumberingFromOldToNew[iOldFace],nodesOfCurrentFaceOld);
  }

  // Updating the Family
  for(vector<FAMILY*>::const_iterator iter=myFamilies.begin();iter!=myFamilies.end();iter++)
    (*iter)->changeElementsNbs(_constituent->getEntity(), renumberingFromOldToNew);

  // FIX PAL13414:
  if ( _constituent && !_constituent->_constituent ) {
    _constituent->_constituent = oldConstituent->_constituent;
    oldConstituent->_constituent = NULL;
  }
  // END FIX PAL13414:

  delete oldConstituent;
  delete [] renumberingFromOldToNew;
  return;
}

/*!  Give descending or nodal connectivity.

  You must give a %medEntityMesh (ie:MED_EN::MED_EDGE)
  and a %medGeometryElement (ie:MED_SEG3).  */
//---------------------------------------------------------------------------------//
const int * MEDMEM::CONNECTIVITY::getConnectivity(medConnectivity    ConnectivityType,
                                                  medEntityMesh      Entity,
                                                  medGeometryElement Type) const
//----------------------------------------------------------------------------------//
  {
    const char LOC[]= "CONNECTIVITY::getConnectivity";
    //  BEGIN_OF_MED(LOC);

    MEDSKYLINEARRAY * Connectivity;

    if(Entity==_entity)
      {
        if(ConnectivityType==MED_EN::MED_NODAL)
          {
            calculateNodalConnectivity();
            Connectivity=_nodal;
          }
        else
          {
            calculateDescendingConnectivity();
            Connectivity=_descending;
          }
        if(Connectivity!=NULL)
          if(Type==MED_ALL_ELEMENTS)
            return Connectivity->getValue();
          else
            {
              for(int i=0; i<_numberOfTypes; i++)
                if (_geometricTypes[i]==Type)
                  //return Connectivity->getI(i+1);
                  return Connectivity->getI(_count[i]);
              throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Type not found !"));
            }
        else
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Connectivity not defined !"));
      }
    else
      if(_constituent != NULL)
        return _constituent->getConnectivity(ConnectivityType,Entity,Type);
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
  }

//--------------------------------------------------------------------------
int CONNECTIVITY::getConnectivityLength(medConnectivity    ConnectivityType,
                                        medEntityMesh      Entity,
                                        medGeometryElement Type) const
//--------------------------------------------------------------------------
{
  const char * LOC = "CONNECTIVITY::getConnectivity";
  BEGIN_OF_MED(LOC);

  MEDSKYLINEARRAY * Connectivity;
  if (Entity == _entity) {
    if (ConnectivityType == MED_EN::MED_NODAL)
    {
      calculateNodalConnectivity();
      Connectivity = _nodal;
    }
    else
    {
      calculateDescendingConnectivity();
      Connectivity = _descending;
    }

    if (Connectivity != NULL) {
      if (Type == MED_ALL_ELEMENTS) {
        return Connectivity->getLength();
      }
      else {
        for (int i=0; i<_numberOfTypes; i++)
          if (_geometricTypes[i]==Type)
            // issue 19983
            //return (_count[i+1]-_count[i])*getType(Type).getNumberOfNodes();
          {
            const int *ind=Connectivity->getIndex();
            return ind[_count[i+1]-1]-ind[_count[i]-1];
          }
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Type not found !"));
      }
    }
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Connectivity not defined !"));
  }
  else
    if (_constituent != NULL)
      return _constituent->getConnectivityLength(ConnectivityType,Entity,Type);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
}

/*!  Give morse index array to use with
  getConnectivity(mode,entity,MED_ALL_ELEMENTS).

  Each value give start index for corresponding entity in connectivity array.

  Example : i-th element, j-th node of it :
  - In C mode : Connectivity[ConnectivityIndex[i]-1+j-1]
  - In fortran mode : Connectivity[ConnectivityIndex[i]+j] */
//-----------------------------------------------------------------------------------------------//
const int * CONNECTIVITY::getConnectivityIndex(medConnectivity ConnectivityType,
                                               medEntityMesh Entity) const
//-----------------------------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getConnectivityIndex";

  MEDSKYLINEARRAY * Connectivity;
  if (Entity==_entity) {

    if (ConnectivityType==MED_EN::MED_NODAL)
      Connectivity=_nodal;
    else
      Connectivity=_descending;

    if (Connectivity!=NULL)
      return Connectivity->getIndex();
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Connectivity not defined !"));

  } else
    if (_constituent != NULL)
      return _constituent->getConnectivityIndex(ConnectivityType,Entity);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
}


/*!
 * \brief Returns CELLMODEL of a given geometrical type
 */
//--------------------------------------------------------------//
const CELLMODEL & CONNECTIVITY::getType(medGeometryElement Type) const
//--------------------------------------------------------------//
{

  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_EN::MED_NONE))
    throw MEDEXCEPTION("CONNECTIVITY::getType : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE !");
  for (int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i];
  throw MEDEXCEPTION("CONNECTIVITY::getType :  medGeometryElement not found !");
}

/*!  Returns the number of elements of type %medGeometryElement.
  Note : not implemented for MED_ALL_ELEMENTS nor for MED_NONE */
//------------------------------------------------------------------------//
int CONNECTIVITY::getNumberOfNodesInType(medGeometryElement Type) const
//------------------------------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::getNumberOfNodesInType";
  BEGIN_OF_MED(LOC);

  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_EN::MED_NONE))
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE!"));
  for (int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i].getNumberOfNodes();
  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement not found !"));
}

/*!  Returns the number of geometric sub cells of %medGeometryElement type.
  not implemented for MED_ALL_ELEMENTS nor for MED_NONE */
//------------------------------------------------------------------------//
int CONNECTIVITY::getNumberOfSubCellInType(medGeometryElement Type) const
//------------------------------------------------------------------------//
{
  if ((Type==MED_ALL_ELEMENTS) || (Type==MED_EN::MED_NONE))
    throw MEDEXCEPTION("CONNECTIVITY::getNumberOfSubCell : medGeometryElement must be different of MED_ALL_ELEMENTS and MED_NONE!");
  for (int i=0; i<_numberOfTypes; i++)
    if (_geometricTypes[i]==Type)
      return _type[i].getNumberOfConstituents(1);
  throw MEDEXCEPTION("CONNECTIVITY::getNumberOfSubCell : medGeometryElement not found !");
}

/*!  Returns the number of elements of type %medGeometryElement.

  Note :
  - Implemented for MED_ALL_ELEMENTS
  - Not implemented for MED_ALL_ENTITIES (A VERIFIER)
  - Not implemented for MED_NONE */
//-----------------------------------------------------------------------------------//
int CONNECTIVITY::getNumberOf(medEntityMesh Entity, medGeometryElement Type) const
//-----------------------------------------------------------------------------------//
{
  if(Entity==MED_EN::MED_NODE)
    return _numberOfNodes;
  if (Entity==_entity) {
    if (Type==MED_EN::MED_NONE)
      return 0; // not defined !
    //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement must be different of MED_NONE"));
    if (!existConnectivity(MED_EN::MED_NODAL,Entity) && !existConnectivity(MED_EN::MED_DESCENDING,Entity))
      return 0;
    if (Type==::MED_ALL_ELEMENTS)
      return _count[_numberOfTypes]-1;
    for (int i=0; i<_numberOfTypes; i++)
      if (_geometricTypes[i]==Type)
        return (_count[i+1] - _count[i]);
    //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : medGeometryElement not found !"));
  } else
    if (_constituent != NULL)
      return _constituent->getNumberOf(Entity,Type);

  return 0; // valid if they are nothing else !
  //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" : Entity not defined !"));
}

/*! A DOCUMENTER */
//--------------------------------------------------------------//
const int* CONNECTIVITY::getValue(medConnectivity TypeConnectivity,
                                  medGeometryElement Type) const
//--------------------------------------------------------------//
{
  if (TypeConnectivity == MED_EN::MED_NODAL)
  {
    calculateNodalConnectivity();
    if (Type==MED_ALL_ELEMENTS)
      return _nodal->getValue();
    for (int i=0; i<_numberOfTypes; i++)
      if (_geometricTypes[i]==Type)
        return _nodal->getI(_count[i]);
  }
  else
  {
    calculateDescendingConnectivity();
    if (Type==MED_ALL_ELEMENTS)
      return _descending->getValue();
    for (int i=0; i<_numberOfTypes; i++)
      if (_geometricTypes[i]==Type)
        return _descending->getI(_count[i]);
  }
  throw MEDEXCEPTION("Not found");
}

/*! A DOCUMENTER */
//---------------------------------------------------------------------//
const int* CONNECTIVITY:: getValueIndex(medConnectivity TypeConnectivity) const
//---------------------------------------------------------------------//
{
  if (TypeConnectivity == MED_EN::MED_NODAL)
  {
    calculateNodalConnectivity();
    return _nodal->getIndex();
  }
  else
  {
    calculateDescendingConnectivity();
    return _descending->getIndex();
  }
}

/*! Not yet implemented */
//----------------------------------------------//
const int* CONNECTIVITY:: getNeighbourhood() const
//----------------------------------------------//
{
  throw MEDEXCEPTION("CONNECTIVITY::getNeighbourhood : Not implemented");
}

/*! Returns an array which contains, for each node, all cells
  arround it. */
//-------------------------------------------------//
const int* CONNECTIVITY::getReverseNodalConnectivity() const
//-------------------------------------------------//
{
  calculateReverseNodalConnectivity();
  return _reverseNodalConnectivity->getValue();
}

/*!  Give index array to use with getReverseConnectivity(MED_NODAL).
  It is unusefull with MED_DESCENDING mode, because we have allways two cells.  */
//-------------------------------------------------------//
const int* CONNECTIVITY::getReverseNodalConnectivityIndex() const
//-------------------------------------------------------//
{
  calculateReverseNodalConnectivity();
  return _reverseNodalConnectivity->getIndex();
}

/*! Returns an array which contains, for each face (or edge),
  the 2 cells of each side. First is cell which face normal is outgoing.
  arround it. */
//------------------------------------------------------//
const int* CONNECTIVITY::getReverseDescendingConnectivity() const
//------------------------------------------------------//
{
  // it is in _constituent connectivity only if we are in MED_EN::MED_CELL
  // (we could not for instance calculate face-edge connectivity !)
  if (_entity!=MED_EN::MED_CELL)
    throw MEDEXCEPTION("CONNECTIVITY::getReverseDescendingConnectivity : Error Only in MED_EN::MED_CELL connectivity");

  // we need descending connectivity
  calculateDescendingConnectivity();
  if (_reverseDescendingConnectivity==NULL)
    _reverseDescendingConnectivity=_descending->makeReverseArray();

  return _reverseDescendingConnectivity->getValue();
}

/*! calculate the reverse descending Connectivity
  and returns the index  ( A DOCUMENTER MIEUX)*/
//-----------------------------------------------------------//
const int* CONNECTIVITY::getReverseDescendingConnectivityIndex() const
//-----------------------------------------------------------//
{
  // it is in _constituent connectivity only if we are in MED_EN::MED_CELL
  if (_entity!=MED_EN::MED_CELL)
    throw MEDEXCEPTION("CONNECTIVITY::getReverseDescendingConnectivityIndex : Error Only in MED_EN::MED_CELL connectivity");

  // we need descending connectivity
  calculateDescendingConnectivity();
  return _reverseDescendingConnectivity->getIndex();
}

/*! A DOCUMENTER (et a finir ???) */
//--------------------------------------------//
void CONNECTIVITY::calculateNodalConnectivity() const
//--------------------------------------------//
{
  if (_nodal==NULL)
  {
    if (_descending==NULL)
      throw MEDEXCEPTION("CONNECTIVITY::calculateNodalConnectivity : No connectivity found !");
    // calculate _nodal from _descending
  }
}

/*! If not yet done, calculate the nodal Connectivity
  and the reverse nodal Connectivity*/
//---------------------------------------------------//
void CONNECTIVITY::calculateReverseNodalConnectivity() const
//---------------------------------------------------//
{
  const char* LOC = "CONNECTIVITY::calculateReverseNodalConnectivity : ";
  BEGIN_OF_MED(LOC);

  SCRUTE_MED(_nodal);
  SCRUTE_MED(_reverseNodalConnectivity);

  if (_nodal==NULL)
    calculateNodalConnectivity();

  if(_reverseNodalConnectivity==NULL)
  {
    vector <vector <int> > reverse_connectivity;
    reverse_connectivity.resize(_numberOfNodes+1);

    // Treat all cells types

    const int *index=_nodal->getIndex();
    const int *conn =_nodal->getValue();
    int cell_number=1;
    for (int j = 0; j < _numberOfTypes; j++)
    {
      for (int k = _count[j]; k < _count[j+1]; k++, cell_number++)
      {
        // node number of the cell
        int nb_nodes = index[ cell_number ] - index[ cell_number-1 ];
        if ( _geometricTypes[ j ] == MED_EN::MED_POLYHEDRA )
        {
          set<int> nodes( conn, conn + nb_nodes );
          set<int>::iterator n = nodes.begin();
          if ( *n == -1 ) ++n;
          for ( ; n != nodes.end(); ++n )
            reverse_connectivity[ *n ].push_back( cell_number );
          conn += nb_nodes;
        }
        else
        {
          for (int i = 0; i < nb_nodes; ++i, ++conn)
            reverse_connectivity[ *conn ].push_back( cell_number );
        }
      }
    }

    // Full reverse_nodal_connectivity and reverse_nodal_connectivity_index

    //calculate size of reverse_nodal_connectivity array
    int size_reverse_nodal_connectivity  = 0;
    for (int i = 1; i < _numberOfNodes+1; i++)
      size_reverse_nodal_connectivity += reverse_connectivity[i].size();

    int * reverse_nodal_connectivity_index = new int[_numberOfNodes+1];
    int * reverse_nodal_connectivity = new int[size_reverse_nodal_connectivity];

    reverse_nodal_connectivity_index[0] = 1;
    for (int i = 1; i < _numberOfNodes+1; i++)
    {
      int size = reverse_connectivity[i].size();
      reverse_nodal_connectivity_index[i] = reverse_nodal_connectivity_index[i-1] + size;
      for (int j = 0; j < size; j++)
        reverse_nodal_connectivity[reverse_nodal_connectivity_index[i-1]-1+j]= reverse_connectivity[i][j];
    }

    _reverseNodalConnectivity = new MEDSKYLINEARRAY(_numberOfNodes,size_reverse_nodal_connectivity,
                                                    reverse_nodal_connectivity_index,
                                                    reverse_nodal_connectivity,true);
  }
  END_OF_MED(LOC);
}

/*! If not yet done, calculate the Descending Connectivity */
//-------------------------------------------------//
void CONNECTIVITY::calculateDescendingConnectivity() const
//-------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::calculateDescendingConnectivity() : ";
  BEGIN_OF_MED(LOC);

  if (_descending==NULL)
  {
    if (_nodal==NULL)
    {
      MESSAGE_MED(LOC<<"No connectivity found !");
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity found !"));
    }
    // calcul _descending from _nodal
    // we need CONNECTIVITY for constituent

    if (_constituent != NULL && _constituent->_nodal != NULL)
    {
      const_cast<CONNECTIVITY*>(this)->calculatePartialDescendingConnectivity();
    }
    else
    {
      const_cast<CONNECTIVITY*>(this)->calculateFullDescendingConnectivity(_entity);
    }
  }
}

/*! If not yet done, calculate the full Descending Connectivity */
//-------------------------------------------------//
void CONNECTIVITY::calculateFullDescendingConnectivity(MED_EN::medEntityMesh Entity)
//-------------------------------------------------//
{
  const char * LOC = "CONNECTIVITY::calculateFullDescendingConnectivity() : ";
  BEGIN_OF_MED(LOC);
  if (_entity != Entity)
  {
    if (_constituent == NULL)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Entity not found !"));
    _constituent->calculateFullDescendingConnectivity(Entity);
  }
  else
  {
    if ( _descending==NULL || _isDescendingConnectivityPartial )
    {
      if (_nodal==NULL )
      {
        MESSAGE_MED(LOC<<"No connectivity found !");
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No connectivity found !"));
      }
      delete _constituent;
      _constituent=0;
      if (_entityDimension == 3)
        _constituent = new CONNECTIVITY(MED_EN::MED_FACE);
      else if (_entityDimension == 2)
        _constituent = new CONNECTIVITY(MED_EN::MED_EDGE);
      else
      {
        MESSAGE_MED(LOC<<"We are in 1D");
        return;
      }

      bool hasPoly = ( _numberOfTypes && _geometricTypes[_numberOfTypes-1] == ( _entityDimension > 2 ? MED_POLYHEDRA : MED_POLYGON ));
      int numberOfClassicTypes = _numberOfTypes - hasPoly;

      _constituent->_typeConnectivity = MED_EN::MED_NODAL;
      _constituent->_numberOfNodes = _numberOfNodes;
      // foreach cells, we built array of constituent
      int DescendingSize = 0;
      for(int i=0; i<numberOfClassicTypes; i++)
        DescendingSize+=(_count[i+1]-_count[i])*_type[i].getNumberOfConstituents(1);
      int * descend_connectivity = new int[DescendingSize];
      for (int i=0; i<DescendingSize; i++)
        descend_connectivity[i]=0;
      int * descend_connectivity_index = new int[_count[numberOfClassicTypes]];
      if(_count[numberOfClassicTypes]>0)
        descend_connectivity_index[0]=1;


      map<medGeometryElement,int> eltsCounter;
      medGeometryElement ConstituentsTypes[2] = { MED_EN::MED_NONE, MED_EN::MED_NONE };
      int NumberOfConstituentsForeachType [2] = { 0, 0 };
      map<medGeometryElement,int>::iterator status;
      for(int i=0; i<numberOfClassicTypes; i++)
      {
        // initialize descend_connectivity_index array :
        int NumberOfConstituents = _type[i].getNumberOfConstituents(1);
        for (int j=_count[i];j<_count[i+1];j++)
        {
          descend_connectivity_index[j]=descend_connectivity_index[j-1]+NumberOfConstituents;
          // compute number of constituent of all cell for each type
          for(int k=1;k<NumberOfConstituents+1;k++)
          {
            medGeometryElement MEDType = _type[i].getConstituentType(1,k);
            status=eltsCounter.find(MEDType);
            if(status!=eltsCounter.end())
              (*status).second++;
            else
              eltsCounter[MEDType]=1;
          }
        }
      }
      if ( hasPoly && _entityDimension == 2 ) // there are constituent segments
      {
        status=eltsCounter.insert(make_pair( MED_EN::MED_SEG2,0 )).first;
        //status->second++; // increment zero or that there was
      }
      if(eltsCounter.size()>2)
      {
        // free memory (issue 0020411: [CEA 342] Sigsegv on gibi writing of MESH coming from MED file without face computation)
        delete [] descend_connectivity;
        delete [] descend_connectivity_index;
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Descending connectivity does not support more than 2 types."));
      }
      status=eltsCounter.begin();
      if(!eltsCounter.empty())
      {
        ConstituentsTypes[0]=(*status).first; NumberOfConstituentsForeachType[0]=(*status).second;
        if(++status!=eltsCounter.end())
          ConstituentsTypes[1]=(*status).first, NumberOfConstituentsForeachType[1]=(*status).second;
      }
      int TotalNumberOfConstituents = NumberOfConstituentsForeachType[0]+NumberOfConstituentsForeachType[1];
      int TotalNumberOfNodes = NumberOfConstituentsForeachType[0]*(ConstituentsTypes[0]%100)+NumberOfConstituentsForeachType[1]*(ConstituentsTypes[1]%100);

      int * ConstituentNodalConnectivity      = new int[TotalNumberOfNodes];
      int * ConstituentNodalConnectivityIndex = new int[TotalNumberOfConstituents+1];
      ConstituentNodalConnectivityIndex[0]=1;

      _constituent->_entityDimension = _entityDimension-1;
      int nbClassicConstituentTypes = eltsCounter.size();
      int nbConstituentTypes = nbClassicConstituentTypes + hasPoly;
      _constituent->_numberOfTypes  = nbConstituentTypes;
      _constituent->_geometricTypes = new medGeometryElement[_constituent->_numberOfTypes];
      _constituent->_type           = new CELLMODEL[_constituent->_numberOfTypes];
      if(_constituent->_count) delete [] _constituent->_count;
      _constituent->_count          = new int[_constituent->_numberOfTypes+1];
      _constituent->_count[0]=1;
      med_int* tmp_NumberOfConstituentsForeachType = new med_int[nbClassicConstituentTypes+1];
      tmp_NumberOfConstituentsForeachType[0]=0; // to count constituent of each type
      for (int i=0; i<nbClassicConstituentTypes;i++)
      {
        _constituent->_geometricTypes[i]=ConstituentsTypes[i];
        _constituent->_type[i]=CELLMODEL(ConstituentsTypes[i]);
        _constituent->_count[i+1]=_constituent->_count[i]+NumberOfConstituentsForeachType[i];
        tmp_NumberOfConstituentsForeachType[i+1]=NumberOfConstituentsForeachType[i];
        for (int j=tmp_NumberOfConstituentsForeachType[i]; j<tmp_NumberOfConstituentsForeachType[i+1]+tmp_NumberOfConstituentsForeachType[i]; j++)
          ConstituentNodalConnectivityIndex[j+1]=ConstituentNodalConnectivityIndex[j]+(ConstituentsTypes[i]%100);
      }

      // we need reverse nodal connectivity
      if (! _reverseNodalConnectivity)
        calculateReverseNodalConnectivity();
      const int * ReverseNodalConnectivityIndex = _reverseNodalConnectivity->getIndex();
      const int * ReverseNodalConnectivityValue = _reverseNodalConnectivity->getValue();

      // array to keep reverse descending connectivity
      int * ReverseDescendingConnectivityValue = new int[TotalNumberOfConstituents*2];

      int TotalNumberOfSubCell = 0;
      for (int i=0; i<numberOfClassicTypes; i++)
      { // loop on all cell type
        CELLMODEL Type = _type[i];
        //      int NumberOfNodesPerCell = Type.getNumberOfNodes();
        int NumberOfConstituentPerCell = Type.getNumberOfConstituents(1);
        for (int j=_count[i]; j<_count[i+1]; j++) // we loop on all cell of this type
          for (int k=1; k<=NumberOfConstituentPerCell; k++)
          { // we loop on all sub cell of it
            if (descend_connectivity[descend_connectivity_index[j-1]+k-2]==0)
            { // it is a new sub cell !
              //      TotalNumberOfSubCell++;
              // Which type ?
              if (Type.getConstituentType(1,k)==_constituent->_geometricTypes[0])
              {
                tmp_NumberOfConstituentsForeachType[0]++;
                TotalNumberOfSubCell=tmp_NumberOfConstituentsForeachType[0];
              } else
              {
                tmp_NumberOfConstituentsForeachType[1]++;
                TotalNumberOfSubCell=tmp_NumberOfConstituentsForeachType[1];
              }
              //we have maximum two types

              descend_connectivity[descend_connectivity_index[j-1]+k-2]=TotalNumberOfSubCell;
              ReverseDescendingConnectivityValue[(TotalNumberOfSubCell-1)*2]=j;

              int NumberOfNodesPerConstituent = Type.getConstituentType(1,k)%100;
              int * NodesLists = new int[NumberOfNodesPerConstituent];
              for (int l=0; l<NumberOfNodesPerConstituent; l++)
              {
                NodesLists[l]=_nodal->getIJ(j,Type.getNodeConstituent(1,k,l+1));
                ConstituentNodalConnectivity[ConstituentNodalConnectivityIndex[TotalNumberOfSubCell-1]-1+l]=NodesLists[l];
              }
              // we use reverse_nodal_connectivity to find the other element which contain this sub cell

              // all elements which contains first node of sub cell :
              int ReverseNodalConnectivityIndex_0 = ReverseNodalConnectivityIndex[NodesLists[0]-1];
              int ReverseNodalConnectivityIndex_1 = getIndexOfEndClassicElementInReverseNodal(ReverseNodalConnectivityValue,ReverseNodalConnectivityIndex,NodesLists[0]-1);
              //ReverseNodalConnectivityIndex[NodesLists[0]];
              int NumberOfCellsInList = ReverseNodalConnectivityIndex_1-ReverseNodalConnectivityIndex_0;

              if (NumberOfCellsInList > 0)
              { // we could have no element !
                int * CellsList = new int[NumberOfCellsInList];
                for (int l=ReverseNodalConnectivityIndex_0; l<ReverseNodalConnectivityIndex_1; l++)
                  CellsList[l-ReverseNodalConnectivityIndex_0]=ReverseNodalConnectivityValue[l-1];

                // foreach node in sub cell, we search elements which are in common
                // at the end, we must have only one !

                for (int l=1; l<NumberOfNodesPerConstituent; l++)
                {
                  int NewNumberOfCellsInList = 0;
                  int * NewCellsList = new int[NumberOfCellsInList];
                  for (int l1=0; l1<NumberOfCellsInList; l1++)
                    for (int l2=ReverseNodalConnectivityIndex[NodesLists[l]-1]; l2<getIndexOfEndClassicElementInReverseNodal(ReverseNodalConnectivityValue,ReverseNodalConnectivityIndex,NodesLists[l]-1); l2++)
                      //for (int l2=ReverseNodalConnectivityIndex[NodesLists[l]-1]; l2<ReverseNodalConnectivityIndex[NodesLists[l]]; l2++)
                    {
                      if (CellsList[l1]<ReverseNodalConnectivityValue[l2-1])
                        // increasing order : CellsList[l1] are not in elements list of node l
                        break;
                      if ((CellsList[l1]==ReverseNodalConnectivityValue[l2-1])&(CellsList[l1]!=j)) {
                        // we have found one
                        NewCellsList[NewNumberOfCellsInList]=CellsList[l1];
                        NewNumberOfCellsInList++;
                        break;
                      }
                    }
                  NumberOfCellsInList = NewNumberOfCellsInList;

                  delete [] CellsList;
                  CellsList = NewCellsList;
                }

                if (NumberOfCellsInList > 0) { // We have found some elements !
                  int CellNumber = CellsList[0];
                  //delete [] CellsList;
                  if (NumberOfCellsInList>1) {
                    // free memory (issue 0020411: [CEA 342] Sigsegv on gibi writing of MESH coming from MED file without face computation)
                    delete [] CellsList;
                    delete [] NodesLists;
                    delete [] ReverseDescendingConnectivityValue;
                    delete [] tmp_NumberOfConstituentsForeachType;
                    delete [] descend_connectivity;
                    delete [] descend_connectivity_index;
                    delete [] ConstituentNodalConnectivity;
                    delete [] ConstituentNodalConnectivityIndex;
                    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"More than one other Cell ("<<NumberOfCellsInList<<") !"));
                  }

                  if (NumberOfCellsInList==1)
                  {
                    ReverseDescendingConnectivityValue[(TotalNumberOfSubCell-1)*2+1]=CellNumber;

                    // we search sub cell number in this cell to not calculate it another time
                    // which type ?
                    CELLMODEL Type2;
                    for (int l=0; l<numberOfClassicTypes; l++)
                      if (CellNumber < _count[l+1])
                      {
                        Type2=_type[l];
                        break;
                      }
                    //int sub_cell_count2 = Type2.get_entities_count(1);
                    //int nodes_cell_count2 = Type2.get_nodes_count();
                    bool find2 = false;
                    for (int l=1; l<=Type2.getNumberOfConstituents(1);l++) // on all sub cell
                    {
                      int counter = 0;
                      for (int m=1; m<=Type2.getConstituentType(1,l)%100; m++)
                        for (int n=1; n<=Type.getConstituentType(1,k)%100; n++)
                        {
                          if (_nodal->getIJ(CellNumber,Type2.getNodeConstituent(1,l,m)) == NodesLists[n-1])
                            counter++;
                        }
                      if (counter==Type.getConstituentType(1,k)%100)
                      {
                        descend_connectivity[descend_connectivity_index[CellNumber-1]+l-2]=-1*TotalNumberOfSubCell; // because, see it in other side !
                        find2 = true;
                      }
                      if (find2)
                        break;
                    }
                    if (!find2)
                      MESSAGE_MED(LOC<<"ERROR ERROR ERROR ERROR ERROR : we find any subcell !!!"); // exception ?
                  }
                } else {
                  ReverseDescendingConnectivityValue[(TotalNumberOfSubCell-1)*2+1]=0;
                }
                delete [] CellsList;
              }

              delete [] NodesLists;
            }
          }
      }
      // we adjust _constituent
      int NumberOfConstituent=0;
      int SizeOfConstituentNodal=0;
      for (int i=0;i<nbClassicConstituentTypes; i++)
      {
        NumberOfConstituent += tmp_NumberOfConstituentsForeachType[i]-_constituent->_count[i]+1;
        SizeOfConstituentNodal += (tmp_NumberOfConstituentsForeachType[i]-_constituent->_count[i]+1)*_constituent->_type[i].getNumberOfNodes();
      }
      // we built new _nodal attribute in _constituent
      //MEDSKYLINEARRAY *ConstituentNodal = new MEDSKYLINEARRAY(NumberOfConstituent,SizeOfConstituentNodal);
      //const int *ConstituentNodalValue = ConstituentNodal->getValue();
      //const int *ConstituentNodalIndex = ConstituentNodal->getIndex();
      int *ConstituentNodalValue = new int[SizeOfConstituentNodal];
      int *ConstituentNodalIndex = new int[NumberOfConstituent+1];
      ConstituentNodalIndex[0]=1;
      // we build _reverseDescendingConnectivity
      //_reverseDescendingConnectivity = new MEDSKYLINEARRAY(NumberOfConstituent,2*NumberOfConstituent);
      //const int *reverseDescendingConnectivityValue = _reverseDescendingConnectivity->getValue();
      //const int *reverseDescendingConnectivityIndex = _reverseDescendingConnectivity->getIndex();
      int *reverseDescendingConnectivityValue = new int[2*NumberOfConstituent];
      int *reverseDescendingConnectivityIndex = new int[NumberOfConstituent+1];
      reverseDescendingConnectivityIndex[0]=1;

      // first constituent type
      for(int j=0; j<tmp_NumberOfConstituentsForeachType[0]; j++)
      {
        ConstituentNodalIndex[j+1]=ConstituentNodalConnectivityIndex[j+1];
        for(int k=ConstituentNodalIndex[j]; k<ConstituentNodalIndex[j+1]; k++)
          ConstituentNodalValue[k-1]=ConstituentNodalConnectivity[k-1];

        reverseDescendingConnectivityIndex[j+1]=reverseDescendingConnectivityIndex[j]+2;
        for(int k=reverseDescendingConnectivityIndex[j]; k<reverseDescendingConnectivityIndex[j+1]; k++)
          reverseDescendingConnectivityValue[k-1]=ReverseDescendingConnectivityValue[k-1];
      }
      // second type if any
      if (nbClassicConstituentTypes==2)
      {
        int offset = _constituent->_count[1]-tmp_NumberOfConstituentsForeachType[0]-1;
        int offset1=offset*_constituent->_type[0].getNumberOfNodes();
        int offset2=offset*2;
        int NumberOfNodesPerConstituent = _constituent->_type[1].getNumberOfNodes();
        for(int j=tmp_NumberOfConstituentsForeachType[0]; j<NumberOfConstituent; j++)
        {
          ConstituentNodalIndex[j+1]=ConstituentNodalIndex[j]+NumberOfNodesPerConstituent;
          for(int k=ConstituentNodalIndex[j]; k<ConstituentNodalIndex[j+1]; k++)
            ConstituentNodalValue[k-1]=ConstituentNodalConnectivity[offset1+k-1];

          reverseDescendingConnectivityIndex[j+1]=reverseDescendingConnectivityIndex[j]+2;
          for(int k=reverseDescendingConnectivityIndex[j]; k<reverseDescendingConnectivityIndex[j+1]; k++)
            reverseDescendingConnectivityValue[k-1]=ReverseDescendingConnectivityValue[offset2+k-1];
        }
        _constituent->_count[2]=NumberOfConstituent+1;
        // we correct _descending to adjust face number
        for(int j=0;j<DescendingSize;j++)
          if (abs(descend_connectivity[j])>tmp_NumberOfConstituentsForeachType[0]) {
            if ( descend_connectivity[j] > 0 )
              descend_connectivity[j]-=offset;
            else
              descend_connectivity[j]+=offset;
          }
      }

      delete [] ConstituentNodalConnectivityIndex;
      delete [] ConstituentNodalConnectivity;
      delete [] ReverseDescendingConnectivityValue;
      if (_constituent->_numberOfTypes > 0)
        _constituent->_count[1]=tmp_NumberOfConstituentsForeachType[0]+1;
      delete [] tmp_NumberOfConstituentsForeachType;

      ////
      vector<int> Descending( descend_connectivity, descend_connectivity + DescendingSize );
      vector<int> DescendingIndex( descend_connectivity_index, descend_connectivity_index + _count[numberOfClassicTypes] );
      vector<int> Reversedescendingconnectivityvalue(reverseDescendingConnectivityValue,reverseDescendingConnectivityValue + 2*NumberOfConstituent);
      vector<int> Reversedescendingconnectivityindex(reverseDescendingConnectivityIndex,reverseDescendingConnectivityIndex + NumberOfConstituent);
      delete [] descend_connectivity;
      delete [] descend_connectivity_index;
      delete [] reverseDescendingConnectivityValue;
      delete [] reverseDescendingConnectivityIndex;


      // polygons (2D mesh)

      vector<int> Constituentnodalvalue(ConstituentNodalValue,ConstituentNodalValue + SizeOfConstituentNodal);
      vector<int> Constituentnodalindex(ConstituentNodalIndex,ConstituentNodalIndex + NumberOfConstituent+1);
      delete [] ConstituentNodalValue;
      delete [] ConstituentNodalIndex;
      int NumberOfNewSeg = 0;

      int i = _count[_numberOfTypes - ( hasPoly && _entityDimension == 2 )];
      int NumberOfPolygons = _count[_numberOfTypes] - i;
      for (; i < _count[_numberOfTypes]; i++) // for each polygon
      {
        const int * vector_begin = _nodal->getValue() + _nodal->getIndex()[i-1] - 1;
        int vector_size = _nodal->getIndex()[i]-_nodal->getIndex()[i-1];
        vector<int> myPolygon(vector_begin, vector_begin+vector_size);
        myPolygon.push_back(myPolygon[0]);
        DescendingIndex.push_back( DescendingIndex.back() + vector_size);
        for (int j=0; j<(int)myPolygon.size()-1; j++) // for each segment of polygon
        {
          MEDMODULUSARRAY segment_poly(2,&myPolygon[j]);
          int ret_compare = 0;

          // we search it in existing segments

          for (int k=0; k<(int)Constituentnodalindex.size()-1; k++)
          {
            MEDMODULUSARRAY segment(2,&Constituentnodalvalue[0] + Constituentnodalindex[k]-1);
            ret_compare = segment_poly.compare(segment);
            if (ret_compare) // segment_poly already exists
            {
              Descending.push_back(ret_compare*(k+1)); // we had it to the connectivity
              insert_vector(Reversedescendingconnectivityvalue, 2*k+1, i); // add polygon i to reverse descending connectivity for segment_poly (in 2nd place)
              break;
            }
          }

          // segment_poly must be created

          if (!ret_compare)
          {
            NumberOfNewSeg++;
            Descending.push_back(NumberOfConstituent+NumberOfNewSeg); // we had it to the connectivity
            Constituentnodalvalue.push_back(segment_poly[0]);
            Constituentnodalvalue.push_back(segment_poly[1]);
            Constituentnodalindex.push_back(Constituentnodalindex[Constituentnodalindex.size()-1] + 2); // we have only segments
            insert_vector(Reversedescendingconnectivityvalue, 2*(NumberOfConstituent+NumberOfNewSeg-1), i ); // add polygon i to reverse descending connectivity for segment_poly (in 1st place)
            insert_vector(Reversedescendingconnectivityindex, NumberOfConstituent+NumberOfNewSeg-1, 2*(NumberOfConstituent+NumberOfNewSeg-1)+1); // idem with index
          }
        }
      }

      if ( NumberOfPolygons > 0 )
      {
        NumberOfConstituent += NumberOfNewSeg;
        SizeOfConstituentNodal += 2*NumberOfNewSeg;
      }

      // polyhedron (3D mesh)

      vector<int> Constituentpolygonsnodalvalue;
      vector<int> Constituentpolygonsnodalindex(1,1);
      int NumberOfNewFaces = 0; // by convention new faces are polygons
      //offset to switch between all types and classical types.
      //int offsetCell = getNumberOf(MED_EN::MED_CELL, MED_ALL_ELEMENTS);
      int *tabRes = new int[1000]; //temporay array for intersection calculation

      i = _count[_numberOfTypes - ( hasPoly && _entityDimension == 3 )];
      int NumberOfPolyhedron = _count[_numberOfTypes] - i;
      for (; i<_count[_numberOfTypes]; i++) // for each polyhedron
      {
        const int * nodes    = _nodal->getValue() + _nodal->getIndex()[i-1] - 1;
        const int * nodesEnd = _nodal->getValue() + _nodal->getIndex()[i  ] - 1;
        int nbConstituentFaces = 0;
        while ( nodes < nodesEnd ) // loop on faces of polyhedron 
        {
          // look for end of face connectivity
          const int * faceNodesEnd = nodes;
          while ( ++faceNodesEnd < nodesEnd && *faceNodesEnd != -1 );
          int myFaceNumberOfNodes = faceNodesEnd - nodes;
          MEDMODULUSARRAY face_poly(myFaceNumberOfNodes,nodes);
          // move pointer to next face while continue working with face_poly
          if (( nodes = faceNodesEnd ) < nodesEnd )
            nodes++; // skip face separator

          nbConstituentFaces++;

          int ret_compare = 0;

          // we search it in existing faces

          // we search first in TRIA3 and QUAD4
          if (myFaceNumberOfNodes == 3 || myFaceNumberOfNodes == 4)
          {
            int Begin = -1; // first TRIA3 or QUAD4
            int Number = 0; // number of TRIA3 or QUAD4
            for (int k=0; k<(int)Constituentnodalindex.size()-1; k++)
              if (Constituentnodalindex[k+1]-Constituentnodalindex[k] == myFaceNumberOfNodes)
              {
                if (Begin == -1)
                  Begin = k;
                Number++;
              }

            for (int k=0; k<Number; k++)
            {
              MEDMODULUSARRAY face(myFaceNumberOfNodes,&Constituentnodalvalue[0] + Constituentnodalindex[Begin+k]-1);
              ret_compare = face_poly.compare(face);
              if (ret_compare)
              {
                Descending.push_back(ret_compare*(Begin+k+1)); // we had it to the connectivity
                insert_vector(Reversedescendingconnectivityvalue, 2*(Begin+k)+1, i); // add polyhedra i to reverse descending connectivity for face_poly (in 2nd place)
                break;
              }
            }
          }

          // we search last in POLYGONS
          if (!ret_compare)
          {
            int lgth;
            const int *facePolyTab=face_poly.getArray(lgth);
            int nbOfCandidatesCell = ReverseNodalConnectivityIndex[facePolyTab[0]] -
              ReverseNodalConnectivityIndex[facePolyTab[0]-1];
            const int *candidatesCell = ReverseNodalConnectivityValue +
              ReverseNodalConnectivityIndex[facePolyTab[0]-1] - 1;
            memcpy(tabRes,candidatesCell,nbOfCandidatesCell*sizeof(int));
            int lgth2=nbOfCandidatesCell;
            for (int k=1;k<lgth && lgth2!=0;k++)
            {
              nbOfCandidatesCell = ReverseNodalConnectivityIndex[facePolyTab[k]] -
                ReverseNodalConnectivityIndex[facePolyTab[k]-1];
              candidatesCell = ReverseNodalConnectivityValue +
                ReverseNodalConnectivityIndex[facePolyTab[k]-1] - 1;
              mergeOrderedTabs(tabRes,lgth2,candidatesCell,nbOfCandidatesCell,tabRes,lgth2);
            }
            if (lgth2<=1)
              ret_compare=0;//here normally tabRes[0]==i
            else //> 2 should never happend : A face is shared by more than 2 polyhedrons...
            {
              if (tabRes[0] == i) //as tabRes is ordered by construction tabRes[1] > tabRes[0] so the current
                // face is shared with an another cell whose id > current id. So let's create
                ret_compare=0;
              else
              {//tabRes[0]<Constituentpolygonsnodalindex.size()-1 that is to say the current face has been built previously.
                int nbOfFacesConstitutingAlreadyBuiltPolyh;
                int* nbOfNodesPerFaces;
                int** nodesOfFacesOfAlreadyBuiltPolyh = getNodesPerFaceOfPolyhedron( tabRes[0], nbOfFacesConstitutingAlreadyBuiltPolyh, nbOfNodesPerFaces);
                for (int k1=0; k1<nbOfFacesConstitutingAlreadyBuiltPolyh && (ret_compare==0); k1++)
                {
                  int nbOfNodesForCurrentFace = nbOfNodesPerFaces[k1];
                  MEDMODULUSARRAY face(nbOfNodesForCurrentFace,nodesOfFacesOfAlreadyBuiltPolyh[k1]);
                  ret_compare = face_poly.compare(face);
                  if (ret_compare)
                  {
                    int curFaceId=Descending[ DescendingIndex[ tabRes[0]-1 ]-1 + k1 ];
                    Descending.push_back(ret_compare*curFaceId); // we had it to the connectivity
                    insert_vector(Reversedescendingconnectivityvalue, 2*(curFaceId-1)+1, i);
                  }
                }
                delete [] nbOfNodesPerFaces;
                delete [] nodesOfFacesOfAlreadyBuiltPolyh;
              }
            }
          }

          // if not found, face_poly must be created

          if (!ret_compare)
          {
            NumberOfNewFaces++;
            Descending.push_back(NumberOfConstituent+NumberOfNewFaces); // we had it to the connectivity
            for (int k=0; k<myFaceNumberOfNodes; k++)
              Constituentpolygonsnodalvalue.push_back(face_poly[k]);
            Constituentpolygonsnodalindex.push_back(Constituentpolygonsnodalindex[Constituentpolygonsnodalindex.size()-1]+myFaceNumberOfNodes);
            insert_vector(Reversedescendingconnectivityvalue, 2*(NumberOfConstituent+NumberOfNewFaces-1), i); // add polyhedra i to reverse descending connectivity for face_poly (in 1st place)
            insert_vector(Reversedescendingconnectivityindex, NumberOfConstituent+NumberOfNewFaces-1, 2*(NumberOfConstituent+NumberOfNewFaces-1)+1); // idem with index
          }
        }  // loop on faces of polyhedron

        DescendingIndex.push_back( DescendingIndex.back() + nbConstituentFaces );
      }
      delete [] tabRes;

      if (NumberOfPolyhedron > 0)
      {
        NumberOfConstituent += NumberOfNewFaces;
        Constituentnodalvalue.insert( Constituentnodalvalue.end(),
                                      Constituentpolygonsnodalvalue.begin(),
                                      Constituentpolygonsnodalvalue.end());
        Constituentpolygonsnodalvalue.clear();
        Constituentnodalindex.reserve( Constituentnodalindex.size() + NumberOfNewFaces );
        int indexShift = Constituentnodalindex.back() - Constituentpolygonsnodalindex.front();
        for ( unsigned i = 1; i < Constituentpolygonsnodalindex.size(); ++i )
          Constituentnodalindex.push_back( Constituentpolygonsnodalindex[i] + indexShift );
        Constituentpolygonsnodalindex.clear();
        _constituent->_geometricTypes[ _constituent->_numberOfTypes-1 ] = MED_POLYGON;
      }

      _constituent->_count[ _constituent->_numberOfTypes ] = NumberOfConstituent+1;
      _constituent->_nodal = new MEDSKYLINEARRAY(NumberOfConstituent,
                                                 Constituentnodalvalue.size(),
                                                 &Constituentnodalindex[0],
                                                 &Constituentnodalvalue[0]);

      if(_descending)
        delete _descending;
      _descending = new MEDSKYLINEARRAY(_count[_numberOfTypes]-1,
                                        Descending.size(),
                                        &DescendingIndex[0],
                                        &Descending[0]);

      Reversedescendingconnectivityindex.push_back(Reversedescendingconnectivityindex[Reversedescendingconnectivityindex.size()-1]+2); // we complete the index
      Reversedescendingconnectivityvalue.push_back(0);
      _reverseDescendingConnectivity = new MEDSKYLINEARRAY(NumberOfConstituent+1,
                                                           2*NumberOfConstituent,
                                                           &Reversedescendingconnectivityindex[0],
                                                           &Reversedescendingconnectivityvalue[0]);
      _isDescendingConnectivityPartial = false;
    }
    END_OF_MED(LOC);
  }
}

void CONNECTIVITY::addToDescendingConnectivity(const set<int>& nodes,
                                               multimap<int,int>& descending,
                                               int iglobal_cell,
                                               const CONNECTIVITY_HashMap & face_map ) const
{
  int dimension = getEntityDimension();
  vector<int> signature (dimension);
  set<int>::const_iterator iter=nodes.begin();
  for (int i=0; i< dimension;i++)
  {
    signature[i]=*iter;
    iter++;
  }


  CONNECTIVITY_HashMap::const_iterator itermap=face_map.find(signature);
  CONNECTIVITY_HashMap::const_iterator iterend=face_map.end();


  if (itermap!=iterend)
    descending.insert(make_pair(iglobal_cell,itermap->second));
}

/*! This method calculates the descending connectivity without creating missing elements. It only maps the constituent elements that are described in the nodal representation.
  For instance, let us consider the following mesh with no MED_EN::MED_EDGE elements on the inner edges.

  +----1----+----2----+
  |         |         |
  8    1    |    2    3
  |         |         |
  +---------+---------+
  |         |         |
  7    3    |    4    4
  |         |         |
  +----6----+----5----+


  calculatePartialDescendingConnectivity()
  will return
  1:1,8
  2:2,3
  3:6,7
  4:4,5

  whereas calculateDescendingConnectivity()
  will create new edges, renumbering existing ones and return
  1:1,2,3,4
  2:5,6,7,2
  3:3,8,9,10
  4:7,11,12,8

  +----1----+----5----+
  |         |         |
  4    1    2    2    6
  |         |         |
  +----3----+----7----+
  |         |         |
  10    3   8    4    11
  |         |         |
  +----9----+----12---+
*/

void CONNECTIVITY::calculatePartialDescendingConnectivity() const
{
  ////////////////////////////////////////////////////////////////////////////
  // First stage : creating hash_map referencing faces with the triplet
  // of the lowest order nodes as a key and the global face number as a value

  CONNECTIVITY_HashMap face_map;
  int iglobal_face=1;

  int dimension = getEntityDimension();
  {
    const int* conn =_constituent->getValue     (MED_EN::MED_NODAL, MED_ALL_ELEMENTS);
    const int* index=_constituent->getValueIndex(MED_EN::MED_NODAL);
    int nbfaces=_constituent->getNumberOf(_constituent->getEntity(),MED_ALL_ELEMENTS);
    for ( ; iglobal_face <= nbfaces; iglobal_face++ )
    {
      const int* face_conn = conn + index[iglobal_face-1] - 1;
      const int nbnodes = index[iglobal_face] - index[iglobal_face-1];
      set<int> nodes( face_conn, face_conn + nbnodes );
      vector<int> signature (dimension);
      set<int>::iterator iter=nodes.begin();
      for (int i=0; i< dimension;i++)
      {
        signature[i]=*iter;
        iter++;
      }
      face_map.insert(make_pair(signature,iglobal_face));
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  //Second stage : going through all the faces of the cells and
  // connecting them to the hash_map created in the first stage

  multimap<int,int> descending; //map storing the descending connectivity

  int nbcell_types = getNumberOfTypes(_entity);
  const medGeometryElement* cell_types = getGeometricTypes(_entity);
  int iglobal_cell = 1;
  for (int itype=0; itype<nbcell_types; itype++)
  {
    medGeometryElement cell_type = cell_types[itype];
    int nbcells = getNumberOf(_entity,cell_type);

    const int* index = _nodal->getIndex();
    const int* conn  = _nodal->getValue();
    switch ( cell_type )
    {
    case MED_POLYGON:
      {
        for (int icell=0;icell<nbcells;icell++)
        {
          int nbnodes=index[icell+1]-index[icell];
          int nbfaces=nbnodes;
          for (int iface=0; iface<nbfaces;iface++)
          {
            set<int> nodes;
            if (iface+1!=nbfaces)
            {
              nodes.insert(conn[index[icell]-1+iface]);
              nodes.insert(conn[index[icell]-1+iface+1]);
            }
            else
            {
              nodes.insert(conn[index[icell]-1+iface]);
              nodes.insert(conn[index[icell]-1]);
            }
            addToDescendingConnectivity(nodes,descending,iglobal_cell,face_map);
          }
          iglobal_cell++;
        }
      }
      break;

    case MED_POLYHEDRA:
      {
        for (int icell = 0; icell < nbcells; icell++)
        {
          const int* face_conn     = conn + index[icell  ] - 1;
          const int* face_conn_end = conn + index[icell+1] - 1;
          while (face_conn < face_conn_end)
          {
            set<int> nodes; // faces are divided by -1
            for ( ; face_conn < face_conn_end && *face_conn != -1; face_conn++ )
              nodes.insert( *face_conn );
            addToDescendingConnectivity(nodes,descending,iglobal_cell,face_map);
          }
          iglobal_cell++;
        }
      }
      break;

    default: // classic cells
      {
        CELLMODEL cellmodel=CELLMODEL_Map::retrieveCellModel(cell_type);
        const int* index=_nodal->getIndex();
        const int* conn=_nodal->getValue();
        for (int icell = 0; icell < nbcells; icell++, iglobal_cell++)
        {
          int nbfaces=cellmodel.getNumberOfConstituents(1);
          for (int iface=0; iface<nbfaces;iface++)
          {
            set<int> nodes;
            const int* local_index=cellmodel.getNodesConstituent(1,iface+1);
            medGeometryElement face_type = cellmodel.getConstituentType(1,iface+1);
            int nbnodes=face_type%100;
            for (int inode=0;inode<nbnodes;inode++)
              nodes.insert(conn[index[iglobal_cell-1]-1+local_index[inode]-1]);
            addToDescendingConnectivity(nodes,descending,iglobal_cell,face_map);
          }
        }
      }
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  // Third stage : reorganizing the descending data to store it in a medskylinearray

  vector<int> index;
  vector<int> value;
  index.push_back(1);

  //the number of cells is given by the number
  //obtained by browsing all the cell types
  int nb_cells = iglobal_cell-1;

  //for (int icell = 0; icell < nb_cells; icell++)
  for (int icell = 1; icell <= nb_cells; icell++)
  {
    multimap<int,int>::iterator beginning_of_range = descending.lower_bound(icell);
    multimap<int,int>::iterator end_of_range = descending.upper_bound(icell);
    int nb=0;
    for (multimap<int,int>::iterator iter2 = beginning_of_range; iter2 != end_of_range; iter2++)
    {
      value.push_back(iter2->second);
      nb++;
    }
    index.push_back(index.back()+nb);
  }
  if(_descending)
    delete _descending;
  _descending = new MEDSKYLINEARRAY(index.size()-1, value.size(), &index[0], &value[0]);
  _isDescendingConnectivityPartial = true;
}


/*! Not implemented yet */
//--------------------------------------------------------------------//
void CONNECTIVITY::calculateNeighbourhood(CONNECTIVITY &myConnectivity)
//--------------------------------------------------------------------//
{
  const char* LOC = "CONNECTIVITY::calculateNeighbourhood(CONNECTIVITY &myConnectivity) : ";
  BEGIN_OF_MED(LOC);

  MESSAGE_MED(PREFIX_MED<<"method not yet implemented " << myConnectivity._entity);
  // Mesh dimension !

  END_OF_MED(LOC);
  return;
}


/*!
  Returns the geometry of an element given by its entity type & its global number.

  Example : medGeometryElement myType = myConnectivity.getElementType(MED_EN::MED_CELL,35);
*/
//--------------------------------------------------------------------//
medGeometryElement CONNECTIVITY::getElementType(medEntityMesh Entity,int globalNumber) const
//--------------------------------------------------------------------//
{
  const char * LOC = "medGeometryElement CONNECTIVITY::getElementType(medEntityMesh Entity, int globalNumber) const : ";
  BEGIN_OF_MED(LOC);
  int globalNumberMin = 1;
  int globalNumberMax ;

  if (_entity==Entity) globalNumberMax = _count[_numberOfTypes];
  else if (_constituent!=NULL) globalNumberMax = _constituent->_count[_constituent->_numberOfTypes];
  else
    throw MEDEXCEPTION(LOCALIZED("getElementType : Entity not defined !"));

  // The globalNumber must verify  : 1 <=  globalNumber < _count[_numberOfTypes] (== totalNumberOfElement+1)

  if ( (globalNumber < globalNumberMin) || (globalNumber >  globalNumberMax-1 )  )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "globalNumber must be between >= |"
                                 << globalNumberMin <<"| and <= |" << globalNumberMax-1 ));

  if (_entity==Entity) {
    for(int i=1; i<=_numberOfTypes;i++)
      if (globalNumber<_count[i])
        return _geometricTypes[i-1];
  }
  else if (_constituent!=NULL)
    return _constituent->getElementType(Entity,globalNumber);
  else
    throw MEDEXCEPTION(LOCALIZED("getElementType : Entity not defined !"));
  throw MEDEXCEPTION(LOCALIZED("getElementType : Wrong Number !"));

  END_OF_MED(LOC);
}

ostream & MEDMEM::operator<<(ostream &os, CONNECTIVITY &co)
{
  os << endl << "------------- Entity = ";
  switch (co._entity)
  {
  case MED_EN::MED_CELL:
    os << "MED_EN::MED_CELL";
    break;
  case MED_EN::MED_FACE:
    os << "MED_EN::MED_FACE";
    break;
  case MED_EN::MED_EDGE:
    os << "MED_EN::MED_EDGE";
    break;
  case MED_EN::MED_NODE:
    os << "MED_EN::MED_NODE";
    break;
  case MED_EN::MED_ALL_ENTITIES:
    os << "MED_EN::MED_ALL_ENTITIES";
    break;
  default:
    os << "Unknown";
    break;
  }
  os  << " -------------\n\nMedConnectivity : ";
  switch (co._typeConnectivity)
  {
  case MED_EN::MED_NODAL:
    os << "MED_EN::MED_NODAL\n";
    break;
  case MED_EN::MED_DESCENDING:
    os << "MED_DESCENDING\n";
    break;
  default:
    break;
  }
  os << "Entity dimension : " << co._entityDimension << endl;
  os << "Number of nodes : " << co._numberOfNodes << endl;
  os << "Number of types : " << co._numberOfTypes << endl;
  for (int i=0; i!=co._numberOfTypes ; ++i)
    os << "  -> Type " << co._geometricTypes[i] << " (" << co._type[i].getName() << ") : "
       << co._count[i+1]-co._count[i] << " elements" << endl;

  if (co._typeConnectivity == MED_EN::MED_NODAL )
  {
    for (int i=0; i<co._numberOfTypes; i++)
    {
      os << endl << co._type[i].getName() << " : " << endl;
      int numberofelements = co._count[i+1]-co._count[i];
      const int * connectivity = co.getConnectivity(co._typeConnectivity, co._entity, MED_EN::MED_ALL_ELEMENTS);
      const int * index = co.getConnectivityIndex(co._typeConnectivity, co._entity) + co._count[i]-1;
      for (int j=0;j<numberofelements;j++)
      {
        const int * cellconn = connectivity+index[j]-1;
        const int * cellconnend = connectivity+index[j+1]-1;
        os << setw(6) << j+1 << " : ";
        while ( cellconn < cellconnend )
          os << *cellconn++ <<" ";
        os << endl;
      }
    }
  }
  else if (co._typeConnectivity == MED_EN::MED_DESCENDING)
  {
    int numberofelements = co.getNumberOf( co._entity , MED_ALL_ELEMENTS);
    if (numberofelements > 0)
    {
      const int *connectivity =  co.getConnectivity( co._typeConnectivity, co._entity, MED_ALL_ELEMENTS);
      const int *connectivity_index =  co.getConnectivityIndex( co._typeConnectivity, co._entity );

      for ( int j=0; j!=numberofelements; j++ )
      {
        os << "Element " << j+1 << " : ";
        for ( int k=connectivity_index[j]; k!=connectivity_index[j+1]; k++ )
          os << connectivity[k-1] << " ";
        os << endl;
      }
    }
  }

  if (co._constituent)
    os << endl << *co._constituent << endl;

  return os;
}

/*!
  method that adds to vector 'nodes' all the nodes of polyhedron with id 'polyhedronId'.
  WARNING the returned pointer should be deallocated. Returned nodes and polyhedronId are in form [1,...]
*/
int *CONNECTIVITY::getNodesOfPolyhedron(int polyhedronId, int& lgthOfTab) const
{
  int offsetWithClassicType=_count[ _numberOfTypes-1 ]-1; // hope that polyhedrons is the last type
  if (polyhedronId<=offsetWithClassicType || polyhedronId> getNumberOf (MED_EN::MED_CELL, MED_ALL_ELEMENTS))
    throw MEDEXCEPTION("Polyhedron ID does not match a polyhedron in the element range");

  const int *nodes=getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL, MED_ALL_ELEMENTS);
  const int *index=getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
  const int *cellnodes    = nodes + index[polyhedronId-1] - 1;
  const int *cellnodesend = nodes + index[polyhedronId  ] - 1;
  set<int> retInSet( cellnodes, cellnodesend );
  if ( *retInSet.begin() == -1 )
    retInSet.erase( retInSet.begin() );
  lgthOfTab=retInSet.size();
  int *ret=new int[lgthOfTab];
  set<int>::iterator iter=retInSet.begin();
  for(int i=0;iter!=retInSet.end();iter++)
    ret[i++]=*iter;
  return ret;
}

/*!
  Idem as MESH::getNodesOfPolyhedron except that returned nodes are sorted by face. 'nbOfNodesPerFaces' is an array of size 'nbOfFaces'.
  Returned int** has a size of 'nbOfNodesPerFaces' too, and for each element j in int** the size is  nbOfNodesPerFaces[j].
  Warning both returned 'nbOfNodesPerFaces' and returned value should be deallocated. Returned nodes and 'polyhedronId' are in form [1,...]
*/
int **CONNECTIVITY::getNodesPerFaceOfPolyhedron(int polyhedronId, int& nbOfFaces, int* & nbOfNodesPerFaces) const
{
  int offsetWithClassicType=_count[ _numberOfTypes-1 ]-1; // hope that polyhedrons is the last type
  if (polyhedronId<=offsetWithClassicType || polyhedronId> getNumberOf(MED_EN::MED_CELL, MED_ALL_ELEMENTS))
    throw MEDEXCEPTION("Polyhedron ID does not match a polyhedron in the element range");
  const int *nodes=getConnectivity(MED_EN::MED_NODAL, MED_EN::MED_CELL, MED_ALL_ELEMENTS);
  const int *index=getConnectivityIndex(MED_EN::MED_NODAL, MED_EN::MED_CELL);

  const int *cellnodes    = nodes + index[polyhedronId-1] - 1;
  const int *cellnodesend = nodes + index[polyhedronId  ] - 1;
  nbOfFaces=0;
  nbOfNodesPerFaces=new int[cellnodesend-cellnodes]; // more than enough
  int **ret=new int* [cellnodesend-cellnodes];
  while ( cellnodes < cellnodesend )
  {
    ret[nbOfFaces] = (int*)cellnodes;
    while ( cellnodes < cellnodesend && *cellnodes != -1 ) ++cellnodes;
    nbOfNodesPerFaces[nbOfFaces] = cellnodes - ret[nbOfFaces];
    cellnodes++;
    nbOfFaces++;
  }
  return ret;
}

/*
  Method used in CalculateDescendingConnectivity. So it's typically a private method.
  The aim of this method is to hide to CalculateDescendingConnectivity algorithm the fact that in reverse connectivity polygons and polyhedrons
  are not in separate data structure, contrary to not reverse connectivities.
*/
int CONNECTIVITY::getIndexOfEndClassicElementInReverseNodal(const int *reverseNodalValue, const int *reverseNodalIndex, int rk)  const
{
  const int lastTypeIsPoly = (_geometricTypes[_numberOfTypes-1]==MED_EN::MED_POLYGON||
                              _geometricTypes[_numberOfTypes-1]==MED_EN::MED_POLYHEDRA);
  if ( !lastTypeIsPoly )
    return reverseNodalIndex[rk+1];
  int nbOfLastElt=_count[_numberOfTypes-1]-1;
  int ret=reverseNodalIndex[rk];
  for(int i=reverseNodalIndex[rk];i<reverseNodalIndex[rk+1];i++)
  {
    if(reverseNodalValue[i-1]<=nbOfLastElt)
      ret++;
  }
  return ret;
}

/*
  Method that inverts the face with faceId 'faceId' in the data structure.
  This method has to be applied on a instance of MEDMEM::CONNECTIVITY with _entityDimension==3.
  WARNING calculateDescendingConnectivity must have been called before.
*/
void CONNECTIVITY::invertConnectivityForAFace(int faceId, const int *nodalConnForFace)
{
  // we have always 2 neighbourings
  int cell1 = _reverseDescendingConnectivity->getIJ(faceId,1);
  int cell2 = _reverseDescendingConnectivity->getIJ(faceId,2);

  if (cell2 != 0) { // we are not on border, update compulsory. If we aren't on border no update necessary so WARNING because user specified a bad oriented face
    // Updating _reverseDescendingConnectivity
    _reverseDescendingConnectivity->setIJ(faceId,1,cell2);
    _reverseDescendingConnectivity->setIJ(faceId,2,cell1);
    // Updating _constituent->_nodal because of reversity
    const int *descendingNodalIndex=_constituent->_nodal->getIndex();
    MEDSKYLINEARRAY *currentNodal=_constituent->_nodal;
    int faceIdRelative=faceId;
    for(int iarray=1;iarray<=(descendingNodalIndex[faceIdRelative]-descendingNodalIndex[faceIdRelative-1]);iarray++)
      currentNodal->setIJ(faceIdRelative,iarray,nodalConnForFace[iarray-1]);

    // Updating _descending for cell1 and cell2
    const int NB_OF_CELLS_SHARING_A_FACE=2;
    int cellsToUpdate[NB_OF_CELLS_SHARING_A_FACE] = { cell1, cell2 };
    for(int curCell=0;curCell<NB_OF_CELLS_SHARING_A_FACE;curCell++)
    {
      int cell=cellsToUpdate[curCell];
      const int *newDescendingIndex=_descending->getIndex();
      MEDSKYLINEARRAY *currentDescending=_descending;
      for(int iface=newDescendingIndex[cell-1];iface<newDescendingIndex[cell];iface++)
      {
        int curValue=currentDescending->getIndexValue(iface);
        if (abs(curValue)==faceId)
          currentDescending->setIndexValue(iface,-curValue);
      }
    }
  }
}

/*
  Method with 2 output : the connectivity required and its length 'lgth'.
  This method gives the connectivity independently it is a polygons/polyhedrons or normal element.
*/
const int * CONNECTIVITY::getConnectivityOfAnElement(MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh Entity, int Number, int &lgth) const
{
  if(Entity==MED_EN::MED_NODE)
    throw  MEDEXCEPTION("No connectivity attached to a node entity");
  if(Entity==_entity)
  {
    if ( Number > getNumberOf( Entity, MED_ALL_ELEMENTS ))
      throw  MEDEXCEPTION("Unknown number");
    const int * conn  = getConnectivity(ConnectivityType,Entity,MED_ALL_ELEMENTS);
    const int * index = getConnectivityIndex(ConnectivityType,Entity);
    lgth=index[Number]-index[Number-1];
    return conn+index[Number-1]-1;
  }
  else
  {
    if(_constituent==NULL)
      calculateDescendingConnectivity();
    return _constituent->getConnectivityOfAnElement(ConnectivityType,Entity,Number,lgth);
  }
}

/*!
  Perform a deep comparison of the 2 connectivities in NODAL mode and on all elements.
*/
bool CONNECTIVITY::deepCompare(const CONNECTIVITY& other) const
{
  CONNECTIVITY* temp=(CONNECTIVITY* )this;
  const int *conn1=temp->getConnectivity(MED_EN::MED_NODAL,_entity,MED_ALL_ELEMENTS);
  int size1=temp->getConnectivityLength(MED_EN::MED_NODAL,_entity,MED_ALL_ELEMENTS);
  temp=(CONNECTIVITY* )(&other);
  const int *conn2=temp->getConnectivity(MED_EN::MED_NODAL,_entity,MED_ALL_ELEMENTS);
  int size2=temp->getConnectivityLength(MED_EN::MED_NODAL,_entity,MED_ALL_ELEMENTS);
  if(size1!=size2)
    return false;
  bool ret=true;
  for(int i=0;i<size1 && ret;i++)
  {
    ret=(conn1[i]==conn2[i]);
  }
  return ret;
}
