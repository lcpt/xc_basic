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

int main (int argc, char ** argv)
 {

  const std::string MedFile = "pointe.med" ;
  const std::string MeshName = "maa1" ;
  
  // create a MESH object by reading it on file :
  MESH myMesh(MED_DRIVER,MedFile,MeshName) ;

  std::string Name = myMesh.getName() ;
  if (Name != MeshName) {
    std::cout << "Error when reading mesh name : We ask for mesh #"
         << MeshName <<"# and we get mesh #"<< Name <<"#"<< std::endl << std::endl ;
    return -1;
  }

  std::cout << "Mesh name : " << Name  << std::endl << std::endl ; 

  int SpaceDimension = myMesh.getSpaceDimension() ;
  int MeshDimension = myMesh.getMeshDimension() ;
  
  std::cout << "Space Dimension : " << SpaceDimension << std::endl << std::endl ; 
  std::cout << "Mesh Dimension : " << MeshDimension << std::endl << std::endl ; 

  return 0 ;
}
