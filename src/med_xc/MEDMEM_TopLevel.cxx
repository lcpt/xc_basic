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

#include "MEDMEM_TopLevel.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_DriverFactory.hxx"

using namespace MED_EN;
using namespace MEDMEM;

MESH *::MEDMEM::readMeshInFile(const std::string& fileName, const std::string& meshName)
{
  MESH *mesh = new MESH;
  driverTypes type = DRIVERFACTORY::deduceDriverTypeFromFileName(fileName);
  int id = mesh->addDriver(type,fileName,meshName,RDWR);
  mesh->read(id);
  return mesh;
}

void ::MEDMEM::writeMeshToFile(const MESH *meshObj, const std::string& fileName)
{
  MESH *meshObjNoC = (MESH *)meshObj;//MED::write should be a const method ... As it's not the case
  driverTypes type = DRIVERFACTORY::deduceDriverTypeFromFileName(fileName);
  int id = meshObjNoC->addDriver(type,fileName,meshObjNoC->getName(),RDWR);
  meshObjNoC->write(id);
}
