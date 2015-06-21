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

#ifndef CONNECTIVITY_HXX
#define CONNECTIVITY_HXX

#include "MEDMEM.hxx"

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_CellModel.hxx"

#include "InterpKernelHashMap.hxx"

#include <vector>
#include <set>
#include <map>

namespace MEDMEM {
  class MEDSKYLINEARRAY;
  class FAMILY;
  class GROUP;
  class CONNECTIVITY;
  MEDMEM_EXPORT std::ostream & operator<<(std::ostream &os, CONNECTIVITY &my);

/*!
        This class deals with all types of connectivity.\n
        It is a recursive class.
*/

/* ------------------------------------------- */
class MEDMEM_EXPORT CONNECTIVITY
/* ------------------------------------------- */
{

  class myHashFn
  {
  public:
    size_t operator()(const std::vector<int>& key) const
    {
      size_t sum=0;
      for (int i=0; i<(int)key.size(); i++)
        //sum+=key[i]; -- KO for conn of edges of face [1,6,5,2] since 1+6==2+5
        sum+=key[i]*(i+key.size());
      return sum;
    }
// #ifdef WNT
//     static const size_t bucket_size = 4;
//     static const size_t min_buckets = 8;
//     bool operator()(const vector<int>& key1, const vector<int>& key2) const
//     {
//       return ::size_t()(key1) < ::size_t()(key2);
//     }
// #endif
  };

  typedef INTERP_KERNEL::HashMap<std::vector<int>,int, myHashFn > CONNECTIVITY_HashMap;

  /* ---------------------- */
  /*    Class Attributs     */
  /* ---------------------- */

protected:
                                         /*! contains MED_CELL MED_FACE or MED_EDGE */
  MED_EN::medEntityMesh           _entity;
                                         /*! contains MED_NODAL or MED_DESCEND */
  MED_EN::medConnectivity         _typeConnectivity;
                                         /*! count of differents cells types
                                            used by the mesh */
  int                             _numberOfTypes;
                                        /*! array of all med_geometric_type used by MESH. */
  MED_EN::medGeometryElement*     _geometricTypes;

                                        /*! map indexed by med_geometric_type
                                            which contains the different
                                            'CellModel' used by MESH. */
  CELLMODEL *      _type;
                                        /*! contains the dimension of the entity */
  int              _entityDimension;

                                        /*! needed by calculateReverseNodalConnectivity */
  int              _numberOfNodes;

                                         /*! array of size _numberOfTypes+1 which
                                         gives for each cell type the first
                                         cell number in _nodal or _descending
                                         array (depends on _typeConnectivity)
                                         To get cells count for one type, we
                                         must minus _count[i+1] and _count[i]
                                         ( 0 <= i < _numberOfTypes ).
                                         Note that _count[_numberOfTypes] returns
                                         total cells count + 1 */
  int *            _count;

                                        /*! pointer to an array which stores the nodal connectivity */
  MEDSKYLINEARRAY* _nodal;
                                        /*! pointer to an array which stores
                                            the descending connectivity */
  mutable MEDSKYLINEARRAY* _descending;
                                        /*! pointer to an array which stores
                                            the reverse nodal connectivity */
  mutable MEDSKYLINEARRAY* _reverseNodalConnectivity;
                                        /*! pointer to an array which stores
                                            the reverse descending connectivity */
  mutable MEDSKYLINEARRAY* _reverseDescendingConnectivity;
                                        /*! if face or edge, list of 2 cells or
                                            2 faces it belongs to.
                                            If 2nd number equals 0, we have a
                                            boundary entity. We could use
                                            MEDSKYLINEARRAY, but we suppose we have
                                            always only 2 (or 1) entities. */
  MEDSKYLINEARRAY* _neighbourhood;
                                        /*! connectivity of sub cell if
                                            descendant connectivity is calculated */
  CONNECTIVITY *   _constituent;
                                        /*! is descending connectivity computed by 
                                            calculatePartialDescendingConnectivity() */
  mutable bool             _isDescendingConnectivityPartial;

  /* -------------------- */
  /*    Class Methods     */
  /* -------------------- */

private:
                                        /*! private method :\n
                                            does nothing if already exists, else
                                            evaluates _nodal from _descending */
  void calculateNodalConnectivity() const;
                                        /*! private method :\n
                                            does nothing if already exists, else
                                            evaluates from _nodal */
  void calculateReverseNodalConnectivity() const;
                                        /*! private method :\n
                                            does nothing if already exists, else
                                            evaluates _descending from _nodal */
  void calculateDescendingConnectivity() const;

  void calculatePartialDescendingConnectivity() const;
  void addToDescendingConnectivity( const std::set<int>&    nodes,
                                    std::multimap<int,int>& descending,
                                    int                     iglobal_cell ,
                                    const CONNECTIVITY_HashMap & ) const;
        
                                        /*! private method :\n
                                          does nothing if already exists, else
                                          evaluates from _descending */
  //  void calculateReverseDescendingConnectivity(CONNECTIVITY *myConnectivity);

  const int*      getReverseNodalConnectivity           () const;
  const int*      getReverseNodalConnectivityIndex      () const;
  const int*      getReverseDescendingConnectivity      () const;
  const int*      getReverseDescendingConnectivityIndex () const;

                                        /*! private method :\n
                                          does nothing if already exists, else
                                          evaluates _neighbourhood from _descending */
  void calculateNeighbourhood(CONNECTIVITY &myConnectivity);

  int getIndexOfEndClassicElementInReverseNodal(const int *reverseNodalValue,
                                                const int *reverseNodalIndex, int rk)  const;

public:

  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;


  friend std::ostream & operator<<(std::ostream &os, CONNECTIVITY &connectivity);

  // in order to fill CONNECTIVITY of MESH
  friend class GRID;

  CONNECTIVITY  (MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
  CONNECTIVITY  (int numberOfTypes, MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
  CONNECTIVITY  (const CONNECTIVITY & m);
  virtual ~CONNECTIVITY ();

  void setConstituent (CONNECTIVITY * Constituent)
                                        throw (MEDEXCEPTION);
  const CONNECTIVITY* getConstituent (const MED_EN::medEntityMesh Entity) const
                                        throw (MEDEXCEPTION);

  void setGeometricTypes (const MED_EN::medGeometryElement * Types,
                          const MED_EN::medEntityMesh        Entity)
                                        throw (MEDEXCEPTION);

  void setCount (const int *                 Count,
                 const MED_EN::medEntityMesh Entity)
                                        throw (MEDEXCEPTION);

  void setNodal (const int *                      Connectivity,
                 const MED_EN::medEntityMesh      Entity,
                 const MED_EN::medGeometryElement Type,
                 const int *                      PolyConnectivityIndex=0)
                                        throw (MEDEXCEPTION);

  inline void setNumberOfNodes(int NumberOfNodes);
  inline int  getNumberOfNodes() const;
  inline int  getEntityDimension() const;

  inline void setEntityDimension(int EntityDimension);

  inline bool existConnectivity (MED_EN::medConnectivity connectivityType,
                                 MED_EN::medEntityMesh   Entity) const;

  virtual void calculateConnectivity (MED_EN::medConnectivity connectivityType,
                                      MED_EN::medEntityMesh   Entity);

  virtual void calculateFullDescendingConnectivity(MED_EN::medEntityMesh Entity);

  virtual void updateFamily (const vector<FAMILY*>& myFamilies);

  inline MED_EN::medEntityMesh  getEntity () const;

  inline int                    getNumberOfTypes (MED_EN::medEntityMesh Entity) const;

  const int *  getConnectivityOfAnElement(MED_EN::medConnectivity ConnectivityType,
                                          MED_EN::medEntityMesh Entity,
                                          int Number, int &lgth) const;

  inline const MED_EN::medGeometryElement * getGeometricTypes (MED_EN::medEntityMesh Entity) const
                                                             throw (MEDEXCEPTION);
  MED_EN::medGeometryElement                getElementType    (MED_EN::medEntityMesh Entity,
                                                               int Number)           const;
  inline MED_EN::medGeometryElement         getPolyTypeRelativeTo() const;
  virtual inline const int *                getGlobalNumberingIndex (MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);

  virtual const int* getConnectivity             (MED_EN::medConnectivity    ConnectivityType,
                                                  MED_EN::medEntityMesh      Entity,
                                                  MED_EN::medGeometryElement Type) const;
  virtual int        getConnectivityLength       (MED_EN::medConnectivity    ConnectivityType,
                                                  MED_EN::medEntityMesh      Entity,
                                                  MED_EN::medGeometryElement Type) const;
  virtual const int* getConnectivityIndex        (MED_EN::medConnectivity ConnectivityType,
                                                  MED_EN::medEntityMesh   Entity) const;
  int *              getNodesOfPolyhedron(int polyhedronId, int& lgthOfTab) const;
  int **             getNodesPerFaceOfPolyhedron(int polyhedronId, int& nbOfFaces, int* & nbOfNodesPerFaces) const;

  const CELLMODEL &  getType                  (MED_EN::medGeometryElement Type) const;
  const CELLMODEL *  getCellsTypes            (MED_EN::medEntityMesh Entity)    const
                                            throw (MEDEXCEPTION);
  string *           getCellTypeNames         (MED_EN::medEntityMesh Entity) const
                                            throw (MEDEXCEPTION);

  int                getNumberOfNodesInType   (MED_EN::medGeometryElement Type) const;
  int                getNumberOfSubCellInType (MED_EN::medGeometryElement Type) const;
  virtual int        getNumberOf              (MED_EN::medEntityMesh      Entity,
                                               MED_EN::medGeometryElement Type) const;
  virtual const int* getValue                 (MED_EN::medConnectivity    TypeConnectivity,
                                               MED_EN::medGeometryElement Type) const;
  virtual const int* getValueIndex            (MED_EN::medConnectivity    TypeConnectivity) const;

  virtual inline const int* getReverseConnectivity (MED_EN::medConnectivity ConnectivityType,
                                                    MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const
                                            throw (MEDEXCEPTION);
  virtual inline const int* getReverseConnectivityIndex (MED_EN::medConnectivity ConnectivityType,
                                                         MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const
                                            throw (MEDEXCEPTION);

  const int*         getNeighbourhood() const;
  void invertConnectivityForAFace(int faceId, const int *nodalConnForFace);
  bool deepCompare(const CONNECTIVITY& other) const;


};
/*----------------------*/
/* Methodes Inline      */
/*----------------------*/

/*! Returns the  medEntityMesh */
//--------------------------------------------------//
inline MED_EN::medEntityMesh CONNECTIVITY::getEntity() const
//--------------------------------------------------//
  { return _entity; }

/*!  Returns the number of different %medGeometryElement types
     existing in the specified entity. \n
     Note : Not implemented for MED_ALL_ENTITIES. */
//-----------------------------------------------------------------------//
inline int CONNECTIVITY::getNumberOfTypes(MED_EN::medEntityMesh Entity) const
//-----------------------------------------------------------------------//
{
  MESSAGE_MED("CONNECTIVITY::getNumberOfTypes : Entity = "<<Entity<<", _entity = "<<_entity);
  if (_entity==Entity)
    return _numberOfTypes;
  else if (_constituent!=NULL)
    return _constituent->getNumberOfTypes(Entity);
  else if (_constituent == NULL)
    {
      MESSAGE_MED("CONNECTIVITY::getNumberOfTypes : _constituent == NULL");
      try
        {
          (const_cast <CONNECTIVITY *> (this))->calculateDescendingConnectivity();
        }
      catch (MEDEXCEPTION &)
        {
          return 0 ;
        }

      SCRUTE_MED(_entityDimension);

      if (_entityDimension != 2 && _entityDimension != 3) return 0;

      try
        {
          _constituent->calculateConnectivity(MED_EN::MED_NODAL,Entity);
        }
      catch (MEDEXCEPTION &)
        {
          return 0 ;
        }

      return _constituent->getNumberOfTypes(Entity);
    }
  else
        return 0; // because it is the right information (no exception needed)!
}

/*!  Returns an array of all %medGeometryElement types existing in the mesh
     for the given %medEntityMesh.
     Note : Not implemented for MED_ALL_ENTITIES.
     Throws an exception if Entity is unknown */
//------------------------------------------------------------------------------------------//
inline const MED_EN::medGeometryElement* CONNECTIVITY::getGeometricTypes(MED_EN::medEntityMesh Entity) const
                                                throw (MEDEXCEPTION)
//------------------------------------------------------------------------------------------//
{
  if (_entity==Entity)
    return _geometricTypes;
  else if (_constituent!=NULL)
    return _constituent->getGeometricTypes(Entity);
  else
    //throw MEDEXCEPTION("CONNECTIVITY::getGeometricTypes : Entity not defined !");
    return 0; // idem to supressed getGeometricTypesWithPoly()
}

/*!   Returns an array containing the accumulated number of entities sorted by the geometric type.\n

      Exemple :\n

      In case of a CONNECTIVITY containing 3*MED_TRIA3 et 2*MED_QUAD4 : \n
            int * count = getGlobalNumberingIndex(MED_CELL)\n
      count[0] is always set to 1\n
      count[1] is set to 1+3=4\n
      count[2] is set to 4+2=6 = total number of cells + 1\n

      Note : Not implemented for MED_ALL_ENTITIES. \n
      Note : The geometric type order is given by the typedef enum medGeometryElement.

*/
//----------------------------------------------------------------------------------//
inline const int * CONNECTIVITY::getGlobalNumberingIndex(MED_EN::medEntityMesh Entity) const
                                                throw (MEDEXCEPTION)
//----------------------------------------------------------------------------------//
{
  if (_entity==Entity)
        return _count;
  else if (_constituent!=NULL)
        return _constituent->getGlobalNumberingIndex(Entity);
  else
        throw MEDEXCEPTION("CONNECTIVITY::getGlobalNumberingIndex : Entity not defined !");
}

/*!  Returns true if a connectivity exists on elements of type "Entity" */
//-----------------------------------------------------------------------------//
inline bool CONNECTIVITY::existConnectivity( MED_EN::medConnectivity ConnectivityType,
                                             MED_EN::medEntityMesh Entity) const
//-----------------------------------------------------------------------------//
{
  if (_entity==Entity) {
    if ((ConnectivityType == MED_EN::MED_NODAL) && (_nodal != (MEDSKYLINEARRAY*)NULL))
      return true;
    if ((ConnectivityType==MED_EN::MED_DESCENDING)&(_descending!=(MEDSKYLINEARRAY*)NULL))
      return true;
  } else if (_constituent!=NULL)
    return _constituent->existConnectivity(ConnectivityType,Entity);
  return false;
}

/*!
Returns an array containing CELLMODEL foreach element type present
in connectivity for given medEntityMesh (similar as getGeometricTypes).\n
Throw an execption if the given entity is not defined or if the array is not defined.
*/
//-----------------------------------------------------------------------------//
inline const CELLMODEL * CONNECTIVITY::getCellsTypes(MED_EN::medEntityMesh Entity) const
                                                throw (MEDEXCEPTION)
//-----------------------------------------------------------------------------//
{
  if (Entity == _entity)
    if (_type!=NULL)
      return _type;
    else
      throw MEDEXCEPTION("CONNECTIVITY::getCellsTypes(medEntityMesh) :"
                         " CELLMODEL array is not defined !");
  else
    if (_constituent != NULL)
      return _constituent->getCellsTypes(Entity);
    else
      throw MEDEXCEPTION("CONNECTIVITY::getCellsTypes(medEntityMesh) : Not found Entity !");
}
/*!
Returns an array (it should deleted after use) containing the whole list of
CELLMODEL Name foreach element type present in connectivity for given
medEntityMesh (similar as getGeometricTypes).\n
Throw an execption if the given entity is not defined or if the array is not
defined.

*/
//-----------------------------------------------------------------------------//
inline string * CONNECTIVITY::getCellTypeNames(MED_EN::medEntityMesh Entity) const
                                                throw (MEDEXCEPTION)
//-----------------------------------------------------------------------------//
{
  if (Entity == _entity)
    if (_type!=NULL)
      {
        string * stringArray = new string[_numberOfTypes];

        for (int i=0;i<_numberOfTypes;i++)
          stringArray[i] = _type[i].getName();

        return stringArray;
      }
    else
      throw MEDEXCEPTION("CONNECTIVITY::getCellTypeNames(medEntityMesh) :"
                         " CELLMODEL array is not defined !");
  else
    if (_constituent != NULL)
      return _constituent->getCellTypeNames(Entity);
    else
      throw MEDEXCEPTION("CONNECTIVITY::getCellTypeNames(medEntityMesh) : Not found Entity !");
}






/*! A DOCUMENTER */
//------------------------------------------------------------------------------------------//
inline const int* CONNECTIVITY::getReverseConnectivity( MED_EN::medConnectivity ConnectivityType,
                                                        MED_EN::medEntityMesh   Entity) const
                                                            throw (MEDEXCEPTION)
//------------------------------------------------------------------------------------------//
{
  if(_entity==Entity)
  {
    if (ConnectivityType==MED_EN::MED_NODAL)
      return getReverseNodalConnectivity();
    else if (ConnectivityType==MED_EN::MED_DESCENDING)
      return getReverseDescendingConnectivity();
    else
      throw MEDEXCEPTION("MESH::getReverseConnectivity : connectivity mode not supported !");
  }
  // other entity :
  if (NULL==_constituent)
    calculateDescendingConnectivity();
  return _constituent->getReverseConnectivity(ConnectivityType,Entity);
}

/*! A DOCUMENTER */
//-----------------------------------------------------------------------------------------------//
inline const int* CONNECTIVITY::getReverseConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
                                                            MED_EN::medEntityMesh   Entity) const
                                                                throw (MEDEXCEPTION)
//-----------------------------------------------------------------------------------------------//
{
  if(_entity==Entity)
  {
    if (ConnectivityType==MED_EN::MED_NODAL)
      return getReverseNodalConnectivityIndex();
    else if (ConnectivityType==MED_EN::MED_DESCENDING)
      return getReverseDescendingConnectivityIndex();
    else
      throw MEDEXCEPTION("MESH::getReverseConnectivityIndex : connectivity mode not supported !");
  }
  // other entity :
  if (NULL==_constituent)
    calculateDescendingConnectivity();
  return _constituent->getReverseConnectivityIndex(ConnectivityType,Entity);
}


inline void CONNECTIVITY::setNumberOfNodes(int NumberOfNodes)
{
  _numberOfNodes=NumberOfNodes;
}

inline int CONNECTIVITY::getNumberOfNodes() const
{
  return _numberOfNodes;
}

inline void CONNECTIVITY::setEntityDimension(int EntityDimension)
{
  _entityDimension=EntityDimension;
}

int CONNECTIVITY::getEntityDimension() const
{
  return _entityDimension;
}

MED_EN::medGeometryElement CONNECTIVITY::getPolyTypeRelativeTo() const
{
  if(_entity==MED_EN::MED_CELL && _entityDimension==3)
    return MED_EN::MED_POLYHEDRA;
  else if((_entity==MED_EN::MED_CELL && _entityDimension==2) || (_entity==MED_EN::MED_FACE && _entityDimension==2))
    return MED_EN::MED_POLYGON;
  else
    throw MEDEXCEPTION("getPolyTypeRelativeTo : ");
}




}//End namespace MEDMEM

#endif /* CONNECTIVITY_HXX */


