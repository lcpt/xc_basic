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

// File      : MEDMEM_Grid.hxx
// Created   : Wed Dec 18 08:35:26 2002
// Descr     : class containing structured mesh data
// Author    : Edward AGAPOV (eap)
// Project   : SALOME Pro
// Module    : MED 
//
#ifndef MEDMEM_Grid_HeaderFile
#define MEDMEM_Grid_HeaderFile

#include "MEDMEM.hxx"

#include "MEDMEM_Mesh.hxx"

// class containing structured mesh data

namespace MEDMEM {
class MEDMEM_EXPORT GRID: public GMESH
{
 protected:
  //-----------------------//
  //   Fields
  //-----------------------//
  
  // 1. grid type: MED_CARTESIAN, MED_POLAR, MED_BODY_FITTED
  MED_EN::med_grid_type     _gridType;
  
  // 2. node coordinates for MED_BODY_FITTED and coordinate names, units etc.
  COORDINATE*       _coordinate;

  // 2.1. i component array: X for MED_CARTESIAN, R for MED_POLAR
  int               _iArrayLength;
  double*           _iArray;
  // 2.2. j component array: Y for MED_CARTESIAN, Theta for MED_POLAR
  int               _jArrayLength;
  double*           _jArray;
  // 2.3. k component array: Z for both MED_CARTESIAN and MED_POLAR
  int               _kArrayLength;
  double*           _kArray;

  // 3. is _gridType a true value or just a default one
  bool              _is_default_gridType;

  // Suppose a READ Driver to do the following except filling _[ijk]Array's
  // 1) fill GMESH fields:
  //    * _name
  //    * _spaceDimension
  //    * _numberOfNodesFamilies, _numberOfCellsFamilies, ect
  //    * _familyNode, _familyCell, etc
  //    * _numberOfNodesGroups, _numberOfCellsGroups, etc
  //    * _groupNode, _groupCell, etc
  //    * _
  // 2) create _coordinate without setting _coordinate->_coordinate and set: 
  //    * _coordinate->_coordinateSystem
  //    * _coordinate->_coordinateName
  //    * _coordinate->_coordinateUnit
 protected:
  virtual bool isEmpty() const;


 public:
  //-----------------------//
  //   Public Methods
  //-----------------------//

  GRID();
  GRID(const MED_EN::med_grid_type type);
  GRID(const GRID &m);
  GRID( driverTypes driverType, const string & fileName,const string & meshName);
  GRID(const std::vector<std::vector<double> >& xyz_array,
       const std::vector<std::string>&          coord_name,
       const std::vector<std::string>&          coord_unit,
       const MED_EN::med_grid_type              type=MED_EN::MED_CARTESIAN);
  GRID & operator=(const GRID &m);
  virtual ~GRID();
  virtual void init();
  virtual bool deepCompare(const GMESH& other) const;
  virtual void printMySelf(std::ostream &os) const;

  virtual const MESH *        convertInMESH() const;

  virtual int                 getMeshDimension() const;
  virtual bool                getIsAGrid() const;
  virtual int                 getNumberOfNodes() const;

  virtual std::string         getCoordinatesSystem() const;
  virtual const std::string * getCoordinatesNames() const;
  virtual const std::string * getCoordinatesUnits() const;

  virtual int                 getNumberOfTypes(MED_EN::medEntityMesh Entity) const;
  virtual int                 getNumberOfElements(MED_EN::medEntityMesh      Entity,
                                                  MED_EN::medGeometryElement Type) const;
  virtual const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const;
  virtual MED_EN::medGeometryElement getElementType(MED_EN::medEntityMesh Entity,
                                                    int                   Number) const;

  virtual SUPPORT *                  getBoundaryElements(MED_EN::medEntityMesh Entity) const
                                                throw (MEDEXCEPTION);
  virtual SUPPORT *                  getSkin(const SUPPORT * Support3D)
                                                throw (MEDEXCEPTION);
  virtual SUPPORT *buildSupportOnNodeFromElementList(const std::list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
  virtual void     fillSupportOnNodeFromElementList(const std::list<int>& listOfElt, SUPPORT *supportToFill) const throw (MEDEXCEPTION);

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
  
  virtual vector< vector<double> >   getBoundingBox() const;


  // Specific GRID methods
  //-----------------------//

  inline int getNodeNumber(const int i, const int j=0, const int k=0) const;
  // return a NODE number by its position in the grid.
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is: 0 <= X < getArrayLength( X_Axis )

  inline int getCellNumber(const int i, const int j=0, const int k=0) const ;
  // return a CELL number by its position in the grid.
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is: 0 <= X < (getArrayLength( X_Axis )-1)

  int getEdgeNumber(const int Axis, const int i, const int j=0, const int k=0) const throw (MEDEXCEPTION) ;
  // return an EDGE number by its position in the grid.
  // Axis [1,2,3] means one of directions: along i, j or k.
  // It selects an edge of ones having same (i,j,k):
  //  * an EDGE going along given Axis.
  // Exception for Axis out of range or when there is no edges in the grid (1D)
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is:
  // 0 <= X < (getArrayLength( X_Axis )-1)
  // 0 <= X < getArrayLength( NOT_X_Axis )

  int getFaceNumber(const int Axis, const int i, const int j=0, const int k=0) const throw (MEDEXCEPTION) ;
  // return a FACE number by its position in the grid.
  // Axis [1,2,3] means one of directions: along i, j or k.
  // It selects a face of ones having same (i,j,k):
  //  * a FACE which is normal to given Axis
  // Exception for Axis out of range or when there is no faces in the grid (1,2D)
  // WARNING: be carefull, there is no check that i,j,k are within a good range
  // A good range is:
  // 0 <= X < (getArrayLength( NOT_X_Axis )-1)
  // 0 <= X < getArrayLength( X_Axis )
  

  void getNodePosition(const int Number, int& i, int& j, int& k) const throw (MEDEXCEPTION) ;
  void getCellPosition(const int Number, int& i, int& j, int& k) const throw (MEDEXCEPTION) ;
  void getEdgePosition(const int Number, int& Axis, int& i, int& j, int& k) const throw (MEDEXCEPTION) ;
  void getFacePosition(const int Number, int& Axis, int& i, int& j, int& k) const throw (MEDEXCEPTION) ;
  // return position (i,j,k) of an entity #Number
  // Axis: [1,2,3], see get*Number for details
  // Exception for Number out of range


  //  Access to fields
  //-----------------------//

  inline MED_EN::med_grid_type getGridType() const;
  // return MED_CARTESIAN, MED_POLAR or MED_BODY_FITTED

  int getArrayLength( const int Axis ) const throw (MEDEXCEPTION);
  // return array length. Axis = [1,2,3] meaning [i,j,k],
  // exception if Axis out of [1-3] range

  const double getArrayValue (const int Axis, const int i) const throw (MEDEXCEPTION) ;
  // return i-th array component. Axis = [1,2,3] meaning [i,j,k],
  // exception if Axis out of [1 - 3] range
  // exception if i is out of range [0 - (getArrayLength(Axis)-1)];

  inline void setGridType(MED_EN::med_grid_type gridType);


  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;
  friend class ENSIGHT_MESH_RDONLY_DRIVER;
};


  //----------------------------------//
  //   Inline Methods Implementation
  //----------------------------------//

inline MED_EN::med_grid_type GRID::getGridType() const
{
  return _gridType;
}
//=======================================================================
//function : getNodeNumber
//purpose  : 
//=======================================================================

inline int GRID::getNodeNumber(const int i, const int j, const int k) const
{
  return 1 + i + _iArrayLength * j + _iArrayLength * _jArrayLength * k;
}

//=======================================================================
//function : getCellNumber
//purpose  : 
//=======================================================================

inline int GRID::getCellNumber(const int i, const int j, const int k) const
{
  return 1 + i + (_iArrayLength-1) * j + (_iArrayLength-1) * (_jArrayLength-1) * k;
}

//=======================================================================
//function : setGridType
//purpose : set the _gridType field od the class GRID
//=======================================================================

inline void GRID::setGridType(MED_EN::med_grid_type gridType)
{
  _gridType = gridType;
}

}

#endif
