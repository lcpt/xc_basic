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

#ifndef VTK_MED_DRIVER_HXX
#define VTK_MED_DRIVER_HXX

#include "MEDMEM.hxx"

#include <string>
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_Exception.hxx"

#include "MEDMEM_GenDriver.hxx"

#include <fstream>
#include <vector>

using namespace std ;


namespace MEDMEM {
class GMESH;
class SUPPORT;
class FIELD_;
class MED;
// This driver pilots within a VTK class read/write accesses of fields/meshes
class MEDMEM_EXPORT VTK_MED_DRIVER : public GENDRIVER
{
protected:

  const std::vector< const FIELD_* > _fields;
  
private:
  VTK_MED_DRIVER();

  void open()   ;
  void close()  ;
  void openConst()  const ;
  void closeConst() const ;

public :
  VTK_MED_DRIVER(const string & fileName, const std::vector< const FIELD_* >& fields);
  VTK_MED_DRIVER(const VTK_MED_DRIVER & driver);
  ~VTK_MED_DRIVER();
  // OPERATEUR DE RECOPIE AVEC _vtkFile ??

  //virtual void write          ( void )  ;
  void write                 ( void ) const ;
  virtual void read          ( void )  {} ;
  //  virtual void writeFrom      ( void ) const ;
  //  virtual void read           ( void ) ;
  GENDRIVER * copy (void ) const ;

private :
  void writeMesh(const GMESH * myMesh) const ;
  void writeField(const FIELD_ * myField, std::string name) const ;
//   void writeSupport(SUPPORT * mySupport) const ;

};
}


#endif /* VTK_MED_DRIVER_HXX */
