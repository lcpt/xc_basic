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

#ifndef DRIVERFACTORY_HXX
#define DRIVERFACTORY_HXX

#include "MEDMEM.hxx"

#include "MEDMEM_MedVersion.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_FieldForward.hxx"

#include <string>

namespace MEDMEM {

  class GMESH;
  //class MED;
  class GENDRIVER;

  namespace DRIVERFACTORY {

    MEDMEM_EXPORT extern bool globalVtkBinaryFormatForWriting;

    MEDMEM_EXPORT bool getVtkBinaryFormatForWriting();

    MEDMEM_EXPORT void setVtkBinaryFormatForWriting(bool isBinary);

    MEDMEM_EXPORT driverTypes deduceDriverTypeFromFileName(const std::string & fileName);

    driverTypes deduceDriverTypeFromFileName(const std::string & fileName);

    MEDMEM_EXPORT GENDRIVER * buildDriverForMesh(driverTypes driverType,
                                                 const std::string & fileName,
                                                 GMESH *mesh,const string &  driverName,
                                                 MED_EN::med_mode_acces access);

    template<class T, class INTERLACING_TAG>
    GENDRIVER * buildDriverForField(driverTypes driverType,
                                    const std::string & fileName,
                                    FIELD<T,INTERLACING_TAG> *fielde,
                                    MED_EN::med_mode_acces access);
    MEDMEM_EXPORT GENDRIVER * buildMeshDriverFromFile(const string & fileName,
                                                      GMESH * ptrMesh,
                                                      MED_EN::med_mode_acces access);
    template<class T, class INTERLACING_TAG>
    GENDRIVER * buildFieldDriverFromFile(const string & fileName,
                                         FIELD<T,INTERLACING_TAG> * ptrField,
                                         MED_EN::med_mode_acces access);
    MEDMEM_EXPORT GENDRIVER * buildConcreteMedDriverForMesh(const std::string & fileName,
                                                            GMESH *mesh,
                                                            const string & driverName,
                                                            MED_EN::med_mode_acces access,
                                                            MED_EN::medFileVersion version);
    template<class T, class INTERLACING_TAG>
    GENDRIVER * buildConcreteMedDriverForField(const std::string & fileName,
                                               FIELD<T,INTERLACING_TAG> *fielde,
                                               MED_EN::med_mode_acces access,
                                               MED_EN::medFileVersion version);
  }
}

#include"MEDMEM_DriverFactory.ixx"

#endif
