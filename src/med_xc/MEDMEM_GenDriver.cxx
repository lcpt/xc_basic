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

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

GENDRIVER::GENDRIVER(driverTypes driverType):
  _id(MED_INVALID),
  _fileName(""),
  _accessMode( (MED_EN::med_mode_acces) MED_INVALID ),
  _status(MED_INVALID),
  _driverType(driverType) {}

GENDRIVER::GENDRIVER(const string & fileName,
                     MED_EN::med_mode_acces accessMode/*=(MED_EN::med_mode_acces) MED_INVALID*/,
                     driverTypes driverType)
  : _id(MED_INVALID),
    _fileName(fileName),
    _accessMode(accessMode),
    _status(MED_CLOSED),
    _driverType(driverType) 
{
}

GENDRIVER::GENDRIVER(const GENDRIVER & genDriver):
  //_id(MED_INVALID), 
  _id(genDriver._id),
  _fileName(genDriver._fileName),
  _accessMode(genDriver._accessMode),
  _status(genDriver._status),
  _driverType(genDriver._driverType) 
{}

GENDRIVER::~GENDRIVER() {}


GENDRIVER & MEDMEM::GENDRIVER::operator=(const GENDRIVER &  genDriver) 
{
  const char* LOC = " GENDRIVER & GENDRIVER::operator=(const GENDRIVER &  genDriver)  : ";
  BEGIN_OF_MED(LOC);
  _fileName    = genDriver._fileName;
  _accessMode  = genDriver._accessMode;
  _status      = genDriver._status;
  _id          = genDriver._id;
  _driverType  = genDriver._driverType;
  return *this;
}

void GENDRIVER::writeFrom      ( void ) const {}
void GENDRIVER::readFileStruct ( void ) {}

void   GENDRIVER::setMeshName   (const string & ) {}
string GENDRIVER::getMeshName()  const { return ""; }
void   GENDRIVER::setFieldName  (const string & ) {}
string GENDRIVER::getFieldName() const { return ""; }

void GENDRIVER::openAppend ( void ) {}
void GENDRIVER::writeAppend ( void ) const {}

void GENDRIVER::setId ( int id )
{
  const char* LOC = "void GENDRIVER::setId ( int id ) : ";
  BEGIN_OF_MED(LOC);

  if ( id >= 0 ) _id=id; else _id = MED_INVALID ;

  END_OF_MED(LOC);
}

int GENDRIVER::getId ( void) const
{
  const char* LOC = "int GENDRIVER::getId ( void) const ";
  BEGIN_OF_MED(LOC);

  return _id ;
}

string GENDRIVER::getFileName() const {

  const char* LOC = "string GENDRIVER::getFileName() const : ";
  BEGIN_OF_MED(LOC);
  
  return _fileName;
}
    

void GENDRIVER::setFileName(const string & fileName)
{
  const char * LOC = "void GENDRIVER::setFileName(const string & fileName) : ";
  BEGIN_OF_MED(LOC);

  if ( _status == MED_OPENED )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<" File |" << _fileName 
                                 << "| is still openned, close it before openning : | " << fileName << "|"));
  else
    _fileName = fileName; 

  END_OF_MED(LOC);
}

MED_EN::med_mode_acces GENDRIVER::getAccessMode() const
{
  const char* LOC = "med_mode_acces GENDRIVER::getAccessMode() const : ";
  BEGIN_OF_MED(LOC);

  return _accessMode;
}

void GENDRIVER::setAccessMode(MED_EN::med_mode_acces mode)
{
  _accessMode = mode;
}

ostream & MEDMEM::operator<<(ostream &os,const GENDRIVER & drv)
{
  switch (drv._accessMode)
    {
    case RDONLY : 
      os<<"C'est un IO de READ"<<endl;
      break;
    case RDWR :
      os<<"C'est un IO d'READ/WRITE"<<endl;
      break;
    case WRONLY :
      os<<"C'est un IO d'WRITE"<<endl;
      break;
    }
  switch (drv._status)
    {
    case MED_OPENED :
      os<<"L'IO_Mesh_MED est open"<<endl;
      break;
    case MED_CLOSED :
      os<<"L'IO_Mesh_MED est fermé"<<endl;
      break;
    case MED_INVALID :
      os<<"L'IO_Mesh_MED est non-valide"<<endl;
      break;
    }
  return os;
}

// Test if this driver has been created from  MED driver
bool MEDMEM::GENDRIVER::operator ==(const GENDRIVER &genDriver) const {
  

  MESSAGE_MED("bool GENDRIVER::operator ==(const GENDRIVER &genDriver) const : ");

  return /*( _id == genDriver._id )  &&*/
    ( _driverType == genDriver._driverType ) &&
    (_accessMode == genDriver._accessMode);
  
}

// Take missing data from other driver.
// Is for object->read( genDriver ) if object was not passed to genDriver,
// then object asks driverFactory to create a driver initialized by object
// and fills the new driver up using merge( genDriver )

void GENDRIVER::merge ( const GENDRIVER &genDriver )
{
  if ( _id == MED_INVALID )
    _id = genDriver._id;
  if ( _fileName.empty() )
    _fileName = genDriver._fileName;
  if ( _accessMode == MED_INVALID )
    _accessMode = genDriver._accessMode;

  if ( getMeshName().empty() )
    setMeshName( genDriver.getMeshName() );
  if ( getFieldName().empty() )
    setFieldName( genDriver.getFieldName() );
}
