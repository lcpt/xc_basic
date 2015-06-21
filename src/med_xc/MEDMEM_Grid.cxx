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
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_DriverFactory.hxx"

#include <memory>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;


// Block defining the comments for the MEDMEM_ug documentation

/*! 

\defgroup GRID_axes Information about axes
This group of methods retrieves information about the axes of the grid.

\defgroup GRID_connectivity Utility methods for defining element positions in the grid
These methods enable the user to convert a position on the grid to a global element number

\defgroup GRID_constructors Constructors
These methods are the different constructors for the grid objects.

*/

namespace
{
  const string* defaultStrings()
  {
    static const string str[3] = { "UNDEFINED", "UNDEFINED", "UNDEFINED" };
    return str;
  }
}

//=======================================================================
//function : GRID
//purpose  : empty constructor
//=======================================================================

GRID::GRID() {
  init();
  MESSAGE_MED("A GRID CREATED");
}
//
//=======================================================================
////function : GRID
////purpose  : array constructor
////=======================================================================
/*!
\if MEDMEM_ug
\addtogroup GRID_constructors
@{
\endif
*/
/*!
 * \brief Constructor specifying the axes of the grid
 *
 * This constructor describes the grid by specifying the location of the nodes on 
each of the axis. The dimension of the grid is implicitly defined by the
size of vector \a xyz_array.
 *
 *\param xyz_array specifies the node coordinates for each direction 
 *\param coord_name names of the different coordinates
 *\param coord_unit names of the different coordinate units
 *\param type  grid type (MED_POLAR, MED_CARTESIAN)
*/
GRID::GRID(const std::vector<std::vector<double> >& xyz_array,
           const std::vector<std::string>&          coord_name,
           const std::vector<std::string>&          coord_unit,
           const MED_EN::med_grid_type              type)
  :_gridType(type)
{
    init(); // PAL 12136
    _is_default_gridType = false;

    _spaceDimension = xyz_array.size();

    // create a non allocated COORDINATE
    _coordinate = new COORDINATE(_spaceDimension, &coord_name[0], &coord_unit[0]);
    string coordinateSystem = "UNDEFINED";
    if( _gridType == MED_EN::MED_CARTESIAN) 
      coordinateSystem = "CARTESIAN";
    else if ( _gridType == MED_POLAR) 
      coordinateSystem = "CYLINDRICAL";
    _coordinate->setCoordinatesSystem(coordinateSystem);

    // set the GRID part
    if (_spaceDimension>=1)
    {
        _iArrayLength=xyz_array[0].size();
        _iArray=new double[_iArrayLength];
        std::copy(xyz_array[0].begin(),xyz_array[0].end(),_iArray);
    }
    if (_spaceDimension>=2)
    {
        _jArrayLength=xyz_array[1].size();
        _jArray=new double[_jArrayLength];
        std::copy(xyz_array[1].begin(),xyz_array[1].end(),_jArray);
    }
    if (_spaceDimension>=3)
    {
        _kArrayLength=xyz_array[2].size();
        _kArray=new double[_kArrayLength];
        std::copy(xyz_array[2].begin(),xyz_array[2].end(),_kArray);
    }
}

//================================================================================
/*!
 * \brief Reads a GRID form the file
 *  \param driverType - type of driver to read the specified file
 *  \param fileName - the file name to read
 *  \param driverName - name of a grid to read
 */
//================================================================================

GRID::GRID(driverTypes driverType, const string &  fileName, const string &  driverName)
{
  
  const char* LOC = "GRID::GRID(driverTypes , const string &  , const string &) : ";
  BEGIN_OF_MED(LOC);
  
  init();
  unique_ptr<GENDRIVER> myDriver (DRIVERFACTORY::buildDriverForMesh(driverType,fileName,this,driverName,RDONLY));
  myDriver->open();
  myDriver->read();
  myDriver->close();

  END_OF_MED(LOC);
}

/*!\if MEDMEM_ug @} \endif */

//=======================================================================
//function : GRID
//purpose  : empty constructor
//=======================================================================

GRID::GRID(const MED_EN::med_grid_type type)
{
  init();
  _gridType = type;
  _is_default_gridType = false;
  MESSAGE_MED("A TYPED GRID CREATED");
}

//=======================================================================
//function : GRID
//purpose  : copying constructor
//=======================================================================

GRID::GRID(const GRID& otherGrid) {
  *this = otherGrid;
}

//=======================================================================
//function : ~GRID
//purpose  : destructor
//=======================================================================

GRID::~GRID() {
  MESSAGE_MED("GRID::~GRID() : Destroying the Grid");
  if ( _coordinate ) delete _coordinate; _coordinate = 0;
  if ( _iArray != (double* ) NULL) delete [] _iArray;
  if ( _jArray != (double* ) NULL) delete [] _jArray;
  if ( _kArray != (double* ) NULL) delete [] _kArray;
}

//=======================================================================
//function : init
//purpose : 
//=======================================================================

void GRID::init()
{
  GMESH::init();

  _gridType = MED_EN::MED_CARTESIAN;
  _is_default_gridType = true;
  _coordinate = 0;
  _iArray = _jArray = _kArray = (double* ) NULL;
  _iArrayLength = _jArrayLength = _kArrayLength = 0;

}

//================================================================================
/*!
 * \brief Return true if contains no elements
 */
//================================================================================

bool GRID::isEmpty() const
{
  return !_iArrayLength && !_coordinate;
}


//=======================================================================
//function: operator=
//purpose : operator=
//=======================================================================

GRID & GRID::operator=(const GRID & otherGrid)
{
  // NOT IMPLEMENTED

  GMESH::operator=(otherGrid);
  return *this;
}

//=======================================================================
/*!
  Returns true if mesh \a other has same
  coordinates (to 1E-15 precision ) and same connectivity as the calling object.
  Information like name or description is not taken into account
  for the comparison.
*/
//=======================================================================

bool GRID::deepCompare(const GMESH& gother) const
{
  if ( gother.getIsAGrid() != getIsAGrid())
    return false;
  const GRID& other = static_cast<const GRID&>( gother );

  if ( getSpaceDimension() != other.getSpaceDimension() )
    return false;

  if ( _gridType != other._gridType )
    return false;

  if( bool( _coordinate) != bool(other._coordinate))
    return false;

  if ( _coordinate->getNumberOfNodes() > 0 )
  {
    if ( _coordinate->getNumberOfNodes() != other._coordinate->getNumberOfNodes() )
      return false;
    const int size = _coordinate->getNumberOfNodes() * getSpaceDimension();
    const double* coord1=_coordinate->getCoordinates(MED_EN::MED_FULL_INTERLACE);
    const double* coord2=other._coordinate->getCoordinates(MED_EN::MED_FULL_INTERLACE);
    for(int i = 0 ; i < size; i++ )
      if ( !(fabs(coord1[i]-coord2[i])<1e-15))
        return false;
  }

  if ( _iArrayLength != other._iArrayLength )
    return false;
  if ( _jArrayLength != other._jArrayLength )
    return false;
  if ( _kArrayLength != other._kArrayLength )
    return false;

  if ( bool(_iArray) != bool(other._iArray) )
    return false;
  if ( bool(_jArray) != bool(other._jArray) )
    return false;
  if ( bool(_kArray) != bool(other._kArray) )
    return false;

  if ( _iArray )
    for ( int i = 0; i < _iArrayLength; ++i )
      if ( !(fabs(_iArray[i]-other._iArray[i])<1e-15))
        return false;
  if ( _jArray )
    for ( int i = 0; i < _jArrayLength; ++i )
      if ( !(fabs(_jArray[i]-other._jArray[i])<1e-15))
        return false;
  if ( _kArray )
    for ( int i = 0; i < _kArrayLength; ++i )
      if ( !(fabs(_kArray[i]-other._kArray[i])<1e-15))
        return false;

  return true;
}

//================================================================================
/*!
 * \brief print my contents
 */
//================================================================================

void GRID::printMySelf(std::ostream &os) const
{
  // TODO
  cerr << "NOT IMPLEMENTED" << endl;
}

//================================================================================
/*!
 * \brief Create an unstructured MESH. Call removeReference() after having finished using it!!!
 */
//================================================================================

const MESH * GRID::convertInMESH() const
{
  MESHING* mesh = new MESHING;
  mesh->setName( getName() );

  int i, j, k;

  // ---------------
  // 1. Coordinates
  // ---------------

  PointerOf< double > coords;
  if ( _gridType == MED_BODY_FITTED )
  {
    if ( !_coordinate )
      throw MEDEXCEPTION
        (LOCALIZED("GRID::convertInMESH() : _coordinate of MED_BODY_FITTED GRID not defined !"));
    coords.set( _coordinate->getCoordinates(MED_EN::MED_FULL_INTERLACE));
  }
  else
  {
    coords.set( getSpaceDimension() * getNumberOfNodes() );
    double* coord = coords;
    switch ( getSpaceDimension() )
    {
    case 3:
      for (k=0; k < _kArrayLength; ++k) 
        for (j=0; j < _jArrayLength; ++j)
          for (i=0; i < _iArrayLength; ++i)
            *coord++ = _iArray[i], *coord++ = _jArray[j], *coord++ = _kArray[k];
      break;

    case 2:
      for (j=0; j < _jArrayLength; ++j)
        for (i=0; i < _iArrayLength; ++i)
          *coord++ = _iArray[i], *coord++ = _jArray[j];
      break;

    case 1:
      coords.set(_iArray);
      break;
    }
  }
  mesh->setCoordinates( getSpaceDimension(),
                        getNumberOfNodes(),
                        (const double *) coords,
                        getCoordinatesSystem(),
                        MED_EN::MED_FULL_INTERLACE);
  mesh->setCoordinatesNames( getCoordinatesNames() );
  mesh->setCoordinatesUnits( getCoordinatesUnits() );

  // ----------------
  // 2. Connectivity
  // ----------------

  // 2.1 Cells
  // ---------
  medEntityMesh subEntity= MED_ALL_ENTITIES;
  {
    mesh->setNumberOfTypes( getNumberOfTypes(MED_EN::MED_CELL), MED_EN::MED_CELL );
    mesh->setTypes( getTypes( MED_EN::MED_CELL), MED_EN::MED_CELL );
    int nbCells = getNumberOfElements( MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS );
    mesh->setNumberOfElements( &nbCells, MED_EN::MED_CELL );

    vector<int> conn;
    switch ( _spaceDimension )
    {
    case 3: // HEXA8
      for ( k = 0; k < _kArrayLength-1; ++k )
        for ( j = 0; j < _jArrayLength-1; ++j )
          for ( i = 0; i < _iArrayLength-1; ++i )
          {
            conn.push_back( getNodeNumber( i  , j  , k  ));
            conn.push_back( getNodeNumber( i  , j+1, k  ));
            conn.push_back( getNodeNumber( i+1, j+1, k  ));
            conn.push_back( getNodeNumber( i+1, j  , k  ));
            conn.push_back( getNodeNumber( i  , j  , k+1));
            conn.push_back( getNodeNumber( i  , j+1, k+1));
            conn.push_back( getNodeNumber( i+1, j+1, k+1));
            conn.push_back( getNodeNumber( i+1, j  , k+1));
          }
      subEntity = MED_EN::MED_FACE;
      break;

    case 2: // QUAD4
      for ( j = 0; j < _jArrayLength-1; ++j )
        for ( i = 0; i < _iArrayLength-1; ++i )
        {
          int n1 = 1 + i + j*_iArrayLength;
          conn.push_back( n1 );
          conn.push_back( n1 + _iArrayLength );
          conn.push_back( n1 + _iArrayLength + 1 );
          conn.push_back( n1 + 1 );
        }
      subEntity = MED_EN::MED_EDGE;
      break;

    case 1: // SEG2
      for ( i = 0; i < _iArrayLength-1; ++i )
      {
        conn.push_back( i + 1 );
        conn.push_back( i + 2 );
      }
      break;
    }
    mesh->setConnectivity( MED_EN::MED_CELL, getTypes(MED_EN::MED_CELL)[0], &conn[0] );
  }

  // 2.2 sub entities
  // -----------------

  if ( _spaceDimension > 1 )
  {
    mesh->setNumberOfTypes( getNumberOfTypes(subEntity), subEntity );
    mesh->setTypes( getTypes( subEntity), subEntity );
    int nbCells = getNumberOfElements( subEntity, MED_ALL_ELEMENTS );
    mesh->setNumberOfElements( &nbCells, subEntity );

    vector<int> conn;
    switch ( _spaceDimension )
    {
    case 3: // QUAD4

      // normal to OX
      for ( k = 0; k < _kArrayLength-1; ++k )
        for ( j = 0; j < _jArrayLength-1; ++j )
          for ( i = 0; i < _iArrayLength; ++i )
          {
            conn.push_back( getNodeNumber( i, j  , k  ));
            conn.push_back( getNodeNumber( i, j  , k+1));
            conn.push_back( getNodeNumber( i, j+1, k+1));
            conn.push_back( getNodeNumber( i, j+1, k  ));
          }
      // normal to OY
      for ( k = 0; k < _kArrayLength-1; ++k )
        for ( j = 0; j < _jArrayLength; ++j )
          for ( i = 0; i < _iArrayLength-1; ++i )
          {
            conn.push_back( getNodeNumber( i  , j, k  ));
            conn.push_back( getNodeNumber( i+1, j, k  ));
            conn.push_back( getNodeNumber( i+1, j, k+1));
            conn.push_back( getNodeNumber( i  , j, k+1));
          }
      // normal to OZ
      for ( k = 0; k < _kArrayLength; ++k )
        for ( j = 0; j < _jArrayLength-1; ++j )
          for ( i = 0; i < _iArrayLength-1; ++i )
          {
            conn.push_back( getNodeNumber( i  , j  , k));
            conn.push_back( getNodeNumber( i  , j+1, k));
            conn.push_back( getNodeNumber( i+1, j+1, k));
            conn.push_back( getNodeNumber( i+1, j  , k));
          }
      break;

    case 2: // SEG2

      // || OX
      for ( j = 0; j < _jArrayLength; ++j )
        for ( i = 0; i < _iArrayLength-1; ++i )
        {
          int n1 = 1 + i + j*_iArrayLength;
          conn.push_back( n1 );
          conn.push_back( n1 + 1 );
        }
      // || OY
      for ( j = 0; j < _jArrayLength-1; ++j )
        for ( i = 0; i < _iArrayLength; ++i )
        {
          int n1 = 1 + i + j*_iArrayLength;
          conn.push_back( n1 );
          conn.push_back( n1 + _iArrayLength );
        }
      break;

    }
    mesh->setConnectivity( subEntity, getTypes(subEntity)[0], &conn[0] );
  }

  // 3. Groups and Families
  // -----------------------

  const vector<GROUP*>* groups[] = { &_groupNode, &_groupCell, &_groupFace, &_groupEdge };
  for ( i = 0; i < 4; ++i )
    for ( j = 0; j < (int)groups[i]->size(); ++j )
      mesh->addGroup( * groups[i]->at( j ));

  return mesh;
}

//=======================================================================
/*!
  return the GRID Geometric type, without computing all connectivity
*/
//=======================================================================

const medGeometryElement * GRID::getTypes(MED_EN::medEntityMesh entity) const
{
  static const medGeometryElement _gridGeometry[4]={MED_EN::MED_HEXA8,MED_EN::MED_QUAD4,MED_EN::MED_SEG2,MED_EN::MED_POINT1};
  int i=0;
  if(entity==MED_EN::MED_CELL)
  {
    i=3-_spaceDimension;
  }
  else if(entity==MED_EN::MED_FACE && _spaceDimension>2 )
    i=1;
  else if(entity==MED_EN::MED_EDGE && _spaceDimension>1 )
    i=2;
  else if(entity==MED_EN::MED_NODE && _spaceDimension>0)
    i=3;
  else
    throw MEDEXCEPTION(LOCALIZED("CONNECTIVITY::getGeometricTypes : Entity not defined !"));
  return &_gridGeometry[i];
}

//=======================================================================
//function : getArrayLength
//purpose  : return array length. Axis = [1,2,3] meaning [i,j,k],
//=======================================================================
/*!\if MEDMEM_ug
\addtogroup GRID_axes
@{
\endif
*/
/*! Returns the number of nodes on axis number \a Axis (axis numbering starts at 1).
*/
int GRID::getArrayLength( const int Axis ) const throw (MEDEXCEPTION)
{
  switch (Axis) {
  case 1: return _iArrayLength;
  case 2: return _jArrayLength;
  case 3: return _kArrayLength;
  default:
    throw MED_EXCEPTION ( LOCALIZED( STRING("GRID::getArrayLength ( ") << Axis << ")"));
  }
  return 0;
}

//=======================================================================
//function : getArrayValue
//purpose  : return i-th array component. Axis = [1,2,3] meaning [i,j,k],
//           exception if Axis out of [1-3] range
//           exception if i is out of range 0 <= i < getArrayLength(Axis);
//=======================================================================
/*!
Returns the value of node coordinate \a i on axis \a Axis.
 */
const double GRID::getArrayValue (const int Axis, const int i) const throw (MEDEXCEPTION)
{
  if (i < 0 || i >= getArrayLength(Axis))
    throw MED_EXCEPTION
      ( LOCALIZED(STRING("GRID::getArrayValue ( ") << Axis << ", " << i << ")"));
  
  switch (Axis) {
  case 1:  return _iArray[ i ];
  case 2:  return _jArray[ i ];
  default: return _kArray[ i ];
  }
  return 0.0;
}

/*!
\if MEDMEM_ug
@}
\endif
 */

/*!
\if MEDMEM_ug
\addtogroup GRID_connectivity
@{
\endif
*/
/*!
@{
\name Position to number conversion methods
\a getXXXNumber methods enable the user to convert an \f$ (i,j,k)\f$ position into a global number in the array.

Axis [1,2,3] means one of directions: along \a i, \a j or \a k.
For cell constituents (FACE or EDGE), Axis selects one of those having same  \f$ (i, j, k )\f$ :
- a FACE which is normal to direction along given \a Axis;
- an EDGE going along given \a Axis.
 \a i, \a j and \a k counts from zero.

Exception for \a Axis out of range.
For 2D grids, \a k is a dummy argument. */

//================================================================================
/*! Edge position to number conversion method*/
//================================================================================

int GRID::getEdgeNumber(const int Axis, const int i, const int j, const int k)
  const throw (MEDEXCEPTION)
{
  const char * LOC = "GRID::getEdgeNumber(Axis, i,j,k) :";

  BEGIN_OF_MED(LOC);

  int Len[4] = {0,_iArrayLength, _jArrayLength, _kArrayLength }, I=1, J=2, K=3;
  int maxAxis = Len[ K ] ? 3 : 2;
  
  if (Axis <= 0 || Axis > maxAxis)
    throw MED_EXCEPTION ( LOCALIZED(STRING(LOC) << "Axis out of range: " << Axis));

  Len[Axis]--;
  int Nb = 1 + i + j*Len[ I ] + k*Len[ J ]*Len[ K ];
  Len[Axis]++ ;
  
  if (!Len[ K ])
    Len[ K ] = 1; 

  if (Axis > 1) { // add all edges in i direction
    Len[I]-- ;
    Nb += Len[ I ]*Len[ J ]*Len[ K ];
    Len[I]++ ;
  }
  if (Axis > 2) { // add all edges in j direction
    Len[J]-- ;
    Nb += Len[ I ]*Len[ J ]*Len[ K ];
  }

  END_OF_MED(LOC);

  return Nb;
}

//================================================================================
/*!
Returns a NODE, EDGE, FACE, CELL number by its position in the grid.
Axis [1,2,3] means one of directions: along i, j or k
For Cell contituents (FACE or EDGE), Axis selects one of those having same (i,j,k):
- a FACE which is normal to direction along given Axis;
- an EDGE going along given Axis.
Exception for Axis out of range
*/
//================================================================================

int GRID::getFaceNumber(const int Axis, const int i, const int j, const int k)
  const throw (MEDEXCEPTION)
{
  const char * LOC = "GRID::getFaceNumber(Axis, i,j,k) :";
  
  BEGIN_OF_MED(LOC);

//  if (Axis <= 0 || Axis > 3)
  if (Axis < 0 || Axis > 3)
    throw MED_EXCEPTION ( LOCALIZED(STRING(LOC) << "Axis = " << Axis));

  int Len[4] = {0,_iArrayLength-1, _jArrayLength-1, _kArrayLength-1 }, I=1, J=2, K=3;

  Len[Axis]++;
  int Nb = 1 + i + j*Len[ I ] + k*Len[ I ]*Len[ J ];
  Len[Axis]--;

  if (Axis > 1) // add all faces nornal to i direction
    Nb += ( Len[ I ]+1 )*Len[ J ]*Len[ K ];

  if (Axis > 2) // add all faces nornal to j direction
    Nb += Len[ I ]*( Len[ J ]+1 )*Len[ K ];

  END_OF_MED(LOC);

  return Nb;
}
/*! @} */


/*!
@{
\name Number to position conversion methods

\a getXXXPosition functions enable the user to convert
a number into a \f$ (i,j,k) \f$ position. 
           Axis [1,2,3] means one of directions: along i, j or k
           For Cell constituents (FACE or EDGE), Axis selects one of those having same (i,j,k):
           - a FACE which is normal to direction along given Axis;
           - an EDGE going along given Axis.

    Exception for Number out of range.
*/

//================================================================================
/*! Node number to position conversion method */
//================================================================================

void GRID::getNodePosition(const int Number, int& i, int& j, int& k) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "GRID::getNodePosition(Number, i,j,k) :";
  
  BEGIN_OF_MED(LOC);

  if (Number <= 0 || Number > getNumberOfNodes() )
    throw MED_EXCEPTION ( LOCALIZED(STRING(LOC) << "Number is out of range: " << Number));

  int Len[] = {_iArrayLength, _jArrayLength, _kArrayLength }, I=0, J=1;
  // , K=2; !! UNUSED VARIABLE !!

  int ijLen = Len[I] * Len[J]; // nb in a full k layer
  int kLen = (Number - 1) % ijLen;    // nb in the non full k layer
  
  i = kLen % Len[J];
  j = kLen / Len[J];
  k = (Number - 1) / ijLen;

  ////cout <<" NODE POS: " << Number << " - " << i << ", " << j << ", " << k << endl;

  END_OF_MED(LOC);

}

//=======================================================================
//function : getCellPosition
//purpose  : 
//=======================================================================
/*! Cell number to position conversion method */
void GRID::getCellPosition(const int Number, int& i, int& j, int& k) const
  throw (MEDEXCEPTION)
{
  
  const char* LOC = "GRID::getCellPosition(Number, i,j,k) :";
  BEGIN_OF_MED(LOC);

  int Len[4] = {0,_iArrayLength-1, _jArrayLength-1, _kArrayLength-1 }, I=1, J=2;
  // , K=3; !! UNUSED VARIABLE !!

//  if (Number <= 0 || Number > getCellNumber(Len[I]-1, Len[J]-1, Len[K]-1))
//    throw MED_EXCEPTION ( LOCALIZED(STRING(LOC) << "Number is out of range: " << Number));

  int ijLen = Len[I] * Len[J]; // nb in a full k layer
  int kLen = (Number - 1) % ijLen;    // nb in the non full k layer
  
  i = kLen % Len[J];
  j = kLen / Len[J];
  k = (Number - 1) / ijLen;

  END_OF_MED(LOC);
}

//=======================================================================
//function : getEdgePosition
//purpose  : 
//=======================================================================
/*! Edge number to poistion conversion method*/
void GRID::getEdgePosition(const int Number, int& Axis, int& i, int& j, int& k)
  const throw (MEDEXCEPTION)
{
  const char * LOC = "GRID::getEdgePosition(Number, i,j,k) :";

  BEGIN_OF_MED(LOC);

  if (!_jArrayLength)
    throw MED_EXCEPTION ( LOCALIZED(STRING(LOC) << "no edges in the grid: "));
  
  if (Number <= 0)
    throw MED_EXCEPTION ( LOCALIZED(STRING(LOC) << "Number is out of range: " << Number));

  int Len[4] = {0,_iArrayLength, _jArrayLength, _kArrayLength }, I=1, J=2, K=3;

  int theNb = Number;
  int maxAxis = _kArrayLength ? 3 : 2;
  
  for (Axis = 1; Axis <= maxAxis; ++Axis)
  {
    Len[Axis]--;  // one less edge in Axis direction

    // max edge number in Axis direction
    int maxNb = getEdgeNumber (Axis, Len[I]-1, Len[J]-1, Len[K]-1);
    
    if (theNb > maxNb)
    {
      Len[Axis]++;
      theNb -= maxNb;
      continue;
    }
    
    if (theNb == maxNb)
    {
      i = Len[I]-1;
      j = Len[J]-1;
      k = Len[K]-1;
    }
    else
    {
      int ijLen = Len[I] * Len[J]; // nb in a full k layer
      int kLen = (theNb - 1) % ijLen;    // nb in the non full k layer
      i = kLen % Len[J];
      j = kLen / Len[J];
      k = (theNb - 1) / ijLen;
    }

  END_OF_MED(LOC);

    return;
  }
  
  throw MED_EXCEPTION ( LOCALIZED(STRING(LOC) << "Number is out of range: " << Number));
}

//=======================================================================
//function : getFacePosition
//purpose  : return position (i,j,k) of an entity #Number
//           Axis [1,2,3] means one of directions: along i, j or k
//           For Cell contituents (FACE or EDGE), Axis selects one of those having same (i,j,k):
//           * a FACE which is normal to direction along given Axis;
//           * an EDGE going along given Axis.
//           Exception for Number out of range
//=======================================================================
/*! Face number to position convertion method*/
void GRID::getFacePosition(const int Number, int& Axis, int& i, int& j, int& k)
  const throw (MEDEXCEPTION)
{
  const char * LOC = "GRID::getFacePosition(Number, i,j,k) :";

  BEGIN_OF_MED(LOC);

  if (_kArrayLength == 0) {
    getCellPosition(Number, i, j, k);
    Axis = 1;
    return;
  };

  if (!_kArrayLength)
    throw MED_EXCEPTION ( LOCALIZED(STRING(LOC) << "no faces in the grid: "));
  
  if ( Number <= 0 )
    throw MED_EXCEPTION ( LOCALIZED(STRING(LOC) << "Number is out of range: " << Number));

  int Len[4] = {0,_iArrayLength-1, _jArrayLength-1, _kArrayLength-1 }, I=1, J=2, K=3;
  int theNb = Number;
  
  for (Axis = 1; Axis <= 3; ++Axis)
  {
    Len[Axis]++;

    // max face number in Axis direction
    int maxNb = getFaceNumber (Axis, Len[I]-1, Len[J]-1, Len[K]-1);
    
    if (theNb > maxNb)
    {
      Len[Axis]--;
      theNb -= maxNb;
      continue;
    }
    
    if (theNb == maxNb)
    {
      i = Len[I]-1;
      j = Len[J]-1;
      k = Len[K]-1;
    }
    else
    {
      int ijLen = Len[I] * Len[J]; // nb in a full k layer
      int kLen = (theNb - 1)  % ijLen;    // nb in the non full k layer
      i = kLen % Len[J];
      j = kLen / Len[J];
      k = (theNb - 1) / ijLen;
    }

  END_OF_MED(LOC);

    return;
  }
  
  throw MED_EXCEPTION ( LOCALIZED(STRING(LOC) << "Number is out of range: " << Number));
}
/*!
@}
\if MEDMEM_ug
@}
\endif
*/

//================================================================================
/*! Get the number of different geometric types for a given entity type.

    medEntityMesh entity : MED_EN::MED_CELL, MED_EN::MED_FACE, MED_EN::MED_EDGE, MED_EN::MED_NODE,
    MED_ALL_ENTITIES

*/
//================================================================================

int GRID::getNumberOfTypes(MED_EN::medEntityMesh entity) const
{
  MESSAGE_MED("GRID::getNumberOfTypes(medEntityMesh entity) : "<<entity);
  return 1; // a grid has one type
}

//================================================================================
/*!
  Return the number of element of given geometric type of given entity. Return 0 if query is not defined.
*/
//================================================================================

int GRID::getNumberOfElements(MED_EN::medEntityMesh entity, MED_EN::medGeometryElement Type) const
{
  int numberOfElements=0;

  // Cas où le nombre d'éléments n'est pas nul
  if (entity==MED_EN::MED_FACE && (Type==MED_EN::MED_QUAD4 || Type==MED_EN::MED_ALL_ELEMENTS) && getMeshDimension()>2)
    numberOfElements=
      (_iArrayLength-1)*(_jArrayLength-1)*(_kArrayLength  )+
      (_iArrayLength-1)*(_jArrayLength  )*(_kArrayLength-1)+
      (_iArrayLength  )*(_jArrayLength-1)*(_kArrayLength-1);

  else if (entity==MED_EN::MED_EDGE && (Type==MED_EN::MED_SEG2 || Type==MED_EN::MED_ALL_ELEMENTS))
    if ( _spaceDimension==2)
      numberOfElements=_iArrayLength*(_jArrayLength-1) + (_iArrayLength-1)*_jArrayLength;
    else if ( _spaceDimension==1)
      numberOfElements=_iArrayLength-1;
    else // 3D
      numberOfElements=
        (_iArrayLength*(_jArrayLength-1) + (_iArrayLength-1)*_jArrayLength) * _kArrayLength +
        _iArrayLength*_jArrayLength*(_kArrayLength-1);

  else if (entity==MED_EN::MED_NODE && (Type==MED_EN::MED_NONE || Type==MED_EN::MED_ALL_ELEMENTS) && _spaceDimension>0)
    numberOfElements=getNumberOfNodes();

  else if (entity==MED_EN::MED_CELL && _spaceDimension==3 && (Type==MED_EN::MED_HEXA8 || Type==MED_EN::MED_ALL_ELEMENTS) )
    numberOfElements=(_iArrayLength-1)*(_jArrayLength-1)*(_kArrayLength-1);

  else if (entity==MED_EN::MED_CELL && _spaceDimension==2 && (Type==MED_EN::MED_QUAD4 || Type==MED_EN::MED_ALL_ELEMENTS))
    numberOfElements=(_iArrayLength-1)*(_jArrayLength-1);

  else if (entity==MED_EN::MED_CELL && _spaceDimension==1 && (Type==MED_EN::MED_SEG2 || Type==MED_EN::MED_ALL_ELEMENTS) )
    numberOfElements=_iArrayLength-1;

  MESSAGE_MED("GRID::getNumberOfElements - entity=" << entity << " Type=" << Type);
  MESSAGE_MED("_spaceDimension=" << _spaceDimension << "  numberOfElements=" << numberOfElements);

  return numberOfElements;
}

//================================================================================
/*!
  Return the geometric type of global element Number of entity Entity.
*/
//================================================================================

MED_EN::medGeometryElement GRID::getElementType(MED_EN::medEntityMesh Entity,int Number) const
{
  return getTypes(Entity)[0];
}

//================================================================================
/*!
 * \brief Return mesh dimension
 */
//================================================================================

int GRID::getMeshDimension() const
{
  return getSpaceDimension();
}

//================================================================================
/*!
 * \brief It is a grid
 */
//================================================================================

bool GRID::getIsAGrid() const
{
  return true;
}

//================================================================================
/*!
 * \brief Return number of nodes
 */
//================================================================================

int GRID::getNumberOfNodes() const
{
  if ( _gridType == MED_EN::MED_BODY_FITTED )
    return _coordinate ? _coordinate->getNumberOfNodes() : 0;

  switch ( _spaceDimension )
  {
  case 3: return _iArrayLength * _jArrayLength * _kArrayLength;
  case 2: return _iArrayLength * _jArrayLength;
  case 1: return _iArrayLength;
  }
  return 0;
}

//=======================================================================
/*!
 * Returns "CARTESIAN", "CYLINDRICAL" or "SPHERICAL"
 */
//=======================================================================

std::string         GRID::getCoordinatesSystem() const
{
  return _coordinate ? _coordinate->getCoordinatesSystem() : defaultStrings()[0];
}

//=======================================================================
/*!
 * Returns an array with names of coordinates. \n
 *     Example : \n
 *     - x,y,z \n
 *     - r,teta,phi \n
 *     - ...
 */
//=======================================================================

const std::string * GRID::getCoordinatesNames() const
{
  return _coordinate ? _coordinate->getCoordinatesNames() : defaultStrings();
}

//=======================================================================
/*!
 * Returns an array with units of coordinates (cm, m, mm, ...)
 * It could be empty. We suppose we are IS (meter).
 */
//=======================================================================

const std::string * GRID::getCoordinatesUnits() const
{
  return _coordinate ? _coordinate->getCoordinatesUnits() : defaultStrings();
}

//=======================================================================
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
//=======================================================================

SUPPORT * GRID::getBoundaryElements(MED_EN::medEntityMesh Entity) const throw (MEDEXCEPTION)
{
  const char * LOC = "GMESH::getBoundaryElements() : " ;
  if ( _spaceDimension < 2 )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not implemented in " << _spaceDimension <<"D space !"));

  if ( _gridType == MED_POLAR) 
    throw MEDEXCEPTION("GRID::getBoundaryElements() : not implemented on MED_POLAR grig");


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
  list<int> bnd_elems1, bnd_elems2;
  int numberOf = getNumberOfElements(entityToParse,MED_ALL_ELEMENTS) ;

  if ( _coordinate->getNumberOfNodes() > 0 ) // BODY FITTED
    {
      throw MEDEXCEPTION("GRID::getBoundaryElements() : not implemented on BOBY FITTED grig");
    }
  else if ( entityToParse == MED_EN::MED_FACE ) // 3D CARTESIAN
    {
      const int nb_x = getArrayLength( 1 ) - 1;
      const int nb_y = getArrayLength( 2 ) - 1;
      const int nb_z = getArrayLength( 3 ) - 1;
      // normal to OX
      for ( int z = 0; z < nb_z; ++z )
        for ( int y = 0; y < nb_y; ++y )
          {
            bnd_elems1.push_back( getFaceNumber( 1, 0,    y, z ));
            bnd_elems2.push_back( getFaceNumber( 1, nb_x, y, z ));
          }
      bnd_elems1.splice( bnd_elems1.end(), bnd_elems2 ); // to have ids in increasing order

      // normal to OY
      for ( int z = 0; z < nb_z; ++z )
        for ( int x = 0; x < nb_x; ++x )
          {
            bnd_elems1.push_back( getFaceNumber( 2, x, 0,    z ));
            bnd_elems2.push_back( getFaceNumber( 2, x, nb_y, z ));
          }
      bnd_elems1.splice( bnd_elems1.end(), bnd_elems2 );

      // normal to OZ
      for ( int y = 0; y < nb_y; ++y )
        for ( int x = 0; x < nb_x; ++x )
          {
            bnd_elems1.push_back( getFaceNumber( 3, x, y, 0    ));
            bnd_elems2.push_back( getFaceNumber( 3, x, y, nb_z ));
          }
      bnd_elems1.splice( bnd_elems1.end(), bnd_elems2 );
    }
  else
    {
      const int nb_x = getArrayLength( 1 ) - 1;
      const int nb_y = getArrayLength( 2 ) - 1;
      // edge || OX
      for ( int x = 0; x < nb_x; ++x )
        {
          bnd_elems1.push_back( getEdgeNumber( 1, x, 0 ));
          bnd_elems2.push_back( getEdgeNumber( 1, x, nb_y ));
        }
      bnd_elems1.splice( bnd_elems1.end(), bnd_elems2 ); // to have ids in increasing order
      // edge || OY
      for ( int y = 0; y < nb_y; ++y )
        {
          bnd_elems1.push_back( getEdgeNumber( 2, y, 0 ));
          bnd_elems2.push_back( getEdgeNumber( 2, y, nb_x ));
        }
      bnd_elems1.splice( bnd_elems1.end(), bnd_elems2 );
    }

  if ( bnd_elems1.empty() && numberOf != 0 )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No boundary elements found by reverse descending connectivity for entity "<<Entity<<" !"));

  if ( Entity == MED_EN::MED_NODE )
    return buildSupportOnNodeFromElementList(bnd_elems1,entityToParse);
  else
    return buildSupportOnElementsFromElementList(bnd_elems1,entityToParse);
}

SUPPORT * GRID::getSkin(const SUPPORT * Support3D) throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GRID::getSkin() : Not implemented yet");
}
SUPPORT *GRID::buildSupportOnNodeFromElementList(const std::list<int>& listOfElt,
                                                 MED_EN::medEntityMesh entity) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GRID::buildSupportOnNodeFromElementList() : Not implemented yet");
}
void GRID::fillSupportOnNodeFromElementList(const std::list<int>& listOfElt,
                                            SUPPORT *             supportToFill) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GRID::fillSupportOnNodeFromElementList() : Not implemented yet");
}

FIELD<double>* GRID::getVolume (const SUPPORT * Support, bool isAbs) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GRID::getVolume() : Not implemented yet");
}

FIELD<double>* GRID::getArea (const SUPPORT * Support) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GRID::getArea() : Not implemented yet");
}

FIELD<double>* GRID::getLength (const SUPPORT * Support) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GRID::getLength() : Not implemented yet");
}

FIELD<double>* GRID::getNormal (const SUPPORT * Support) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GRID::getNormal() : Not implemented yet");
}

FIELD<double>* GRID::getBarycenter (const SUPPORT * Support) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("GRID::getBarycenter() : Not implemented yet");
}

vector< vector<double> >   GRID::getBoundingBox() const
{
  throw MEDEXCEPTION("GRID::getBoundingBox() : Not implemented yet");
}
