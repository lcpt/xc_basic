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

#ifndef ENSIGHT_FIELD_DRIVER_HXX
#define ENSIGHT_FIELD_DRIVER_HXX

#include <string>

#include "MEDMEM_EnsightUtils.hxx"

// ==========================================================================================
/*!
  Driver Ensight for FIELD.
*/
// ==========================================================================================

namespace MEDMEM {

class MEDMEM_EXPORT ENSIGHT_FIELD_DRIVER : public MEDMEM_ENSIGHT::_CaseFileDriver_User
{
protected:
  FIELD_ *    _ptrField;
  std::string _fieldName;
  std::string _constantValue;

  void openConst(bool checkDataFile=false) const throw (MEDEXCEPTION);

public :

  /*!
    Constructor.
  */
  ENSIGHT_FIELD_DRIVER();
  ENSIGHT_FIELD_DRIVER(const std::string &    fileName,
                       FIELD_ *               ptrField,
                       MED_EN::med_mode_acces accessMode);
  /*!
    Copy constructor.
  */
  ENSIGHT_FIELD_DRIVER(const ENSIGHT_FIELD_DRIVER & fieldDriver);
  virtual void merge ( const GENDRIVER& driver );
  /*!
    Destructor.
  */
  virtual ~ENSIGHT_FIELD_DRIVER();

  /*!
    Set the name of the FIELD in EnSight file.

    It could be different than the name of the FIELD object.
  */
  virtual void setFieldName(const std::string & fieldName) throw (MEDEXCEPTION);

  /*!
    Get the name of the FIELD in EnSight file.
  */
  virtual std::string getFieldName() const { return _fieldName; }

  virtual void open() throw (MEDEXCEPTION) { openConst(); }

  virtual void close() {}

  FIELD_* getField() { return _ptrField; }

  void setConstantValue(const std::string& value) { _constantValue = value; }

  const std::string& getConstantValue() const { return _constantValue; }

};

// ==========================================================================================

class MEDMEM_EXPORT ENSIGHT_FIELD_RDONLY_DRIVER : public ENSIGHT_FIELD_DRIVER
{
public :
  
  /*!
    Constructor to read the field of a specified name and index-th time step.
    Append "_Im" to field name to select an imaginary part of a complex field
  */
  ENSIGHT_FIELD_RDONLY_DRIVER(const string & fileName, FIELD_ * ptrField, int step=1);

  ENSIGHT_FIELD_RDONLY_DRIVER();
  ENSIGHT_FIELD_RDONLY_DRIVER(const ENSIGHT_FIELD_RDONLY_DRIVER & fieldDriver);
  ~ENSIGHT_FIELD_RDONLY_DRIVER();
  virtual GENDRIVER * copy ( void ) const ;
  virtual void merge ( const GENDRIVER& driver );

  /*!
    Read a field
  */
  void read ( void ) throw (MEDEXCEPTION);

  /*!
    Return a MEDEXCEPTION : it is a read-only driver.
  */
  void write( void ) const throw (MEDEXCEPTION) ;

private:

  void readGoldASCII ();
  void readGoldBinary();
  void read6ASCII    ();
  void read6Binary   ();

  void skipTimeStamp(MEDMEM_ENSIGHT::_BinaryFileReader& fileReader);

  int _fieldStep;
};

// ==========================================================================================

class MEDMEM_EXPORT ENSIGHT_FIELD_WRONLY_DRIVER : public ENSIGHT_FIELD_DRIVER
{
public :
  
  /*!
    Constructor to append a field to the file
  */
  ENSIGHT_FIELD_WRONLY_DRIVER(const string & fileName, const FIELD_ * ptrField);

  ENSIGHT_FIELD_WRONLY_DRIVER();
  ENSIGHT_FIELD_WRONLY_DRIVER(const ENSIGHT_FIELD_WRONLY_DRIVER & fieldDriver);
  ~ENSIGHT_FIELD_WRONLY_DRIVER();
  GENDRIVER * copy ( void ) const ;

  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void ) throw (MEDEXCEPTION) ;

  /*!
    Write FIELD in the specified file, the mesh is supposed to be
    written in this file
  */
  void write( void ) const throw (MEDEXCEPTION) ;

};

}//End namespace MEDMEM

#endif /* ENSIGHT_FIELD_DRIVER_HXX */
