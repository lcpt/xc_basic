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

#ifndef ENSIGHT_MED_DRIVER_HXX
#define ENSIGHT_MED_DRIVER_HXX

#include <string>
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_Exception.hxx"

#include "MEDMEM_EnsightUtils.hxx"

#include <fstream>

using namespace std ;


namespace MEDMEM {
class MESH;
class SUPPORT;
class FIELD_;

// ==============================================================================
/*!
 * \brief Base of EnSight drivers
 */
// ==============================================================================

class MEDMEM_EXPORT ENSIGHT_MED_DRIVER : public MEDMEM_ENSIGHT::_CaseFileDriver_User
{
protected:
  
  virtual void openConst()  const;

public :
  ENSIGHT_MED_DRIVER();
  ENSIGHT_MED_DRIVER(const string & fileName, MED_EN::med_mode_acces accessMode);
  ENSIGHT_MED_DRIVER(const ENSIGHT_MED_DRIVER & driver);
  virtual ~ENSIGHT_MED_DRIVER();

  virtual void open();
  virtual void close();
};

// ==============================================================================
/*!
 * \brief Reading EnSight driver
 */
// ==============================================================================

class MEDMEM_EXPORT ENSIGHT_MED_RDONLY_DRIVER : public virtual ENSIGHT_MED_DRIVER
{
public :
  ENSIGHT_MED_RDONLY_DRIVER();
  ENSIGHT_MED_RDONLY_DRIVER(const string & fileName, std::vector< FIELD_* >& fields);
  ENSIGHT_MED_RDONLY_DRIVER(const ENSIGHT_MED_RDONLY_DRIVER & driver);
  virtual ~ENSIGHT_MED_RDONLY_DRIVER();
  virtual void write          ( void ) const throw (MEDEXCEPTION) ;
  virtual void read           ( void ) ;
  virtual void readFileStruct ( void ) ;
  GENDRIVER * copy ( void ) const;
private:

  std::vector< FIELD_* > * _fields;
  bool _isFileStructRead;
};

// ==============================================================================
/*!
 * \brief Writing EnSight driver.
 * To set writing format use
 * setEnSightFormatForWriting(EnSightFormat) and
 * setEnSightBinaryFormatForWriting(bool)
 */
// ==============================================================================

class MEDMEM_EXPORT ENSIGHT_MED_WRONLY_DRIVER : public virtual ENSIGHT_MED_DRIVER
{
  const std::vector< const FIELD_* > _fields;
public :
  ENSIGHT_MED_WRONLY_DRIVER();
  ENSIGHT_MED_WRONLY_DRIVER(const string & fileName, const std::vector< const FIELD_* >& fields);
  ENSIGHT_MED_WRONLY_DRIVER(const ENSIGHT_MED_WRONLY_DRIVER & driver);
  virtual ~ENSIGHT_MED_WRONLY_DRIVER();
  virtual void write          ( void ) const throw (MEDEXCEPTION) ;
  virtual void read           ( void ) throw (MEDEXCEPTION) ;
  GENDRIVER * copy ( void ) const;
};

}


#endif /* ENSIGTH_MED_DRIVER_HXX */
