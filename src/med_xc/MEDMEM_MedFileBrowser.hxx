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

# ifndef MEDFILEBROWSER_HXX
# define MEDFILEBROWSER_HXX

#include "MEDMEM.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Exception.hxx"

#include <string>
#include <map>
#include <vector>

namespace MEDMEM {

typedef struct { int dt; int it; } DT_IT_; //!< { numero du pas de temps, numero d'ordre }
struct MEDMEM_EXPORT LT_DT_IT_
{
  bool operator() (const DT_IT_ &p1, const DT_IT_ &p2) const
  {
    if ( p1.dt == p2.dt)
      return p1.it < p2.it ;
    else
      return  p1.dt < p2.dt ;
  }
};
typedef std::vector<DT_IT_> VEC_DT_IT_;

/*!
 * \brief Class giving access to names of meshes and fields contained in the med file
 */
class MEDMEM_EXPORT MEDFILEBROWSER
{
  std::string                          _fileName;

  std::map< std::string, bool >        _meshes; //!< mesh name and isStructured flag

  typedef struct
  {
    MED_EN::med_type_champ _type;
    std::string            _meshName;
    VEC_DT_IT_             _vec_dtit;
  } FIELD_DATA_;

  std::map< std::string, FIELD_DATA_ > _fields; //!< field name and its type and "dtit"

public:

  MEDFILEBROWSER();
  MEDFILEBROWSER (const std::string & fileName) throw (MEDEXCEPTION);
  void readFileStruct(const std::string & fileName) throw (MEDEXCEPTION);

  std::string  getFileName() const;

  int        getNumberOfMeshes ( void ) const;
  int        getNumberOfFields ( void ) const;

  void       getMeshNames      ( std::string * meshNames ) const;
  void       getFieldNames     ( std::string * fieldNames ) const;

  std::vector< std::string > getMeshNames () const;
  std::vector< std::string > getFieldNames() const;

  bool isStructuredMesh(const std::string & meshName) const throw (MEDEXCEPTION);

  MED_EN::med_type_champ getFieldType (const std::string & fieldName) const throw (MEDEXCEPTION) ;
  std::string             getMeshName (const std::string & fieldName) const throw (MEDEXCEPTION) ;
  VEC_DT_IT_        getFieldIteration (const std::string & fieldName) const throw (MEDEXCEPTION) ;
};

}

#endif
