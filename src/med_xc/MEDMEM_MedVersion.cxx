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

#include "MEDMEM_MedVersion.hxx"
#include "MEDMEM_Utilities.hxx"

MED_EN::medFileVersion MEDMEM::getMedFileVersion(const string & fileName)
  throw (MEDEXCEPTION)
{

  med_idt fid22;

  med_err ret22;

  med_int major22;
  med_int minor22;
  med_int release22;

  med_access_mode access22 = MED_ACC_RDONLY;

  /*
    Med Version 2.3 access to the file
  */

  fid22 = MEDfileOpen(fileName.c_str(),access22);

  if (fid22 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.2 access");

  ret22 = MEDfileNumVersionRd(fid22,&major22,&minor22,&release22);

  if (ret22 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.2 version numbers reading");

  if ((minor22 == -1) || (release22 == -1))
    {
      MESSAGE_MED("getMedFileVersion the file may have been produced by a version 2.1.x x<5");
      minor22 = 1;
      release22 = 5;
    }

  ret22 = MEDfileClose(fid22);

  if (ret22 < 0)
    throw MEDEXCEPTION("Problem in getMedFileVersion(const string &) Med file V2.2 file closing");

  MED_EN::medFileVersion version= MED_EN::V22;
  if (major22 == 2)
    {
      if (minor22 == 1)
        version = MED_EN::V21;
      else if (minor22 > 1)
        version = MED_EN::V22;
    }
  else
    version = MED_EN::V22;

  MESSAGE_MED("getMedFileVersion the version of the file is " << version);

  return version;
}

med_access_mode MEDMEM::getMedAccessMode(MED_EN::med_mode_acces mode)
{
  //from med.h:
  //    MED_ACC_RDONLY, /**<Ouverture en lecture seule*/
  //    MED_ACC_RDWR,   /**<Ouverture en lecture/ecriture, si un élément existe il est écrasé*/
  //    MED_ACC_RDEXT,  /**<Ouverture en lecture/ecriture, si un élément existe une erreur est générée*/
  //    MED_ACC_CREAT,  /**<Créer le fichier s'il n'existe pas, l'écrase sinon*/
  //    MED_ACC_UNDEF   /**<Variable globale interne initialisée au mode d'ouverture*/ 
  switch ( mode ) {
  case MED_EN::RDONLY: return MED_ACC_RDONLY;
  case MED_EN::WRONLY: return MED_ACC_CREAT;
  case MED_EN::RDWR:   return MED_ACC_RDWR;
  default:
    return med_access_mode( mode );
  }
}
