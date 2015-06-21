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

#ifndef ENSIGHT_MESH_DRIVER_HXX
#define ENSIGHT_MESH_DRIVER_HXX

#include "MEDMEM_define.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_EnsightUtils.hxx"

#include <fstream>

namespace MEDMEM {

  class GMESH;
  class SUPPORT;

// ==============================================================================
/*!
  Driver Ensight for MESH ( writing and reading ).

  Generic part : implement open and close methods.
*/
// ==============================================================================

class MEDMEM_EXPORT ENSIGHT_MESH_DRIVER : public MEDMEM_ENSIGHT::_CaseFileDriver_User
{
protected:

  GMESH *      _ptrMesh;
  std::string  _meshName;

  virtual void openConst(bool checkDataFile=false) const;

public :
  ENSIGHT_MESH_DRIVER() ;
  ENSIGHT_MESH_DRIVER(const std::string & fileName,  GMESH * ptrMesh) ;
  ENSIGHT_MESH_DRIVER(const std::string & fileName,  GMESH * ptrMesh,
                      MED_EN::med_mode_acces accessMode);
  ENSIGHT_MESH_DRIVER(const ENSIGHT_MESH_DRIVER & driver) ;
  virtual ~ENSIGHT_MESH_DRIVER() ;

  void open();
  void close();

  virtual void        setMeshName(const string & meshName);
  virtual std::string getMeshName() const;

  GMESH* getMesh() { return _ptrMesh; }
};

// ==============================================================================
/*!
 * \brief Reading EnSight driver.
 */
// ==============================================================================
  
class MEDMEM_EXPORT ENSIGHT_MESH_RDONLY_DRIVER : public virtual ENSIGHT_MESH_DRIVER
{
public :
  //!< to read mesh of index-th time step
  ENSIGHT_MESH_RDONLY_DRIVER(const std::string & fileName, MESH * ptrMesh, int index=1);

  ENSIGHT_MESH_RDONLY_DRIVER();
  ENSIGHT_MESH_RDONLY_DRIVER(const ENSIGHT_MESH_RDONLY_DRIVER & driver);
  virtual ~ENSIGHT_MESH_RDONLY_DRIVER();
  virtual GENDRIVER * copy ( void ) const;
  virtual void merge ( const GENDRIVER& driver );
  virtual void write       ( void ) const throw (MEDEXCEPTION) ;
  virtual void read        ( void ) throw (MEDEXCEPTION);

  //!< count number of parts in EnSight geometry file 
  static int countParts(const std::string& geomFileName,
                        const bool         isSingleFileMode);

  static GROUP* makeGroup( MEDMEM::_groupe &           interGroup,
                           MEDMEM_ENSIGHT::_InterMed & imed);
                        
private:

  void readGoldASCII (MEDMEM_ENSIGHT::_InterMed & imed);
  void readGoldBinary(MEDMEM_ENSIGHT::_InterMed & imed);
  void read6ASCII    (MEDMEM_ENSIGHT::_InterMed & imed);
  void read6Binary   (MEDMEM_ENSIGHT::_InterMed & imed);

  static int countPartsBinary(MEDMEM_ENSIGHT::_BinaryFileReader& fileReader,
                              const bool                         isSingleFileMode);

  bool _isMadeByMed;
  int  _indexInCaseFile;
};

// ==============================================================================
/*!
 * \brief Writing EnSight mesh driver.
 *     To set writing format use
 *       setEnSightFormatForWriting(EnSightFormat) and
 *       setEnSightBinaryFormatForWriting(bool)
 */
// ==============================================================================

class MEDMEM_EXPORT ENSIGHT_MESH_WRONLY_DRIVER : public virtual ENSIGHT_MESH_DRIVER
{
public :
  //!< write a mesh;
  // to be appended, a mesh mush have same nb of groups and dimension as already present ones
  ENSIGHT_MESH_WRONLY_DRIVER(const std::string & fileName,
                             const GMESH * ptrMesh,
                             bool append=false);

  ENSIGHT_MESH_WRONLY_DRIVER();
  ENSIGHT_MESH_WRONLY_DRIVER(const ENSIGHT_MESH_WRONLY_DRIVER & driver);
  virtual ~ENSIGHT_MESH_WRONLY_DRIVER();
  GENDRIVER * copy ( void ) const;
  void write       ( void ) const throw (MEDEXCEPTION) ;
  void read        ( void ) throw (MEDEXCEPTION) ;

  int nbPartsToWrite() const;

private:

  void writePart6Binary   (MEDMEM_ENSIGHT::_BinaryFileWriter& file, const SUPPORT* support) const;
  void writePartGoldBinary(MEDMEM_ENSIGHT::_BinaryFileWriter& file, const SUPPORT* support) const;
  void writePart6ASCII    (std::ofstream&               fileStream, const SUPPORT* support) const;
  void writePartGoldASCII (std::ofstream&               fileStream, const SUPPORT* support) const;

  bool _append;
};

}

#endif /* ENSIGHT_MESH_DRIVER_HXX */

