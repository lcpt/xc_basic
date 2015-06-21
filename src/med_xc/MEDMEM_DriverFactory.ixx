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

#ifndef DRIVERFACTORY_IXX
#define DRIVERFACTORY_IXX

#include "MEDMEM_VtkFieldDriver.hxx"
#include "MEDMEM_MedFieldDriver.hxx"
#include "MEDMEM_AsciiFieldDriver.hxx"
#include "MEDMEM_EnsightFieldDriver.hxx"

namespace MEDMEM {
  template<class T, class INTERLACING_TAG>
  GENDRIVER * DRIVERFACTORY::buildDriverForField(driverTypes driverType,
                                                 const std::string & fileName,
                                                 FIELD<T,INTERLACING_TAG> *field,
                                                 MED_EN::med_mode_acces access)
  {
    GENDRIVER *ret= NULL;
    switch(driverType)
      {
      case MED_DRIVER : {
        switch(access)
          {
          case MED_EN::RDONLY : {
            ret = new MED_FIELD_RDONLY_DRIVER<T>(fileName,field);
            break;
          }
          case MED_EN::WRONLY : {
            ret= new MED_FIELD_WRONLY_DRIVER<T>(fileName,field);
            break;
          }
          case MED_EN::RDWR : {
            ret = new MED_FIELD_RDWR_DRIVER<T>(fileName,field);
            break;
          }
          default:
            throw MED_EXCEPTION ("access type has not been properly specified to the method");
          }
        break;
      }

      case ENSIGHT_DRIVER : {
        switch(access)
          {
          case MED_EN::RDONLY : {
            ret = new ENSIGHT_FIELD_RDONLY_DRIVER(fileName,field);
            break;
          }
          case MED_EN::WRONLY : {
            ret=new ENSIGHT_FIELD_WRONLY_DRIVER(fileName,field);
            break;
          }
          case MED_EN::RDWR : {
            throw MED_EXCEPTION ("not yet implemented");
            break ;
          }
          default:
            throw MED_EXCEPTION ("access type has not been properly specified to the method");
          }
        break;
      }

      case VTK_DRIVER : {
        switch(access)
          {
          case MED_EN::RDONLY : {
            throw MED_EXCEPTION ("access mode other than MED_ECRI and MED_REMP has been specified with the VTK_DRIVER type which is not allowed because VTK_DRIVER is only a write access driver");
            break;
          }
          case MED_EN::WRONLY : {
            ret=new VTK_FIELD_DRIVER<T>(fileName,field);
            break;
          }
          case MED_EN::RDWR : {
            ret=new VTK_FIELD_DRIVER<T>(fileName,field);
            break ;
          }
          default:
            throw MED_EXCEPTION ("access type has not been properly specified to the method");
          }
        break;
      }

      case GIBI_DRIVER : {
        throw MED_EXCEPTION ("driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD");
        break;
      }

      case PORFLOW_DRIVER : {
        throw MED_EXCEPTION ("driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD");
        break;
      }

      case ASCII_DRIVER : {
        switch(access)
          {
          case MED_EN::WRONLY : {
            ret=new ASCII_FIELD_DRIVER<T>(fileName,field);
            break;
          }
          default:
            throw MED_EXCEPTION ("driver ASCII_DRIVER on FIELD only in write mod");
          }
        break;
      }

      case NO_DRIVER : {
        throw MED_EXCEPTION ("driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD");
        break;
      }
      default:
        MED_EXCEPTION ("driverType other than MED_DRIVER and VTK_DRIVER has been specified to the method which is not allowed for the object FIELD");
      }
    return ret;
  }

  template<class T, class INTERLACING_TAG>
  GENDRIVER * DRIVERFACTORY::buildFieldDriverFromFile(const string & fileName,
                                                      FIELD<T,INTERLACING_TAG> * ptrField,
                                                      MED_EN::med_mode_acces access)
  {
    MED_EN::medFileVersion version = MED_EN::V22;

    try
      {
        version = getMedFileVersion(fileName);
      }
    catch (MEDEXCEPTION & )
      {
      }

    MESSAGE_MED("buildFieldDriverFromFile version of the file " << version);

    GENDRIVER * driver=0;

    switch(access)
      {
      case MED_EN::RDONLY : {
        if (version == MED_EN::V21)
          throw MED_EXCEPTION ("med-2.1 files are no more supported");
        else if (version == MED_EN::V22)
          driver = new MED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField);
        return driver;
      }
      case MED_EN::WRONLY : {
        if (version == MED_EN::V21)
          throw MED_EXCEPTION ("med-2.1 files are no more supported");
        else if (version == MED_EN::V22)
          driver = new MED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField);
        return driver;
      }
      case MED_EN::RDWR : {
        if (version == MED_EN::V21)
          throw MED_EXCEPTION ("med-2.1 files are no more supported");
        else if (version == MED_EN::V22)
          driver = new MED_FIELD_RDWR_DRIVER<T>(fileName,ptrField);
        return driver;
      }
      default:
        throw MED_EXCEPTION ("access type has not been properly specified to the method");
      }
    return driver;
  }

  template<class T, class INTERLACING_TAG>
  GENDRIVER * DRIVERFACTORY::buildConcreteMedDriverForField(const std::string & fileName,
                                                            FIELD<T,INTERLACING_TAG> *ptrField,
                                                            MED_EN::med_mode_acces access,
                                                            MED_EN::medFileVersion version)
  {

    MESSAGE_MED("buildConcreteMedDriverForField version of the file " << version);

    if (version == MED_EN::V21)
      throw MED_EXCEPTION ("med-2.1 files are no more supported");

    GENDRIVER * driver=0;

    switch(access)
      {
      case MED_EN::RDONLY : {
        driver = new MED_FIELD_RDONLY_DRIVER<T>(fileName,ptrField);
        return driver;
      }
      case MED_EN::WRONLY : {
        driver = new MED_FIELD_WRONLY_DRIVER<T>(fileName,ptrField);
        return driver;
      }
      case MED_EN::RDWR : {
        driver = new MED_FIELD_RDWR_DRIVER<T>(fileName,ptrField);
        return driver;
      }
      default:
        throw MED_EXCEPTION ("access type has not been properly specified to the method");
      }
    return driver;
  }
}
#endif
