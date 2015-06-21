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

using namespace MEDMEM ;
using namespace MED_EN ;

int main (int argc, char ** argv)
{
  //   const string MedFile = "polyedres.med" ;
  //   const string MeshName = "Erreur orientation" ;
  //   const string MedFile = "polygones.med" ;
  //   const string MeshName = "Bord" ;
  const std::string MedFile = "pointe.med" ;
  const std::string MeshName = "maa1" ;
  MESH myMesh(MED_DRIVER,MedFile,MeshName) ;

  std::cout << "Mesh name : " << myMesh.getName()  << std::endl << std::endl ; 

  // we get all type for cell entity :
  int NumberOfTypes = myMesh.getNumberOfTypes(MED_CELL) ;
  std::cout << "Show Connectivity (Nodal) :" << std::endl ;
  // this example use access with a specified medGeometryElement array
  const medGeometryElement * Types = myMesh.getTypes(MED_CELL);
  string * cellTypeNames =  myMesh.getCellTypeNames(MED_CELL);
  for (int i=0; i<NumberOfTypes; i++) {
    std::cout << "For type " << cellTypeNames[i] << " : " << std::endl ;
    medGeometryElement myType = Types[i] ;
    int NumberOfElements = myMesh.getNumberOfElements(MED_CELL,myType);
    int NomberOfNodesPerCell = Types[i]%100 ;
    const int * Connectivity = myMesh.getConnectivity(MED_NODAL,MED_CELL,myType);
    for (int j=0; j<NumberOfElements; j++){
      std::cout << "Element "<< j+1 <<" : " ;
      for (int k=0; k<NomberOfNodesPerCell; k++)
        std::cout << Connectivity[j*NomberOfNodesPerCell+k]<<" ";
      std::cout << std::endl ;
    }
  }
  std::cout << "Show Reverse Nodal Connectivity :" << std::endl ;
  // this example use global access with index array
  int NumberOfNodes = myMesh.getNumberOfNodes() ;
  const int * ReverseNodalConnectivity = 
    myMesh.getReverseConnectivity(MED_NODAL) ;
  const int * ReverseNodalConnectivityIndex = 
    myMesh.getReverseConnectivityIndex(MED_NODAL) ;
  for (int i=0; i<NumberOfNodes; i++) {
    std::cout << "Node "<<i+1<<" : " ;
    int IndexBegin = ReverseNodalConnectivityIndex[i] ;
    int IndexEnd = ReverseNodalConnectivityIndex[i+1] ;
    for (int j=IndexBegin; j<IndexEnd; j++)
      // Index value begin at 1 so use j-1
      std::cout << ReverseNodalConnectivity[j-1] << " " ; 
    std::cout << std::endl ;
  }

  std::cout << "Show Connectivity (Descending) :" << std::endl ;
  // this example use global access with index array
  int NumberOfElements = myMesh.getNumberOfElements(MED_CELL,MED_ALL_ELEMENTS);
  const int * DescendingConnectivity =  
    myMesh.getConnectivity(MED_DESCENDING,MED_CELL,MED_ALL_ELEMENTS);
  const int * DescendingConnectivityIndex =
    myMesh.getConnectivityIndex(MED_DESCENDING,MED_CELL);
  for (int i=0; i<NumberOfElements; i++) {
    std::cout << "Element "<<i+1<<" : " ;
    int IndexBegin = DescendingConnectivityIndex[i] ;
    int IndexEnd = DescendingConnectivityIndex[i+1] ;
    for (int j=IndexBegin; j<IndexEnd; j++)
      // Index value begin at 1 so use j-1
      std::cout << DescendingConnectivity[j-1] << " " ;
    std::cout << std::endl ;
  }

  std::cout << "Show Reverse Descending Connectivity :" << std::endl ;
  // this example use global access with Index array
  const int * ReverseDescendingConnectivity = 
    myMesh.getReverseConnectivity(MED_DESCENDING) ;
  const int * ReverseDescendingConnectivityIndex = 
    myMesh.getReverseConnectivityIndex(MED_DESCENDING) ;

  int MeshDimension = myMesh.getMeshDimension() ;
  int NumberOfConstituents  = 0;
  string Constituent ;
  medEntityMesh ConstituentEntity ;
  std::cout << "**** mesh dimension: " << MeshDimension << std::endl;
  // test if we have face (3D) or edge (2D)
  if (MeshDimension==3) {
    Constituent = "Face" ;
    ConstituentEntity = MED_EN::MED_FACE ;
  }
  if (MeshDimension==2) {
    Constituent = "Edge" ;
    ConstituentEntity = MED_EN::MED_EDGE ;
  }

  NumberOfConstituents = 
    myMesh.getNumberOfElements(ConstituentEntity,MED_ALL_ELEMENTS);

  if (MeshDimension==1) {
    MESSAGE_MED("ERROR : MeshDimension = 1 !");
    MESSAGE_MED("We could not see Reverse Descending Connectivity.") ;
  } else {
    for (int i=0; i<NumberOfConstituents; i++) {
      std::cout << Constituent << " " << i+1 << " : " ;
      int IndexBegin = ReverseDescendingConnectivityIndex[i] ;
      int IndexEnd = ReverseDescendingConnectivityIndex[i+1] ;
      for (int j=IndexBegin;j<IndexEnd;j++)
        // Index value begin at 1 so use j-1
        std::cout << ReverseDescendingConnectivity[j-1] << " " ;
      std::cout << std::endl ;
    }
  }
  std::cout << "Show "<< Constituent <<" Connectivity (Nodal) :" << std::endl ;
  // this example use global access with index array
  const int * ConstituentConnectivity = myMesh.getConnectivity(MED_EN::MED_NODAL,ConstituentEntity,MED_ALL_ELEMENTS);
  std::cout << "**** SALE" << std::endl;
  const int * ConstituentConnectivityIndex= myMesh.getConnectivityIndex(MED_EN::MED_NODAL,ConstituentEntity);
  for (int i=0; i<NumberOfConstituents; i++) {
    std::cout << Constituent << " " << i+1 << " : " ;
    int IndexBegin = ConstituentConnectivityIndex[i] ;
    int IndexEnd = ConstituentConnectivityIndex[i+1] ;
    for (int j=IndexBegin; j<IndexEnd; j++)
      // Index value begin at 1 so use j-1
      std::cout << ConstituentConnectivity[j-1]<<" ";
    std::cout << std::endl ;
  }

  return 0 ;
}
