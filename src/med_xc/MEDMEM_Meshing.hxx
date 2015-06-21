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
  File MEDMEM_Meshing.hxx
*/

#ifndef MESHING_HXX
#define MESHING_HXX

#include "MEDMEM.hxx"

#include "MEDMEM_Mesh.hxx"


namespace MEDMEM {

/*!
  This class MESHING is a special class to set a MESH object.
*/

class MEDMEM_EXPORT MESHING: public MESH
{
 public :
  MESHING();
  ~MESHING();

  void setCoordinates      (const int SpaceDimension,
                            const int NumberOfNodes,
                            const double * Coordinates,
                            const string System,
                            const MED_EN::medModeSwitch Mode) ;
  void setCoordinatesNames (const string * names) ;
  void setCoordinateName   (const string name, const int i) ;
  void setCoordinatesUnits (const string * units) ;
  void setCoordinateUnit   (const string unit, const int i) ;

  void setNumberOfTypes    (const int NumberOfTypes,
                            const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION) ;

  void setTypes            (const MED_EN::medGeometryElement * Types,
                            const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION) ;

  void setNumberOfElements (const int * NumberOfElements,
                            const MED_EN::medEntityMesh Entity) throw (MEDEXCEPTION) ;

  void setConnectivity     (const MED_EN::medEntityMesh Entity,
                            const MED_EN::medGeometryElement Type,
                            const int * Connectivity,
                            const int * PolyConnectivityIndex=0)    throw (MEDEXCEPTION) ;

  void addGroup            (const GROUP & Group)                throw (MEDEXCEPTION) ;
};
}

#endif /* MESHING_HXX */
