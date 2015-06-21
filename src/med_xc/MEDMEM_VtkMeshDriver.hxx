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

#ifndef VTK_MESH_DRIVER_HXX
#define VTK_MESH_DRIVER_HXX

#include <cstring>

#include "MEDMEM.hxx"

#include <string>
#include <vector>
#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Utilities.hxx"

#include <fstream>
#include <unistd.h>


/*!

  Driver Vtk for MESH (only for writing).

  Generic part : implement open and close methods.

*/
namespace MEDMEM {
class GMESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;
class MEDMEM_EXPORT _VTK_BinaryWriter;

class MEDMEM_EXPORT VTK_MESH_DRIVER : public GENDRIVER
{
protected:

  const GMESH * _ptrMesh;
  std::string   _meshName;
  mutable std::ofstream *    _vtkFile ;     // The _vtkFile used to write Meshes to _filename
  mutable _VTK_BinaryWriter* _binaryFile;

public :

  /*!
    Constructor.
  */
  VTK_MESH_DRIVER() ;
  /*!
    Constructor.
  */
  VTK_MESH_DRIVER(const std::string & fileName, const GMESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  VTK_MESH_DRIVER(const VTK_MESH_DRIVER & driver) ;

  /*!
    Destructor.
  */
  ~VTK_MESH_DRIVER() ;

  void open() ;
  void close() ;

  void openConst() const throw (MEDEXCEPTION);
  void closeConst() const throw (MEDEXCEPTION);

  void write( void ) const throw (MEDEXCEPTION) ;
  void read ( void ) throw (MEDEXCEPTION) ;

  /*!
    Set the name of the MESH asked in file.

    It could be different than the name of the MESH object.
  */
  void   setMeshName(const string & meshName) ;
  /*!
    Get the name of the MESH asked in file.
  */
  string getMeshName() const ;

private:
  GENDRIVER * copy ( void ) const;

  template <typename T>
  void writeBinary(const T* data, int nbValues) const throw (MEDEXCEPTION);
};

class MEDMEM_EXPORT _VTK_BinaryWriter
{
  std::string _fileName;
  int         _binaryFile;
public:
  _VTK_BinaryWriter(const std::string file);
  bool open(bool append=false) const;
  bool close() const;

  template <typename T>
  void write(const T* data, int nbValues) const throw (MEDEXCEPTION)
  {
    // DEBUG
//     if ( sizeof(T) == sizeof(char))
//       cout << data ;
//     else
//       for ( int i=0; i < nbValues;++i )
//         cout << data[i] << " ";
    const void* toWrite = (const void* ) data;
    T* swappedData = 0;
    if ( sizeof(T) != sizeof(char))
      {
        // inverse bytes
        toWrite = (const void* )( swappedData = new T[ nbValues ]);
        memcpy( swappedData, data, nbValues * sizeof(T));          
        int* intBuf = ((int*) swappedData) - 1;
        int* bufEnd = (int*)((char*) swappedData + nbValues * sizeof(T));
        while ( ++intBuf < bufEnd )
          *intBuf = swapBytes( *intBuf );
      }
    ssize_t nbWritten = ::write( _binaryFile, toWrite, nbValues * sizeof(T));
    if ( swappedData )
      delete [] swappedData;
    if ( nbWritten < 0 )
      throw MEDEXCEPTION(LOCALIZED(STRING("_VTK_BinaryWriter::Failed to write into ")<< _fileName));
  }
};

template <typename T>
void VTK_MESH_DRIVER::writeBinary(const T* data, int nbValues) const throw (MEDEXCEPTION)
{
  _binaryFile->write( data, nbValues );
}

}


#endif /* VTK_MESH_DRIVER_HXX */
