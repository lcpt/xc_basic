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

#ifndef VTK_FIELD_DRIVER_HXX
#define VTK_FIELD_DRIVER_HXX

#include <string>
#include <fstream>
#include <sstream>

#include "MEDMEM_define.hxx"

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_nArray.hxx"
#include "MEDMEM_ArrayConvert.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_VtkMeshDriver.hxx"

#ifdef WNT
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>

/*!

  Driver Med for FIELD.

  Generic part : implement open and close methods.

*/

namespace MEDMEM {
template <class T> class VTK_FIELD_DRIVER : public GENDRIVER
{
protected:
  
  const FIELD<T> *           _ptrField;
  std::string                _fieldName;
  int                        _fieldNum;

  mutable ofstream *         _vtkFile ;
  mutable _VTK_BinaryWriter* _binaryFile;

public :

  /*!
    Constructor.
  */
  VTK_FIELD_DRIVER():GENDRIVER(VTK_DRIVER),
                     _ptrField(0), _fieldName(""), _fieldNum(MED_EN::MED_INVALID),
                     _vtkFile(0), _binaryFile(0)
  {
    const char * LOC = "VTK_FIELD_DRIVER::VTK_FIELD_DRIVER() ";
    BEGIN_OF_MED(LOC);
    END_OF_MED(LOC);
  }
  /*!
    Constructor.
  */
  template <class INTERLACING_TAG>
  VTK_FIELD_DRIVER(const std::string &              fileName,
                   const FIELD<T, INTERLACING_TAG> * ptrField):
    GENDRIVER(fileName, MED_EN::WRONLY, VTK_DRIVER),
    _ptrField((const FIELD<T> *) ptrField), _fieldName(fileName),_fieldNum(MED_EN::MED_INVALID),
    _vtkFile(0), _binaryFile(0)
  {
    const char* LOC = "VTK_FIELD_DRIVER::VTK_FIELD_DRIVER(const string & fileName, FIELD<T> * ptrField) ";
    BEGIN_OF_MED(LOC);
    END_OF_MED(LOC);
  }

  /*!
    Copy constructor.
  */
  VTK_FIELD_DRIVER(const VTK_FIELD_DRIVER & fieldDriver):
    GENDRIVER(fieldDriver),
    _ptrField(fieldDriver._ptrField),
    _fieldName(fieldDriver._fieldName),
    _fieldNum(fieldDriver._fieldNum),
    _vtkFile(0), _binaryFile(0)
  {
  }

  /*!
    Destructor.
  */
  ~VTK_FIELD_DRIVER()
  {
    const char* LOC = "VTK_FIELD_DRIVER::~VTK_FIELD_DRIVER()";
    BEGIN_OF_MED(LOC);

    close();

    if ( _vtkFile )    delete _vtkFile ;
    if ( _binaryFile ) delete _binaryFile;

    _vtkFile = 0;
    _binaryFile = 0;

    END_OF_MED(LOC);
  }

  void openConst(bool append=false) const throw (MEDEXCEPTION)
  {
    const char * LOC = "VTK_FIELD_DRIVER::openConst()" ;
    BEGIN_OF_MED(LOC);

  if ( _fileName == "" )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "_fileName is |\"\"|, please set a correct fileName before calling open()"));

  if ( DRIVERFACTORY::getVtkBinaryFormatForWriting() )
    {
      if ( _vtkFile )
        {
          closeConst();
          delete _vtkFile;
          _vtkFile = 0;
        }
      if ( !_binaryFile )
        _binaryFile = new _VTK_BinaryWriter( _fileName );
      else
        _binaryFile->close();
      if (!_binaryFile->open(append))
        {
          delete _binaryFile;
          _binaryFile = 0;
          throw MED_EXCEPTION( LOCALIZED( STRING(LOC) << "Could not open file "<< _fileName));
        }
    }
  else
    {
      if ( _binaryFile )
        {
          _binaryFile->close();
          delete _binaryFile;
          _binaryFile = 0;
        }
 
      if (!_vtkFile )
        _vtkFile = new ofstream();
      else
        (*_vtkFile).close();

      if ( append )
        (*_vtkFile).open(_fileName.c_str(), ofstream::out | ofstream::app);
      else
        (*_vtkFile).open(_fileName.c_str()); 

      if (!(*_vtkFile))
        {
          delete _vtkFile;
          _vtkFile = 0;
          throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "Could not open file "<< _fileName));
        }
    }
    END_OF_MED(LOC);
  }

  void openConstAppend() const throw (MEDEXCEPTION)
  {
    openConst(true);
  }

  void open() throw (MEDEXCEPTION)
  {
    openConst() ;
  }

  void openAppend() throw (MEDEXCEPTION)
  {
    openConst(true) ;
  }

  void closeConst() const throw (MEDEXCEPTION)
  {
    const char * LOC = "VTK_FIELD_DRIVER::closeConst() " ;
    BEGIN_OF_MED(LOC);

    if ( _vtkFile )
      {
        if ((*_vtkFile).is_open())
          (*_vtkFile).close();

        if ( (*_vtkFile) && _vtkFile->is_open() )
          throw MED_EXCEPTION( LOCALIZED( STRING(LOC) << "Could not close file "<< _fileName));
      }
    if ( _binaryFile )
      {
        _binaryFile->close();
        delete _binaryFile;
        _binaryFile = 0;
      }

    END_OF_MED(LOC);
  }

  void close() {
    closeConst() ;
  }

  /*!
    Set the name of the FIELD asked in file.

    It could be different than the name of the FIELD object.
  */
  void   setFieldName(const string & fieldName) ;

  /*!
    Get the name of the FIELD asked in file.
  */
  string getFieldName() const ;

  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void ) throw (MEDEXCEPTION) ;

  /*!
    Write FIELD in the specified file, with its mesh through its support
    which has to be on all entities (excluding the faces in 3d and edges
    in 2d).
  */
  void write( void ) const throw (MEDEXCEPTION) ;

  /*!
    Write FIELD in the specified file, the mesh is supposed to be
    written in this file. The field support has to be on all entities
    (excluding the faces in 3d and edges in 2d).
  */
  void writeAppend( void ) const throw (MEDEXCEPTION);

private:
  GENDRIVER * copy ( void ) const ;

};

  /*-------------------------*/
  /* template implementation */
  /*-------------------------*/

/*--------------------- DRIVER PART -------------------------------*/

template <class T> void VTK_FIELD_DRIVER<T>::setFieldName(const string & fieldName)
{
  _fieldName = fieldName; 
}

template <class T> string  VTK_FIELD_DRIVER<T>::getFieldName() const
{
  return _fieldName;
}

template <class T> GENDRIVER * VTK_FIELD_DRIVER<T>::copy(void) const
{
  VTK_FIELD_DRIVER<T> * myDriver = new VTK_FIELD_DRIVER<T>(*this);

  return myDriver ;
}

template <class T> void VTK_FIELD_DRIVER<T>::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("VTK_FIELD_DRIVER::read : Can't read with a VTK driver because it is write only driver !");
}

//================================================================================
/*!
 * \brief Write
 */
//================================================================================

template <class T> void VTK_FIELD_DRIVER<T>::write(void) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "VTK_FIELD_DRIVER::write(void) const " ;
  BEGIN_OF_MED(LOC);

  // we get the Support and its associated Mesh

  const SUPPORT * supportField = _ptrField->getSupport();
  const GMESH * meshField = supportField->getMesh();
  if (! meshField )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": mesh was not read before writing")) ;

  // first, write mesh
  {
    VTK_MESH_DRIVER meshDriver( _fileName, meshField );
    meshDriver.write();
  }

  // write field
  writeAppend();
}

//================================================================================
/*!
 * \brief Write append
 */
//================================================================================

template <class T> void VTK_FIELD_DRIVER<T>::writeAppend(void) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "VTK_FIELD_DRIVER::writeAppend(void) const " ;
  BEGIN_OF_MED(LOC);

  // we get the Support and its associated Mesh

  const SUPPORT * supportField = _ptrField->getSupport();
  const GMESH * meshField = supportField->getMesh();
  MED_EN::medEntityMesh entitySupport = supportField->getEntity();

  if (! meshField )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": mesh was not read before writing")) ;

  if ( _ptrField->getGaussPresence() )
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which use Gauss Points !" << entitySupport));

  if (!(supportField->isOnAllElements()))
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which is not on all entities of the mesh !" << entitySupport));


  // Well we must open vtk file first, because there are
  // no other driver than MED for VTK that do it !
  openConstAppend() ;

  // first : field on node
  // fields is on all node !

  // second : field on cell
  // fields is on all cell !

  int dt = _ptrField->getIterationNumber();
  int it = _ptrField->getOrderNumber();

  ostringstream name ;
  string nameField = _ptrField->getName();
  name << nameField << "_" << dt << "_" << it ;

  // BEGIN issue 0020610: [CEA 371] VTK field driver : save many fields.
  // POINT_DATA and CELL_DATA must encounters once
  STRING dataStr;
  if (entitySupport == MED_EN::MED_NODE)
    dataStr << "POINT_DATA " << meshField->getNumberOfNodes() ;
  else if (entitySupport == MED_EN::MED_CELL)
    dataStr << "CELL_DATA " << meshField->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
  else
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" which is not on all nodes or cells but it's on !" << entitySupport));

  // check if dataStr is already present in the file
  bool toWriteDataStr = true;
#ifdef WNT
  int vtkFile = ::_open (_fileName.c_str(), _O_RDONLY|_O_BINARY);
#else
  int vtkFile = ::open (_fileName.c_str(), O_RDONLY);
#endif
  if ( vtkFile > 0 )
  {
#ifdef WNT
    ssize_t fileSize = ::_lseek( vtkFile, 0, SEEK_END); ::lseek( vtkFile, 0, SEEK_SET);
    char* buf = new char[ fileSize ];
    ::_read (vtkFile, buf, fileSize );
#else
    ssize_t fileSize = ::lseek( vtkFile, 0, SEEK_END); ::lseek( vtkFile, 0, SEEK_SET);
    char* buf = new char[ fileSize ];
    const int r= ::read (vtkFile, buf, fileSize );
    (void)(r); //LCPT Just to get rid of the warning.
#endif
    char *vtkData = buf, *vtkDataEnd = buf+fileSize-dataStr.size();
    while ( ++vtkData < vtkDataEnd && toWriteDataStr )
      toWriteDataStr = ( strncmp( dataStr.data(), vtkData, dataStr.size()) != 0 );
    delete [] buf;
#ifdef WNT
    ::_close (vtkFile);
#else
    ::close (vtkFile);
#endif
  }
  std::ostringstream vtkFileStr; // to collect labels
  if ( _binaryFile )
    vtkFileStr << endl;
  if ( toWriteDataStr )
    vtkFileStr << dataStr << endl;
  // END issue 0020610: [CEA 371] VTK field driver : save many fields

  int NomberOfValue = supportField->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS) ;
  int NomberOfComponents =  _ptrField->getNumberOfComponents() ;

  MED_EN::med_type_champ fieldType = _ptrField->getValueType() ;

  SCRUTE_MED(name.str());
  SCRUTE_MED(fieldType);

  std::string typeStr;
  switch (fieldType)
    {
    case MED_EN::MED_INT32 :
      {
        typeStr = " int"; break ;
      }
    case MED_EN::MED_REEL64 :
      {
        typeStr = " float"; break ;
      }
    default :
      {
        throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<< name.str() <<" the type is not int or double !"));
      }
    }

  if (NomberOfComponents==3)
    vtkFileStr << "VECTORS " << name.str() << typeStr << endl ;
  else if (NomberOfComponents<=4)
    {
      vtkFileStr << "SCALARS " << name.str() << typeStr << " " << NomberOfComponents << endl ;
      vtkFileStr << "LOOKUP_TABLE default" << endl ;
    }
  else
    throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << "Could not write field "<<_ptrField->getName()<<" there are more than 4 components !"));

  const T * value ;
  MEDMEM_Array_ * tmpArray = 0;
  if ( _ptrField->getInterlacingType() == MED_EN::MED_FULL_INTERLACE )
    {
      value = _ptrField->getValue();
    }
  else if ( _ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE )
    {
      MEDMEM_Array_ * ptrArray = _ptrField->getArray();
      MEDMEM_Array<T,NoInterlaceByTypeNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,NoInterlaceByTypeNoGaussPolicy> * >  ( ptrArray );
      MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * array = ArrayConvert2No( *temp );
      tmpArray = array;
      value = array->getPtr();
    }
  else
    {
      MEDMEM_Array_ * ptrArray = _ptrField->getArray();
      MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * temp = dynamic_cast<MEDMEM_Array<T,NoInterlaceNoGaussPolicy> * >  ( ptrArray );
      MEDMEM_Array<T,FullInterlaceNoGaussPolicy> * array = ArrayConvert( *temp );
      tmpArray = array;
      value = array->getPtr();
    }

  if ( _vtkFile ) // ASCII
    {
      (*_vtkFile) << vtkFileStr.str();
      for (int i=0; i<NomberOfValue; i++)
        {
          for(int j=0; j<NomberOfComponents; j++)
            (*_vtkFile) << value[j*NomberOfValue+i] << " " ;
          (*_vtkFile) << endl ;
        }
    }
  else // BINARY
    {
      std::string str = vtkFileStr.str();
      _binaryFile->write( str.data(), str.size() );
      // Though type "double" is available in VTK but actually it does not work
      // (at least paraview shows wrong values)
      if ( fieldType == MED_EN::MED_REEL64 )
        {
          vector<float> floatValue(NomberOfValue * NomberOfComponents );
          for ( unsigned i = 0; i < floatValue.size(); ++i )
            floatValue[i]=float( value[i] );
          _binaryFile->write( &floatValue[0], NomberOfValue * NomberOfComponents );
        }
      else
        {
          _binaryFile->write( value, NomberOfValue * NomberOfComponents );
        }
    }

  if ( _ptrField->getInterlacingType() != MED_EN::MED_FULL_INTERLACE )
    delete tmpArray;

  closeConst();

  END_OF_MED(LOC);
}
}//End namespace MEDMEM

#endif /* VTK_FIELD_DRIVER_HXX */
