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

#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_STRING.hxx"

#include "MEDMEM_Utilities.hxx"

#include <cstring>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

/*! Default Constructor : should not be used */
//----------------------------------------------------------//
COORDINATE::COORDINATE():_coordinateSystem(""),
                         _coordinate(MEDARRAY<double>()),
                         _coordinateName(),
                         _coordinateUnit(),
                         _nodeNumber()
//----------------------------------------------------------//
{
  const char* LOC = "Default Constructor COORDINATE";
  BEGIN_OF_MED(LOC);
}

/*! This constructor allocate a MEDARRAY of SpaceDimension * NumberOfNodes.\n
    It will create empty array for optional data (nodeNumber..) */
//------------------------------------------------------------------------------//
COORDINATE::COORDINATE(int SpaceDimension, int NumberOfNodes, medModeSwitch Mode):
  _coordinateSystem(""),
  _coordinate(SpaceDimension,NumberOfNodes,Mode),
  _coordinateName(SpaceDimension),
  _coordinateUnit(SpaceDimension),
  _nodeNumber()

//------------------------------------------------------------------------------//
{
  const char* LOC = "Constructor COORDINATE";
  BEGIN_OF_MED(LOC);
}

/*! This constructor is specialy designed for use in class GRID.\n
 *  The purpose is to create a COORDINATE without allocating memory for nodes (in the MEDARRAY).
 *  The allocation (call to setCoordinates) is performed afterward by GRID, if necessary.
 *  We set _coordinateName & _coordinateUnit in the constructor, because calls to setCoordinateName
 *  and setCoordinateUnit are not possible before allocation of MEDARRAY.
 */
//------------------------------------------------------------------------------//
COORDINATE::COORDINATE(int SpaceDimension,const string * CoordinateName, const string * CoordinateUnit): 
   _coordinateSystem(""), _coordinate(MEDARRAY<double>()), _coordinateName(SpaceDimension), 
   _coordinateUnit(SpaceDimension), _nodeNumber()
{
    for (int i=0; i<SpaceDimension; ++i)
    {
        _coordinateName[i]=CoordinateName[i];
        _coordinateUnit[i]=CoordinateUnit[i];
    }
}


/*! This constructor will COPY all data (it is a deep copy) included in m.\n  
    But only the default storage mode of coordinates array
    will be copied (not both storage representation modes even if they both
    exist in original object) :  for example only full_interlace mode  */
//------------------------------------------------------------------------------//
COORDINATE::COORDINATE(const COORDINATE & m):
  _coordinateSystem(m._coordinateSystem)
//------------------------------------------------------------------------------//
{
  const char* LOC = "Copy Constructor COORDINATE";
  BEGIN_OF_MED(LOC);

  _coordinate = m._coordinate;
  //int spaceDimension = _coordinate.getLeadingValue();
  int numberOfNodes = _coordinate.getLengthValue();

  SCRUTE_MED(_coordinate.getLeadingValue());
  //setCoordinatesNames((const string*)m._coordinateName) ;
  //setCoordinatesUnits((const string*)m._coordinateUnit) ;
  setCoordinatesNames( m.getCoordinatesNames() );
  setCoordinatesUnits( m.getCoordinatesUnits() );

  if ( (const int * const) m._nodeNumber != NULL)
    _nodeNumber.set(numberOfNodes,(const int*)m._nodeNumber);
  // PG : it's default no ?
//    else
//      {
//        _nodeNumber.set(0);
//      }
}


/* does nothing :   all attributs are object (not pointers) */
//----------------------//
COORDINATE::~COORDINATE()
//----------------------//
{
  MESSAGE_MED("~COORDINATE()");
}

/*! sets the attribute _coordinate with Coordinate           */
//----------------------------------------------------------//
void COORDINATE::setCoordinates(MEDARRAY<double> *Coordinate,bool shallowCopy) 
//----------------------------------------------------------//
{ 

  const medModeSwitch mode = Coordinate->getMode(); 
  //  const int  spaceDimension = (int) Coordinate->getLeadingValue();
  //  const int  numberOfNodes  = (int) Coordinate->getLengthValue();
  if ( Coordinate->get(mode) != NULL)
  {
    if(shallowCopy)
      {
        _coordinate.shallowCopy(*Coordinate);
      }
    else
      {
        MEDARRAY<double> pourAttribut(*Coordinate,false);
        _coordinate = pourAttribut;
        //_coordinate.set(mode,Coordinate->get(mode));
      }
  }
  else
  {
        throw MED_EXCEPTION ( LOCALIZED(STRING("setCoordinates(MEDARRAY<double> *Coordinate)") << "No Coordinate"));
  }
}

/*!
  Sets the value in attribute _coordinate with Coordinate.
  _coordinate must be allocated (with 
  COORDINATE::COORDINATE(int,int,medModeSwitch).
*/
//----------------------------------------------------------//
void COORDINATE::setCoordinates(const medModeSwitch Mode, const double *Coordinate) 
//----------------------------------------------------------//
  { 
//    if (_coordinate == NULL)
//      throw MEDEXCEPTION("COORDINATE::setCoordinates(double*) : coordinate array not allocated !");

    _coordinate.set(Mode,Coordinate);
  }

/*! sets the attribute _coordinateName with CoordinateName   */
//----------------------------------------------------------//
void COORDINATE::setCoordinatesNames(const string * CoordinateName) 
//----------------------------------------------------------//
{
  int SpaceDimension = getSpaceDimension() ;
  //_coordinateName.set(SpaceDimension) ;
  _coordinateName.resize(SpaceDimension);
  for (int i=0; i<SpaceDimension; i++)
    _coordinateName[i]=CoordinateName[i];
}

/*!
  sets the (i+1)^th component of the attribute _coordinateName with
  CoordinateName
*/
//----------------------------------------------------------//
void COORDINATE::setCoordinateName(const string CoordinateName, const int i)
//----------------------------------------------------------//
{
  _coordinateName[i]=CoordinateName;
}

/*! sets the attribute _coordinateUnit with CoordinateUnit   */
//----------------------------------------------------------//
void COORDINATE::setCoordinatesUnits(const string * CoordinateUnit) 
//----------------------------------------------------------//
{ 
  int SpaceDimension = getSpaceDimension() ;
  //_coordinateUnit.set(SpaceDimension) ; 
  _coordinateUnit.resize(SpaceDimension) ; 
  for (int i=0; i<SpaceDimension; i++)
    _coordinateUnit[i]=CoordinateUnit[i];
}

/*!
  sets the (i+1)^th component of the attribute _coordinateUnit with
  CoordinateUnit
*/
//----------------------------------------------------------//
void COORDINATE::setCoordinateUnit(const string CoordinateUnit, const int i) 
//----------------------------------------------------------//
{ 
  _coordinateUnit[i]=CoordinateUnit;
}

/*! sets the attribute _coordinateSystem with CoordinateSystem   */
//----------------------------------------------------------//
void COORDINATE::setCoordinatesSystem(const string CoordinateSystem) 
//----------------------------------------------------------//
{ 
        _coordinateSystem=CoordinateSystem; 
}

/*! sets the attribute _nodeNumber with NodeNumber */
//------------------------------------------------//
void COORDINATE::setNodesNumbers(const int * NodeNumber) 
//------------------------------------------------//
{       
  int NumberOfNodes = getNumberOfNodes() ;
  _nodeNumber.set(NumberOfNodes,NodeNumber) ; 
}

int COORDINATE::getSpaceDimension() const
{       
  return _coordinate.getLeadingValue() ; 
}

int COORDINATE::getNumberOfNodes() const
{       
  return _coordinate.getLengthValue() ; 
}


/*! returns a pointer to the optional array storing 
    eventual nodes numbers */
//-------------------------------------------------//
const int * COORDINATE::getNodesNumbers() const
//-------------------------------------------------//
{
        return  (const int *)_nodeNumber;
}

/*! returns a Pointer to Coordinates Array in specified mode representation */
//--------------------------------------------------------------------------//
const double *  COORDINATE::getCoordinates (medModeSwitch Mode)
//--------------------------------------------------------------------------//
{
        return _coordinate.get(Mode) ;
}

/* returns the coordinate of node Number on axis Axis */
//----------------------------------------------------//
double COORDINATE::getCoordinate(int Number,int Axis) 
//----------------------------------------------------//
{       
        return _coordinate.getIJ(Number,Axis) ;
}

/* returns all nodes coordinates from  axis Axis      */
//----------------------------------------------------//
const double *  COORDINATE::getCoordinateAxis(int Axis)
//----------------------------------------------------//
{                       //< return all nodes coordinates from axis Axis
         return _coordinate.getColumn(Axis) ;
}

/*! returns an array with names of coordinates. \n
      Example : \n
      - x,y,z \n
      - r,teta,phi \n
      - ... */
//--------------------------------------//
const string * COORDINATE::getCoordinatesNames() const
{
  return &(_coordinateName[0]) ;
}

/* returns the name of axis Axis             */
//-------------------------------------------//
string COORDINATE::getCoordinateName(int Axis) const
//-------------------------------------------//
{
  return _coordinateName[Axis-1];
}

/*!  returns an array with units of coordinates (cm, m, mm, ...)
     It could be empty. We suppose we are IS (meter).  */
//-----------------------------------------------------//
const string * COORDINATE::getCoordinatesUnits() const
//-----------------------------------------------------//
{
  return &(_coordinateUnit[0]) ;
}

/*! returns the unit of axis Axis           */
//------------------------------------------//
string COORDINATE::getCoordinateUnit(int Axis) const
//------------------------------------------//
{
  return _coordinateUnit[Axis-1] ;
}
/*! returns "CARTESIAN", "CYLINDRICAL" or "SPHERICAL"*/
//---------------------------------------------------//
string COORDINATE::getCoordinatesSystem() const
//---------------------------------------------------//
{
  return _coordinateSystem ;
}
