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
  File Support.hxx
*/

#ifndef SUPPORT_HXX
#define SUPPORT_HXX

#include "MEDMEM.hxx"

#include <list>
#include <vector>
#include <string>
#include <list>

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_RCBase.hxx"


namespace MEDMEM {

  class GMESH;
  class MESH;

/*!

  This class describe a support of elements on an entity of the mesh.

  It contains the list of meshes elements for an entity (MED_NODE,
  MED_CELL, MED_FACE or MED_EDGE).

*/
class MEDMEM_EXPORT SUPPORT : public RCBASE
{
protected:
  /*!
    \if developper
    Support name.
    \endif
  */
  std::string                   _name;

 /*!
    \if developper
    Name of the associated mesh if the _mesh pointer
    is NULL.
    \endif
  */
  mutable std::string                   _meshName;

  /*!
    \if developper
    Description of the support (optional).
    \endif
  */
  std::string                   _description;

  /*!
    \if developper
    Reference to the mesh on which the support is defined.
    \endif
  */
  mutable const GMESH *            _mesh;

  /*!
    \if developper
    Type of entity on which the support is defined
    (only one for each support).
    \endif
  */
  MED_EN::medEntityMesh            _entity ;

  /*!
    \if developper
    Number of geometric type defined in the support.
    \endif
  */
  int                      _numberOfGeometricType;

  /*!
    \if developper
    Array of all geometric type defined in the support.
    \endif
  */
  PointerOf<MED_EN::medGeometryElement>     _geometricType;

  /*!
    \if developper
    If true, we consider all entities of type _entity
    defined in the associated mesh
    \endif
  */
  bool                     _isOnAllElts;

  /*!
    \if developper
    Index array of size _numberOfGeometricType wich contains
    for each geometric type, the number of elements of this type.
    \endif
  */
  PointerOf<int>                    _numberOfElements;

  /*!
    \if developper
    Sum of each _numberOfElements component.
    \endif
  */
  int                      _totalNumberOfElements;

  // the two following arrays are defined only if _isOnAllElts is false :

  /*!
    \if developper
    Array of size _index[_numberOfType]-1 wich contain number of
    entities of each geometric type. We use global numbering.\n
    Defined only if _isOnAllElts is false.
    \endif
  */
  mutable MEDSKYLINEARRAY * _number;

  /*!
    \if developper
    Array of size _index[_numberOfType]-1 wich contain number of
    entities of each geometric type. We use file numbering.\n
    Defined only if _isOnAllElts is false.
    \endif
  */
  mutable MEDSKYLINEARRAY * _number_fromfile;

  /*!
    \if developper
    Array of size <_numberOfGeometricType> wich contain the profil name of 
    entities of each geometric type.\n
    Defined only if _isOnAllElts is false.
    If it exist an entities list on a geometric type in _number but there is no profil name associated
    ( MED_NOPFL ) the MED driver will consider and verify this entities list as being all the
    entities available on the associated mesh for this geometric type.
    \endif
  */

  std::vector< std::string > _profilNames;
public:
  SUPPORT();
  //SUPPORT(GMESH* Mesh, std::string Name="", MED_EN::medEntityMesh Entity=MED_EN::MED_CELL);
  SUPPORT(const SUPPORT & m);
public:
  friend MEDMEM_EXPORT ostream & operator<<(ostream &os,const SUPPORT &my);

  SUPPORT& operator=(const SUPPORT &support);
  bool operator == (const SUPPORT &support) const;
  bool deepCompare(const SUPPORT &support) const;
  void update();

  inline void setName(const std::string& Name);
  inline void setDescription(const std::string& Description);
  void        setMesh(const GMESH *Mesh) const;
  void        setMeshName(const string & meshName);
  inline void setAll(bool All);
  void        setEntity(MED_EN::medEntityMesh Entity);
  void        setNumberOfGeometricType(int NumberOfGeometricType);
  void        setGeometricType(const MED_EN::medGeometryElement *GeometricType);
  void        setNumberOfElements(const int *NumberOfElements);
  void        setNumber(MEDSKYLINEARRAY * Number);
  void        setNumber(const int * index, const int* value, bool shallowCopy=false);

  inline const std::string&    getName() const;
  inline const std::string&    getDescription() const;
  virtual inline const GMESH * getMesh() const;
  std::string                  getMeshName() const;
  inline MED_EN::medEntityMesh getEntity() const;

  inline bool   isOnAllElements() const;
  inline int    getNumberOfTypes() const;
  inline const MED_EN::medGeometryElement* getTypes() const ;
  int           getNumberOfElements(MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  const int *   getNumberOfElements() const throw (MEDEXCEPTION);
  virtual MEDSKYLINEARRAY *   getnumber() const throw (MEDEXCEPTION);
  virtual MEDSKYLINEARRAY *   getnumberFromFile() const throw (MEDEXCEPTION);
  virtual const int *         getNumber(MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  virtual const int *         getNumberFromFile(MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  virtual        const int *  getNumberIndex() const throw (MEDEXCEPTION);
  virtual int getValIndFromGlobalNumber(const int number) const throw (MEDEXCEPTION);

  void blending(const SUPPORT * mySupport) throw (MEDEXCEPTION) ;

  // Les numéros d'entités dans les profils doivent être croissant
  // pour respecter la norme MED
  void setpartial(const std::string& Description, int NumberOfGeometricType,
                  int TotalNumberOfEntity, const MED_EN::medGeometryElement *GeometricType,
                  const int *NumberOfEntity, const int *NumberValue);

  void setpartial(MEDSKYLINEARRAY * number, bool shallowCopy=false) throw (MEDEXCEPTION);


  void setpartial_fromfile(MEDSKYLINEARRAY * number, bool shallowCopy=false) throw (MEDEXCEPTION);
  
  // Si les noms de profils ne sont pas positionnés, les profils ne seront
  // pas écrits par MEDFICHIER.
  void   setProfilNames(const std::vector<std::string>& profilNames) throw (MEDEXCEPTION);
  //string getProfilName(const MED_EN::medGeometryElement GeometricType) const throw (MEDEXCEPTION);
  std::vector<std::string> getProfilNames() const throw (MEDEXCEPTION);

  void getBoundaryElements() throw (MEDEXCEPTION);
  void changeElementsNbs(MED_EN::medEntityMesh entity, const int *renumberingFromOldToNew);
  void intersecting(const SUPPORT * mySupport) throw (MEDEXCEPTION) ;
  bool belongsTo(const SUPPORT& other, bool deepCompare=false) const;
  SUPPORT *getComplement() const;
  SUPPORT *substract(const SUPPORT& other) const throw (MEDEXCEPTION) ;
  SUPPORT *getBoundaryElements(MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION);
  SUPPORT* buildSupportOnNode() const throw (MEDEXCEPTION);
  void fillFromNodeList(const std::list<int>& listOfNode) throw (MEDEXCEPTION);
  void fillFromElementList(const std::list<int>& listOfElt) throw (MEDEXCEPTION);
  void clearDataOnNumbers();
  MESH* makeMesh() const;
 protected:
  virtual ~SUPPORT();
 protected:
  static std::list<int> *sub(int start,int end,const int *idsToSuppress,int lgthIdsToSuppress);
  static std::list<int> *sub(const int *ids,int lgthIds,const int *idsToSuppress,int lgthIdsToSuppress);
};

// _____________________
// Methodes Inline
// _____________________

/*! set the attribute _name to Name */
//--------------------------------------
inline void SUPPORT::setName(const std::string& Name)
//--------------------------------------
{
  _name=Name;
}

/*! set the attribute _description to Description */
//--------------------------------------------------
inline void SUPPORT::setDescription(const std::string& Description)
//--------------------------------------------------
{
  _description=Description;
}

  /*! \if MEDMEM_ug
\addtogroup SUPPORT_creation
@{
\endif
  */

/*! Creates a support on all elements of the type specified in the constructor.

  Even if _isonAllElts is true, geometric types defining the FIELD's SUPPORT
  must be read from the SUPPORT not from the associated GMESH (the geometric
  types defining the FIELD's SUPPORT may be a subset of the geometric types
  defined in the GMESH even if for each SUPPORT geometric type all MESH entities
  are used).
*/
//------------------------------------------
inline void SUPPORT::setAll(bool All)
//------------------------------------------
{
  _isOnAllElts=All;
}
  /*! \if MEDMEM_ug  @} \endif */

/*! returns the name of the support. */
//------------------------------------
inline const std::string& SUPPORT::getName() const
//------------------------------------
{
  return _name;
}

/*! returns the description of the support. */
//--------------------------------------------
inline const std::string& SUPPORT::getDescription() const
//--------------------------------------------
{
  return _description;
}

/*! returns a reference to the mesh */
//------------------------------------
inline const GMESH * SUPPORT::getMesh() const
//------------------------------------
{
  return _mesh;
}

/*!
  Returns true if all elements of this entity are
  concerned, false otherwise.
  If true, you must use mesh reference (getMesh) to get more information.
*/
//------------------------------------------
inline bool SUPPORT::isOnAllElements() const
//------------------------------------------
{
  return _isOnAllElts;
}

/*!
  Returns number of geometric Types
  defines in the support
*/
//------------------------------------------
inline int SUPPORT::getNumberOfTypes() const
//------------------------------------------
{
  //    if ((_isOnAllElts)&(_entity != MED_NODE))
  //      return _mesh->getNumberOfTypes(_entity) ;
  //    else
  return _numberOfGeometricType ;
}

/*!
  Returns the %medEntityMesh's type used by the support.
  Note : A support deals only with one entity's type
  (for example : MED_FACE or MED_NODE)
*/
//---------------------------------------------
inline MED_EN::medEntityMesh SUPPORT::getEntity() const
//---------------------------------------------
{
  return _entity;
}

/*!
  If isOnAllElements is false, returns an array of %medGeometryElement
  types used by the support.
*/
//---------------------------------------------------
inline const MED_EN::medGeometryElement * SUPPORT::getTypes() const
//---------------------------------------------------
{
  return _geometricType;
}

}//End namespace MEDMEM


#endif /* SUPPORT_HXX */
