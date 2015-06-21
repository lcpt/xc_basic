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

#ifndef __MEDMEM_TOPLEVEL_HXX__
#define __MEDMEM_TOPLEVEL_HXX__

#include "MEDMEM_FieldForward.hxx"
#include "MEDMEM_Exception.hxx"
#include <string>

namespace MEDMEM
{
  class MED;
  class MESH;
}

namespace MEDMEM
{
  //read part
  MEDMEM_EXPORT MESH *readMeshInFile(const std::string& fileName, const std::string& meshName);
  template<class T>
  FIELD<T> *readFieldInFile(const std::string& fileName, const std::string& fieldName);
  //write part
  MEDMEM_EXPORT void writeMeshToFile(const MESH *meshObj, const std::string& fileName);
  template<class T>
  void writeFieldToFile(const FIELD<T> *fieldObj, const std::string& fileName);
}

#include "MEDMEM_Field.hxx"

namespace MEDMEM
{
  template<class T>
  FIELD<T> *readFieldInFile(const std::string& fileName, const std::string& fieldName)
  {
    FIELD<T> *ret=new FIELD<T>();
    ret->setName(fieldName);
    driverTypes type=DRIVERFACTORY::deduceDriverTypeFromFileName(fileName);
    int id=ret->addDriver(type,fileName,fieldName);
    ret->read(id);
    return ret;
  }
  
  template<class T>
  void writeFieldToFile(const FIELD<T> *fieldObj, const std::string& fileName)
  {
    FIELD<T> *fieldObjNoC=(FIELD<T> *)fieldObj;
    driverTypes type=DRIVERFACTORY::deduceDriverTypeFromFileName(fileName);
    int id=fieldObjNoC->addDriver(type, fileName, fieldObj->getName());
    fieldObjNoC->write(id);
  }
}

#endif
