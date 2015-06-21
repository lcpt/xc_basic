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

#ifndef MESH_HXX
#define MESH_HXX

#include "MEDMEM.hxx"

#include <string>
#include <vector>
#include <list>
#include <map>

#include "MEDMEM_GMesh.hxx"
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_RCBase.hxx"
#include "MEDMEM_FieldForward.hxx"

namespace MEDMEM {

class CELLMODEL;
class FAMILY;
class GROUP;
class SUPPORT;
class MESH;

MEDMEM_EXPORT ostream & operator<<(ostream &os, const MESH &my);

/*! This class contains all the informations related with a MESH :
  - COORDINATES
  - CONNECTIVITIES
*/

class MEDMEM_EXPORT MESH : public GMESH
{
  //-----------------------//
  //   Attributes
  //-----------------------//

protected :

  int   _numberOfNodes;
  mutable COORDINATE *   _coordinate;
  mutable CONNECTIVITY * _connectivity;

  //////////////////////////////////////////////////////////////////////////////////////
  ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
  //////////////////////////////////////////////////////////////////////////////////////
  ///
  ///  La map suivante donne le lien numérotation optionnelle => numérotation cannonique
  ///  Elle  sera calculée apres l'appel de MEDnumLire(...)
  ///  Et sera utilisée avant chaque appel a MEDconnLire(...) pour renuméroter toutes les mailles de façon canonique [1...n]
  ///  _coordinate->NodeNumber sera utilisé avant chaque appel à MEDconnEcri pour démunéroter les mailles en leur numérotation originelle
  ///  Ce traitement devrait prévenir tout plantage du aux numérotations optionnelles DES NOEUDS
  ///  Et ne ralentira que tres peu les traitements sans numéros optionnels

  int _arePresentOptionnalNodesNumbers;
  map<int,int> _optionnalToCanonicNodesNumbers;

  //-----------------------//
  //   Methods
  //-----------------------//

 public :

  // Add your personnal driver line (step 2)
  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;

  friend class MED_MED_RDONLY_DRIVER21;
  friend class MED_MED_WRONLY_DRIVER21;
  friend class MED_MED_RDWR_DRIVER21;
  friend class MED_MED_RDONLY_DRIVER22;
  friend class MED_MED_WRONLY_DRIVER22;
  friend class MED_MED_RDWR_DRIVER22;

  friend class GIBI_MESH_RDONLY_DRIVER;
  friend class GIBI_MESH_WRONLY_DRIVER;
  friend class GIBI_MESH_RDWR_DRIVER;

  friend class PORFLOW_MESH_RDONLY_DRIVER;
  friend class PORFLOW_MESH_WRONLY_DRIVER;
  friend class PORFLOW_MESH_RDWR_DRIVER;

  friend class VTK_MESH_DRIVER;

  friend class ENSIGHT_MESH_RDONLY_DRIVER;

 public:
  MESH();
  MESH(MESH &m);
  virtual ~MESH();
  MESH( driverTypes driverType, const string & fileName="",
        const string & meshName="") throw (MEDEXCEPTION);
 public:
  void init();
  MESH & operator=(const MESH &m);
  virtual bool operator==(const MESH& other) const;
  virtual bool deepCompare(const GMESH& other) const;
  virtual bool isEmpty() const;

  friend ostream & operator<<(ostream &os, const MESH &my);
  virtual void printMySelf(ostream &os) const;

  virtual int         getMeshDimension() const;
  virtual bool        getIsAGrid() const;
  virtual const MESH* convertInMESH() const;

  inline int                        getNumberOfNodes() const;
  virtual inline const COORDINATE * getCoordinateptr() const;
  inline string                     getCoordinatesSystem() const;
  virtual inline const double *     getCoordinates(MED_EN::medModeSwitch Mode) const;
  virtual inline const double       getCoordinate(int Number,int Axis) const;
  inline const string *             getCoordinatesNames() const;
  inline const string *             getCoordinatesUnits() const;

  virtual inline int getNumberOfTypes(MED_EN::medEntityMesh Entity) const;
  virtual inline const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const;
  virtual inline const CELLMODEL * getCellsTypes(MED_EN::medEntityMesh Entity) const;
  virtual inline string * getCellTypeNames(MED_EN::medEntityMesh Entity) const;
  virtual inline const int * getGlobalNumberingIndex(MED_EN::medEntityMesh Entity) const;
  virtual inline int getNumberOfElements(MED_EN::medEntityMesh Entity,
                                         MED_EN::medGeometryElement Type) const;
  virtual inline bool existConnectivity(MED_EN::medConnectivity ConnectivityType,
                                        MED_EN::medEntityMesh Entity) const;

  virtual inline MED_EN::medGeometryElement getElementType(MED_EN::medEntityMesh Entity,
                                                           int Number) const;
  virtual inline void calculateConnectivity(MED_EN::medConnectivity ConnectivityType,
                                            MED_EN::medEntityMesh Entity) const ;
  virtual inline int getConnectivityLength(MED_EN::medConnectivity ConnectivityType,
                                           MED_EN::medEntityMesh Entity,
                                           MED_EN::medGeometryElement Type) const;
  virtual inline const int * getConnectivity(MED_EN::medConnectivity ConnectivityType,
                                             MED_EN::medEntityMesh Entity,
                                             MED_EN::medGeometryElement Type) const;
  virtual inline const int * getConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
                                                  MED_EN::medEntityMesh Entity) const;

  virtual int                 getElementNumber(MED_EN::medConnectivity ConnectivityType,
                                               MED_EN::medEntityMesh Entity,
                                               MED_EN::medGeometryElement Type,
                                               int * connectivity) const;
  virtual inline int getReverseConnectivityLength(MED_EN::medConnectivity ConnectivityType,
                                                  MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
  virtual inline const int * getReverseConnectivity(MED_EN::medConnectivity ConnectivityType,
                                                    MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
  virtual inline int getReverseConnectivityIndexLength(MED_EN::medConnectivity ConnectivityType,
                                                         MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;
  virtual inline const int * getReverseConnectivityIndex(MED_EN::medConnectivity ConnectivityType,
                                                         MED_EN::medEntityMesh Entity=MED_EN::MED_CELL) const;

  virtual inline const CONNECTIVITY*   getConnectivityptr() const;
  inline void setConnectivityptr(CONNECTIVITY*);
  virtual SUPPORT *                    getBoundaryElements(MED_EN::medEntityMesh Entity) const
                                                throw (MEDEXCEPTION);
  virtual SUPPORT *                    getSkin(const SUPPORT * Support3D)
                                                throw (MEDEXCEPTION);

  //  Node DonneBarycentre(const Cell &m) const;
  virtual FIELD<double>* getVolume (const SUPPORT * Support, bool isAbs = true) const
                                throw (MEDEXCEPTION);
                                // Support must be on 3D elements
  virtual FIELD<double>* getArea (const SUPPORT * Support) const
                                throw (MEDEXCEPTION);
                                // Support must be on 2D elements
  virtual FIELD<double>* getLength (const SUPPORT * Support) const
                                throw (MEDEXCEPTION);
                                // Support must be on 1D elements
  virtual FIELD<double>* getNormal (const SUPPORT * Support) const
                                throw (MEDEXCEPTION);
                                // Support must be on 2D elements
  virtual FIELD<double>* getBarycenter (const SUPPORT * Support) const
                                throw (MEDEXCEPTION);
  //  FIELD<int>* getNeighbourhood(SUPPORT * Support) const
  //                            throw (MEDEXCEPTION); // Il faut preciser !

  SUPPORT *buildSupportOnNodeFromElementList(const list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
  void fillSupportOnNodeFromElementList(const list<int>& listOfElt, SUPPORT *supportToFill) const throw (MEDEXCEPTION);
  int getElementContainingPoint(const double *coord);
  vector< vector<double> > getBoundingBox() const;
  void convertToPoly();
};

// ---------------------------------------
//              Methodes Inline
// ---------------------------------------

inline const CONNECTIVITY* MESH::getConnectivityptr() const
{
//   checkGridFillConnectivity();
  return _connectivity;
}

inline void MESH::setConnectivityptr(CONNECTIVITY* conn)
{
  if(_connectivity)
    delete _connectivity;
  _connectivity=conn;
}

/*! \if MEDMEM_ug
\addtogroup MESH_nodes
@{
\endif
*/
/*! Gets the number of nodes used in the mesh. */
inline int MESH::getNumberOfNodes() const
{
  return _numberOfNodes;
}

/*! \ifnot MEDMEM_ug
 Gets the COORDINATES object. Use it only if you need COORDINATES informations not provided by the MESH class.
\endif
*/
inline const COORDINATE * MESH::getCoordinateptr() const
{
//   checkGridFillCoords();
  return _coordinate;
}

/*! Retrieves the system in which coordinates are given (CARTESIAN,CYLINDRICAL,SPHERICAL). */
inline string MESH::getCoordinatesSystem() const
{
  return _coordinate->getCoordinatesSystem();
}

/*! Gets the whole coordinates array in a given interlacing mode. The interlacing mode are :
  - MED_NO_INTERLACE   :  X1 X2 Y1 Y2 Z1 Z2
  - MED_FULL_INTERLACE :  X1 Y1 Z1 X2 Y2 Z2
 */
inline const double * MESH::getCoordinates(MED_EN::medModeSwitch Mode) const
{
//   checkGridFillCoords();
  return _coordinate->getCoordinates(Mode);
}

/*! Gets the coordinate number \a number on axis \a axis.*/
inline const double MESH::getCoordinate(int number, int axis) const
{
//   checkGridFillCoords();
  return _coordinate->getCoordinate(number,axis);
}

/*! Gets a pointer to the coordinate names array.
*/
inline const string * MESH::getCoordinatesNames() const
{
  return _coordinate->getCoordinatesNames();
}

/*! Gets a pointer to the coordinate units array.
*/
inline const string * MESH::getCoordinatesUnits() const
{
  return _coordinate->getCoordinatesUnits();
}
/*!\if MEDMEM_ug

Here is a small C++ example program for which the
Python version may be found in
\a MESHcoordinates.py.

\example MESHcoordinates.cxx

@}
\endif
*/

//  int * MESH::getNodesNumbers() const
//  {
//    return nodesNumbers;
//  }

/*!\if MEDMEM_ug
\addtogroup MESH_connectivity
@{
\endif
*/

/*! Gets the number of different geometric types for a given entity type.

    For example getNumberOfTypes(MED_CELL) would return 3 if the MESH
    have some MED_TETRA4, MED_PYRA5 and MED_HEXA8 in it.
    If entity is not defined, returns 0.
    If there is no connectivity, returns an exception.

    \param entity  entity type (MED_CELL, MED_FACE, MED_EDGE, MED_NODE, MED_ALL_ENTITIES)
*/
inline int MESH::getNumberOfTypes(MED_EN::medEntityMesh entity) const
{
  MESSAGE_MED("MESH::getNumberOfTypes(medEntityMesh entity) : "<<entity);
  if (entity == MED_EN::MED_NODE)
    return 1;
//   checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getNumberOfTypes(entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getNumberOfTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*!
  Gets the list of geometric types used by a given entity.
 If entity is not defined, it returns an exception.
\param entity Entity type must be MED_CELL, MED_FACE, MED_EDGE or MED_ALL_ENTITIES.
 Passing MED_NODE as an entity type will throw an exception.
*/
inline const MED_EN::medGeometryElement * MESH::getTypes(MED_EN::medEntityMesh entity) const
{
  if (entity == MED_EN::MED_NODE)
    throw MEDEXCEPTION(LOCALIZED("MESH::getTypes( medEntityMesh ) : No medGeometryElement with MED_NODE entity !"));
  // return un tableau de taille 1 contenant MED_NONE, comme les supports pour etre coherent avec getNumberOfTypes ???? PG

//   checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getGeometricTypes(entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getTypes( medEntityMesh ) : Connectivity not defined !"));
}
/*! \if MEDMEM_ug @} \endif */

/*!
  Get the whole list of CELLMODEL used by cells of given type (medEntityMesh).
 Passing MED_NODE as an entity type will throw an exception.
*/
inline const CELLMODEL * MESH::getCellsTypes(MED_EN::medEntityMesh Entity) const
{
  //  checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getCellsTypes(Entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getCellsTypes( medEntityMesh ) : Connectivity not defined !"));
}

/*!
  Get an array (it should deleted after use) of the whole list of CELLMODEL
  Name of a given type (medEntityMesh).

  REMARK : Don't use MED_NODE as medEntityMesh
*/
inline string * MESH::getCellTypeNames(MED_EN::medEntityMesh Entity) const
{
  //  checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getCellTypeNames(Entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getCellTypesName( medEntityMesh ) : Connectivity not defined !"));
}
/*!\if MEDMEM_ug
\addtogroup MESH_connectivity
@{
\endif
*/

/*! Returns an array of size NumberOfTypes+1 which contains, for each
    geometric type of the given entity, the first global element number
    of this type.

    For exemple, if we have a mesh with 5 triangles and 4 quadrangle :
    - size of GlobalNumberingIndex is 3
    - GlobalNumberingIndex[0]=1 (the first type)
    - GlobalNumberingIndex[1]=6 (the second type)
    - GlobalNumberingIndex[2]=10
*/
inline const int * MESH::getGlobalNumberingIndex(MED_EN::medEntityMesh entity) const
{
  //  checkGridFillConnectivity();
  if (_connectivity != NULL)
    return _connectivity->getGlobalNumberingIndex(entity);
  throw MEDEXCEPTION(LOCALIZED("MESH::getNumberOfTypes( medEntityMesh ) : Connectivity not defined !"));
}
/*!
  Returns the number of elements of given geometric type of given entity. Returns 0 if query is not defined.

  Example :
  - getNumberOfElements(MED_NODE,MED_NONE) : number of nodes
  - getNumberOfElements(MED_NODE,MED_TRIA3) : returns 0 (not defined)
  - getNumberOfElements(MED_FACE,MED_TRIA3) : returns number of triangle
  elements defined in face entity (0 if not defined)
  - getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS) : returns total number
  of elements defined in cell entity
 */
inline int MESH::getNumberOfElements(MED_EN::medEntityMesh entity, MED_EN::medGeometryElement Type) const
  {
    int retval= 0;
    if(entity==MED_EN::MED_NODE)
      {
        if((Type==MED_EN::MED_NONE)|(Type==MED_EN::MED_ALL_ELEMENTS))
          retval= _numberOfNodes;
      }
    else
      {
//       checkGridFillConnectivity();
        if(_connectivity != (CONNECTIVITY*)NULL)
        retval= _connectivity->getNumberOf(entity,Type);
      }
    return retval;
  }

/*! \if MEDMEM_ug

This example shows the use of connectivity retrieval methods on a mesh which
corresponds to the four-element mesh given in figure ??. Note
the use of connectivity and connnectivity index tables, and the
offsets used to convert Fortran-style numbering to C arrays.

The output of this program reads :
\code
Number of types : 3
Type #0
Element 1 : 1 2 3
Type #1
Element 1 : 2 4 5 3
Element 2 : 5 6 7 8
Type #2
Element 1 : 4 6 5 10 11 9
\endcode

\example read_example.cxx

A more complete example involving descending connectivities
can be found in \c MESHconnectivities.cxx and \c MESHconnectivities.py.

@}
\endif
*/
/*!
  Returns true if the wanted connectivity exist, else returns false
  (to use before a getSomething method).
 */
inline bool MESH::existConnectivity(MED_EN::medConnectivity connectivityType, MED_EN::medEntityMesh entity) const
{
//   checkGridFillConnectivity();
  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("MESH::existConnectivity(medConnectivity,medEntityMesh) : no connectivity defined !");
  return _connectivity->existConnectivity(connectivityType,entity);
}
/*!\if MEDMEM_ug
\addtogroup MESH_connectivity
@{
\endif
*/

/*!
  Returns the geometric type of global element number \a Number of entity \a Entity.

  Throw an exception if \a Entity is not defined or if \a Numberis wrong.
 */
inline MED_EN::medGeometryElement MESH::getElementType(MED_EN::medEntityMesh Entity,int Number) const
{
  //  checkGridFillConnectivity();
  if (_connectivity==(CONNECTIVITY*)NULL)
    throw MEDEXCEPTION("MESH::getElementType(medEntityMesh,int) : no connectivity defined !");
  return _connectivity->getElementType(Entity,Number);
}
/*!
\if MEDMEM_ug
@}
\endif
 */

/*!
  Calculates the required connectivity. Returns an exception if this could not be
  done. Do nothing if connectivity already exist.
 */

inline void MESH::calculateConnectivity(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh entity) const
{
  _connectivity->calculateConnectivity(ConnectivityType,entity);
}
/*!
 Returns the corresponding length of the array returned by MESH::getConnectivity with exactly the same arguments.
 Used particulary for wrapping CORBA and python.
 */
inline int MESH::getConnectivityLength(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh entity, MED_EN::medGeometryElement Type) const
{
  int nbOfElm = getNumberOfElements(entity,Type);
  int size;

  if (Type == MED_EN::MED_ALL_ELEMENTS)
    {
      size = getConnectivityIndex(ConnectivityType,entity)[nbOfElm]-1;
    }
  else
    {
      size = _connectivity->getConnectivityLength(ConnectivityType,entity,Type); // issue 19983
    }
  return size;
}

/*!\if MEDMEM_ug
\addtogroup MESH_connectivity
@{
\endif
*/

/*!
  Returns the required connectivity in mode \a Mode for the
  geometric type \a Type of the entity type \a entity.
\a ConnectivityType specifies descending or nodal connectivity.

  To get connectivity for all geometric type, use \a Mode=MED_FULL_INTERLACE
  and \a Type=MED_ALL_ELEMENTS.
  You must also get the corresponding index array.
 */
inline const int * MESH::getConnectivity(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh entity, MED_EN::medGeometryElement Type) const
  {
    return _connectivity->getConnectivity(ConnectivityType,entity,Type);
  }
/*!
  Returns the required index array for a connectivity received in
  MED_FULL_INTERLACE mode and MED_ALL_ELEMENTS type.

  This array allows to find connectivity of each element.

  Example : Connectivity of i-th element (1<=i<=NumberOfElement) begins
  at index ConnectivityIndex[i-1] and ends at index ConnectivityIndex[i]-1
  in Connectivity array (Connectivity[ConnectivityIndex[i-1]-1] is the
  first node of the element)
 */
inline const int * MESH::getConnectivityIndex(MED_EN::medConnectivity    ConnectivityType,
                                              MED_EN::medEntityMesh      entity) const
{
  return _connectivity->getConnectivityIndex(ConnectivityType, entity);
}

/*!
\if MEDMEM_ug
@}
\endif
 */

/*!
  Returns the corresponding length of the array returned by MESH::getReverseConnectivity with exactly the same arguments.
  Used particulary for wrapping CORBA and python.
 */

inline int MESH::getReverseConnectivityLength(MED_EN::medConnectivity ConnectivityType, MED_EN::medEntityMesh   Entity) const
  {
    int spaceDim = getSpaceDimension();
    int nb= 0;

    if(ConnectivityType == MED_EN::MED_NODAL)
      { nb = getNumberOfNodes(); }
    else
      {
        if(spaceDim == 2)
          nb = getNumberOfElements(MED_EN::MED_EDGE, MED_EN::MED_ALL_ELEMENTS);
        else if (spaceDim == 3)
          nb = getNumberOfElements(MED_EN::MED_FACE, MED_EN::MED_ALL_ELEMENTS);
      }
    return getReverseConnectivityIndex(ConnectivityType,Entity)[nb]-1;
  }
/*!
  Returns the reverse connectivity required by ConnectivityType :
  - If ConnectivityType=MED_NODAL : returns connectivity node-cell
  - If ConnectivityType=MED_DESCENDING : returns connectivity face-cell

  You must get ReverseConnectivityIndex array to use it.
 */

inline const int * MESH::getReverseConnectivity(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh Entity/*=MED_CELL*/) const
{
  //  checkGridFillConnectivity();
  if (NULL==_connectivity)
    throw MEDEXCEPTION("MESH::getReverseConnectivity : no connectivity defined in MESH !");

  return _connectivity->getReverseConnectivity(ConnectivityType,Entity);
}
/*!
  Returns the corresponding length of the array returned by MESH::getReverseConnectivityIndex with exactly the same arguments.
  Used particulary for wrapping CORBA and python.
 */
inline int MESH::getReverseConnectivityIndexLength(MED_EN::medConnectivity ConnectivityType,
                                                         MED_EN::medEntityMesh Entity) const
{
  int spaceDim = getSpaceDimension();

  if (ConnectivityType == MED_EN::MED_NODAL)
    {
      return getNumberOfNodes()+1;
    }
  else
    {
      if (spaceDim == 2)
        return getNumberOfElements(MED_EN::MED_EDGE,MED_EN::MED_ALL_ELEMENTS)+1;
      else if (spaceDim == 3)
        return getNumberOfElements(MED_EN::MED_FACE,MED_EN::MED_ALL_ELEMENTS)+1;
      else
        throw MEDEXCEPTION("Invalid dimension");
    }
}
/*!
  Returns the index array required by ConnectivityType.

  This array allow to find reverse connectivity of each elements.

  Example : Reverse connectivity of i^{th} elements (1<=i<=NumberOfElement)
  begin at index ReverseConnectivityIndex[i-1] and end at index
  ReverseConnectivityIndex[i]-1
  in ReverseConnectivity array (
  ReverseConnectivity[ReverseConnectivityIndex[i-1]-1]
  is the first value)
 */
inline const int * MESH::getReverseConnectivityIndex(MED_EN::medConnectivity ConnectivityType,MED_EN::medEntityMesh Entity/*=MED_CELL*/) const
{
  //  checkGridFillConnectivity();
  if (NULL==_connectivity)
    throw MEDEXCEPTION("MESH::getReverseConnectivityIndex : no connectivity defined in MESH !");

  return _connectivity->getReverseConnectivityIndex(ConnectivityType,Entity);
}

}

#endif /* MESH_HXX */
