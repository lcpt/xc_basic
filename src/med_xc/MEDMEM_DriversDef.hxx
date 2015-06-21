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

# ifndef DRIVERS_DEF_HXX
# define DRIVERS_DEF_HXX

#include "MEDMEM.hxx"

// STL
# include <string>
# include <list>
# include <map>

using namespace std ;

#include "MEDMEM_define.hxx"
#include "MEDMEM_Utilities.hxx"

namespace MED_EN {
/*!  This Class inherits from  map. \n
     It is a constant map association int (which is a med_geometrie_element) and string. \n
     Operator [] returns the (string) name of the geometry of an element 
     given by a med_geometrie_element value. \n 
     Such a static map is instancied and can be used in any code to have
     the name of the geometry of an element : MED_FR:GEO_NAME 
     A simple test/use case can be found in test/testUGeoNameMeshEntities.cxx  */
//----------------------------------------
class MEDMEM_EXPORT GEO_NAME : public map<int,string>
//----------------------------------------
{
  
public :
  
  GEO_NAME();
  ~GEO_NAME();
  string & operator[]( const MED_EN::medGeometryElement &c ) const;
} ;

static const GEO_NAME geoNames ;

//                              ____________________________    
  
/*! This Class inherits from map. \n
    It is a constant map association int (which is a med_entite_maillage) and string. \n
    Operator[] returns the (string) name of the type of an entity given by 
    a med_entite_maillage value. \n
    Such a static map is instancied and can be used in any code to have
    the name of the geometry of an element : MED_FR:ENT_NAME 
    A simple test/use case can be found in test/testUGeoNameMeshEntities.cxx  */

//----------------------------------------
class MEDMEM_EXPORT ENT_NAME : public map<int,string>
//----------------------------------------
{
  
public :

  ENT_NAME ();
  ~ENT_NAME();
  string & operator[]( const MED_EN::medEntityMesh &c ) const;
} ;

static const ENT_NAME entNames ;

//                              ____________________________    

/*! This Class inherits from map. \n
    It is a constant map association int (which is a med_entite_maillage) and  a list. \n
    Operator[] returns the list of all exisiting med_geometrie_element for 
    a med_entite_maillage value. \n
    Such a static map is instancied and can be used in any code to have
    the name of the geometry of an element : MED_FR:MESH_ENTITIES
    A simple test/use case can be found in test/testUGeoNameMeshEntities.cxx  */


//--------------------------------------------------------------------------------
class MEDMEM_EXPORT MESH_ENTITIES : public map<int, const list<MED_EN::medGeometryElement> >
//--------------------------------------------------------------------------------
{

public :

  MESH_ENTITIES ();
  ~MESH_ENTITIES();
  const list<MED_EN::medGeometryElement> & operator[]( const  MED_EN::medEntityMesh &c ) const;
  
private:
                                        /*! This operator is used to initialize class objects of 
                                            type  const list<med_geometrie_element> */
  list<MED_EN::medGeometryElement> & operator[]( const  MED_EN::medEntityMesh &c );
  
} ;

static const MESH_ENTITIES meshEntities ;

} // End Of NameSpace MED_EN

#endif

