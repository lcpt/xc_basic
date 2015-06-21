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
 File Coordinate.hxx
*/

#ifndef COORDINATE_HXX
#define COORDINATE_HXX

#include "MEDMEM.hxx"

#include <string>
#include <vector>

#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"


#include "MEDMEM_Array.hxx"

/*!
    This class contains coordinates of the nodes .\n
    It could also store useful optional information about nodes
    as  node numbers and about axes as names or units. \n
    spaceDimension and  numberOfNodes can be found in _coordinate object.
*/

namespace MEDMEM {
class MEDMEM_EXPORT COORDINATE
  {
  protected:
     /*! contains "CARTESIAN", "CYLINDRICAL" or "SPHERICAL"*/
    string _coordinateSystem;


    /*! _coordinate is a MEDARRAY<double> object : \n
        - spaceDimension \n
        - numberOfNodes \n
        - default storage mode \n
        - Up to 4 "PointerOf" to an array of size spaceDimension*NumberOfNodes\n

        Storing the object (not a pointer to this object) is more convenient for memory
        management.
    */
    mutable MEDARRAY<double> _coordinate;

    /*! PointerOf to an array of size spaceDimension storing axes names*/
    //PointerOf<string> _coordinateName;
    vector<std::string> _coordinateName;

    /*! PointerOf to an array of size spaceDimension storing units */
    //PointerOf<string>  _coordinateUnit;
    vector<std::string>  _coordinateUnit;

    /*! PointerOf to an array of size NumberOfNodes : optional nodes numbers */
    PointerOf<int> _nodeNumber;


  public:


  friend class MED_MESH_RDONLY_DRIVER;
  friend class MED_MESH_WRONLY_DRIVER;


  COORDINATE();
  COORDINATE(int SpaceDimension, int NumberOfNodes, MED_EN::medModeSwitch Mode);
  COORDINATE(int SpaceDimension,const string * CoordinateName, const string * CoordinateUnit);
  COORDINATE(const COORDINATE & m);
  virtual ~COORDINATE();

  void setCoordinates(MEDARRAY<double> *Coordinate,bool shallowCopy=false);
  void setCoordinates(const MED_EN::medModeSwitch Mode, const double *Coordinate);
  void setCoordinatesNames(const string * CoordinateName);
  void setCoordinateName(const string CoordinateName, const int i);
  void setCoordinatesUnits(const string * CoordinateUnit);
  void setCoordinateUnit(const string CoordinateUnit, const int i);
  void setCoordinatesSystem(const string CoordinateSystem);
  void setNodesNumbers(const int * NodeNumber);

  int getSpaceDimension() const;
  int getNumberOfNodes() const;

  virtual const int *getNodesNumbers() const;
  //const int*            getNodesNumbers() ;
  std::string getCoordinatesSystem() const;

  virtual const double *getCoordinates(MED_EN::medModeSwitch Mode);
  virtual double getCoordinate(int Number,int Axis);
  virtual const double *getCoordinateAxis(int Axis);

  const string * getCoordinatesNames() const;
  string   getCoordinateName(int Axis) const;
  const string * getCoordinatesUnits() const;
  string   getCoordinateUnit(int Axis) const;
};
}

#endif /* COORDINATE_HXX */
