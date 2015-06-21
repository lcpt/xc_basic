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

#include "xc_basic/src/med_xc/MEDMEM_Mesh.hxx"
#include <iostream>

using namespace MEDMEM ;
using namespace MED_EN ;

int main (int argc, char ** argv)
  {

    const std::string MedFile = "pointe.med" ;
  const std::string MeshName = "maa1" ;
  MESH myMesh(MED_DRIVER,MedFile,MeshName) ;

  std::cout << "Mesh name : " << myMesh.getName()  << std::endl << std::endl ; 

  int SpaceDimension = myMesh.getSpaceDimension() ;
  int NumberOfNodes = myMesh.getNumberOfNodes() ;
  std::cout << "Space dimension  : " << SpaceDimension << std::endl << std::endl ; 
  std::cout << "Number of nodes  : " << NumberOfNodes  << std::endl << std::endl ; 

  std::cout << "Show Nodes Coordinates : " << std::endl ;

  // coordinates names :
  std::cout << "Name :" << std::endl ;
  const std::string * CoordinatesNames = myMesh.getCoordinatesNames() ;
  for(int i=0; i<SpaceDimension ; i++) {
    std::cout << " - " << CoordinatesNames[i] << std::endl ;
  }
  // coordinates unit :
  std::cout << "Unit :" << std::endl ;
  const std::string * CoordinatesUnits = myMesh.getCoordinatesUnits() ;
  for(int i=0; i<SpaceDimension ; i++) {
    std::cout << " - " << CoordinatesUnits[i] << std::endl ;
  }
  // coordinates value
  const double * Coordinates = 
    myMesh.getCoordinates(med_xc::MED_FULL_INTERLACE) ;
  for(int i=0; i<NumberOfNodes ; i++) {
    std::cout << "Nodes " << i+1 << " : " ;
    for (int j=0; j<SpaceDimension ; j++)
      std::cout << Coordinates[i*SpaceDimension+j] << " " ;
    std::cout << std::endl ;
  }
  
  return 0 ;
}
