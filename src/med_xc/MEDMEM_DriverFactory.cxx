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

#include "MEDMEM_DriverFactory.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_GibiMeshDriver.hxx"
#include "MEDMEM_PorflowMeshDriver.hxx"
#include "MEDMEM_VtkMeshDriver.hxx"
#include "MEDMEM_VtkMedDriver.hxx"
#include "MEDMEM_EnsightFieldDriver.hxx"
#include "MEDMEM_EnsightMeshDriver.hxx"

#include "MEDMEM_Exception.hxx"

#include "MEDMEM_MedVersion.hxx"
#include "MEDMEM_MedMeshDriver.hxx"

using namespace MEDMEM;
using namespace MED_EN;

template<>
void MEDMEM::fill<-1,0x3>(double *a, const double *b)
{
}

template<>
bool MEDMEM::compare<-1>(const double *a, const double *b)
{
  return false;
}

bool DRIVERFACTORY::globalVtkBinaryFormatForWriting = false;

bool DRIVERFACTORY::getVtkBinaryFormatForWriting()
{
  return globalVtkBinaryFormatForWriting;
}

void DRIVERFACTORY::setVtkBinaryFormatForWriting(bool isBinary)
{
  globalVtkBinaryFormatForWriting = isBinary;
}

driverTypes DRIVERFACTORY::deduceDriverTypeFromFileName(const std::string & fileName)
{
  string extension(fileName);
  string::size_type pos=extension.rfind('.');
  if(pos==string::npos)
    return NO_DRIVER;
  extension.erase(0,pos+1);
  if(extension=="med")
    return MED_DRIVER;
  if(extension=="sauve" || extension=="sauv")
    return GIBI_DRIVER;
  if(extension=="cnc" || extension=="inp" || extension=="xyz")
    return PORFLOW_DRIVER;
  if(extension=="vtk")
    return VTK_DRIVER;
  if(extension=="case")
    return ENSIGHT_DRIVER;
  return NO_DRIVER;
}

GENDRIVER *DRIVERFACTORY::buildDriverForMesh(driverTypes driverType,
                                             const std::string & fileName,
                                             GMESH *mesh,
                                             const string & driverName,
                                             MED_EN::med_mode_acces access)
{
  GENDRIVER *ret;
  switch(driverType)
    {
    case MED_DRIVER : {
      switch(access)
        {
        case RDONLY : {
          ret = new MED_MESH_RDONLY_DRIVER(fileName, mesh);
          ret->setMeshName(driverName);
          return ret;
        }
        case WRONLY : {
          ret = new MED_MESH_WRONLY_DRIVER(fileName, mesh, access);
          ret->setMeshName(driverName);
          return ret;
        }
        case RDWR : {
          ret = new MED_MESH_RDWR_DRIVER(fileName, mesh);
          ret->setMeshName(driverName);
          return ret;
        }
        default:
          throw MED_EXCEPTION ("access type has not been properly specified to the method");
        }
      break;
    }

    case GIBI_DRIVER : {
      if ( mesh->getIsAGrid() )
        throw MED_EXCEPTION("GIBI file can contain unstructured mesh only, not a GRID");
      switch(access)
        {
        case RDONLY : {
          ret=new GIBI_MESH_RDONLY_DRIVER(fileName,(MESH*)mesh);
          return ret;
        }
        case RDWR :
          ret=new GIBI_MESH_RDWR_DRIVER(fileName,(MESH*)mesh);
          return ret;
          
        case WRONLY :{
          ret=new GIBI_MESH_WRONLY_DRIVER(fileName,(MESH*)mesh);
          return ret;
        }
        default:
          throw MED_EXCEPTION ("access type has not been properly specified to the method");
        }
      break;
    }

    case PORFLOW_DRIVER : {
      if ( mesh->getIsAGrid() )
        throw MED_EXCEPTION("PORFLOW file can contain unstructured mesh only, not a GRID");
      switch(access)
        {
        case RDONLY : {
          ret=new PORFLOW_MESH_RDONLY_DRIVER(fileName,(MESH*)mesh);
          return ret;
        }
        case RDWR :
        case WRONLY : {
          throw MED_EXCEPTION ("access mode other than MED_LECT has been specified with the PORFLOW_DRIVER type which is not allowed because PORFLOW_DRIVER is only a read access driver");
        }
        default:
          throw MED_EXCEPTION ("access type has not been properly specified to the method");
        }
      break;
    }

    case ENSIGHT_DRIVER : {
      if ( mesh->getIsAGrid() )
        throw MED_EXCEPTION("EnSight driver reads unstructured mesh, not a GRID");
      switch(access)
        {
        case RDONLY : {
          ret=new ENSIGHT_MESH_RDONLY_DRIVER(fileName,(MESH*)mesh);
          return ret;
        }
        case WRONLY : {
          ret=new ENSIGHT_MESH_WRONLY_DRIVER(fileName,(MESH*)mesh);
          return ret;
        }
        case RDWR : {
          throw MED_EXCEPTION ("not yet implemented");
          return ret;
        }
        default:
          throw MED_EXCEPTION ("access type has not been properly specified to the method");
        }
      break;
    }

    case VTK_DRIVER : {
      switch(access)
        {
        case RDONLY : {
          throw MED_EXCEPTION ("access mode other than MED_ECRI or MED_REMPT has been specified with the VTK_DRIVER type which is not allowed because VTK_DRIVER is only a write access driver");
        }
        case RDWR :
        case WRONLY : {
          ret=new VTK_MESH_DRIVER(fileName,mesh);
          return ret;
        }

        default:
          throw MED_EXCEPTION ("access type has not been properly specified to the method");
        }
      break;
    }

    case NO_DRIVER : {
      throw MED_EXCEPTION ("NO_DRIVER has been specified to the method 1 which is not allowed");
    }
    default:
      throw MED_EXCEPTION ("other driver than MED_DRIVER GIBI_DRIVER PORFLOW_DRIVER and VT_DRIVER has been specified to the method which is not allowed");
    }
}

GENDRIVER * DRIVERFACTORY::buildMeshDriverFromFile(const string &         fileName,
                                                   GMESH *                ptrMesh,
                                                   MED_EN::med_mode_acces access)
{
  medFileVersion version = MED_EN::V22;

  try
    {
      version = getMedFileVersion(fileName);
    }
  catch (MEDEXCEPTION & ex)
    {
    }

  MESSAGE_MED("buildMeshDriverFromFile version of the file " << version);

  if (version == MED_EN::V21)
    throw MED_EXCEPTION ("med-2.1 files are no more supported");

  GENDRIVER * driver=0;

  switch(access)
    {
    case RDONLY : {
      driver = new MED_MESH_RDONLY_DRIVER(fileName,ptrMesh);
      return driver;
    }
    case WRONLY : {
      driver = new MED_MESH_WRONLY_DRIVER(fileName,ptrMesh,access);
      return driver;
    }
    case RDWR : {
      driver = new MED_MESH_RDWR_DRIVER(fileName,ptrMesh);
      return driver;
    }
    default:
      throw MED_EXCEPTION ("access type has not been properly specified to the method");
    }
  return driver;
}

GENDRIVER * DRIVERFACTORY::buildConcreteMedDriverForMesh(const std::string &    fileName,
                                                         GMESH *                ptrMesh,
                                                         const string &         driverName,
                                                         MED_EN::med_mode_acces access,
                                                         MED_EN::medFileVersion version)
{
  GENDRIVER * driver=0;

  MESSAGE_MED("buildConcreteMedDriverForMesh version of the file " << version);

  if (version == MED_EN::V21)
    throw MED_EXCEPTION ("med-2.1 files are no more supported");

  switch(access)
    {
    case RDONLY : {
      driver = new MED_MESH_RDONLY_DRIVER(fileName,ptrMesh);
      driver->setMeshName(driverName);
      return driver;
    }
    case WRONLY : {
      driver = new MED_MESH_WRONLY_DRIVER(fileName,ptrMesh);
      driver->setMeshName(driverName);
      return driver;
    }
    case RDWR : {
      driver = new MED_MESH_RDWR_DRIVER(fileName,ptrMesh);
      driver->setMeshName(driverName);
      return driver;
    }
    default:
      throw MED_EXCEPTION ("access type has not been properly specified to the method");
    }
  return driver;
}
