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

#include "MEDMEM_MedFileBrowser.hxx"

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_MedVersion.hxx"

#include <set>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

namespace med_file
{
  /*!
   * \brief Structure to open a med file and to close it at destruction
   * (especially useful when we throw an exception)
   */
  struct MED_FILE
  {
    med_idt _id;
    MED_FILE(const string fileName)
    {
      _id = MEDfileOpen(fileName.c_str(),MED_ACC_RDONLY);
    }
    ~MED_FILE()
    {
      if ( _id > 0 )
	MEDfileClose(_id);
    }
    operator med_idt() const { return _id; }
  };
}
/*!

\defgroup MEDFILEBROWSER_constructors Initialization

\defgroup MEDFILEBROWSER_query Query methods
These methods enable the user to retrieve information
about a MED file structure, i.e. the meshes and fields it contains.

*/

/*! \if MEDMEM_ug 
\addtogroup MEDFILEBROWSER_constructors
@{
\endif
*/
//================================================================================
/*!
 * \brief Constructor of an empty MEDFILEBROWSER to be filled with \ref readFileStruct()
 */
//================================================================================

MEDFILEBROWSER::MEDFILEBROWSER()
{
}

//================================================================================
/*!
 * \brief This constructor constructs the MEDFILEBROWSER object from the file \a filename.
 * Names of meshes and fields are read.
 */
//================================================================================

MEDFILEBROWSER::MEDFILEBROWSER(const std::string & fileName) throw (MED_EXCEPTION)
{
  readFileStruct( fileName );
}

//================================================================================
/*!
 * \brief Read names of meshes and fields from the file \a fileName
 */
//================================================================================

void MEDFILEBROWSER::readFileStruct(const std::string & fileName) throw (MEDEXCEPTION)
{
  const char * LOC = "MEDFILEBROWSER::readFileStruct() : ";
  BEGIN_OF_MED(LOC);

  _fileName = fileName;
  _meshes.clear();
  _fields.clear();

  // =================
  // 1. Open the file
  // =================

  if ( _fileName.empty() )
    throw MED_EXCEPTION
      ( LOCALIZED( STRING(LOC) << "_fileName is |\"\"|, please provide a correct fileName"));

  MED_EN::medFileVersion version = getMedFileVersion(_fileName);
  if ( version == MED_EN::V21 )
    throw MED_EXCEPTION
      ( LOCALIZED( STRING(LOC) << _fileName << " is med-2.1 file which is of no longer supported version"));

  med_file::MED_FILE medIdt(_fileName);

  if (medIdt < 0)
    throw MED_EXCEPTION
      (LOCALIZED( STRING(LOC)<< "Can't open |"  << _fileName<< "|, medIdt : " << medIdt));

  int err,i,j;

  // =========================================
  // 2. Read number of meshes and their names
  // =========================================
  {
    char                  meshName[MED_NAME_SIZE+1]="";
    char                  meshDescription[MED_COMMENT_SIZE+1]="";
    med_int      meshDim;
    med_mesh_type meshType;

    int numberOfMeshes = MEDnMesh(medIdt) ;
    if ( numberOfMeshes <= 0 ) 
      MESSAGE_MED(LOC << "Be careful there is no mesh in file |"<<_fileName<<"| !");

    for (i=1;i<=numberOfMeshes;i++)
    {
      //get information on the i^th mesh
      int spaceDim;
      int naxis= MEDmeshnAxis(medIdt,i);
      med_axis_type axistype;
      med_sorting_type stype;
      char *axisname=new char[naxis*MED_SNAME_SIZE+1];
      char *axisunit=new char[naxis*MED_SNAME_SIZE+1];
      int nstep;
      char dtunit[MED_COMMENT_SIZE+1];
      err = MEDmeshInfo(medIdt, i, meshName, &spaceDim, &meshDim, &meshType, meshDescription, dtunit, &stype, &nstep, &axistype, axisname, axisunit) ;
      delete [] axisname;
      delete [] axisunit;
      if (err != MED_VALID)
        throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << ": can't get information about the mesh #" << i << " of the file |" << _fileName << "| !"));

      // Don't want to prevent the user from reading other correct meshes
//       if (meshType != MED_STRUCTURE &&
//           meshType != MED_NON_STRUCTURE )
//         throw MEDEXCEPTION
//           (LOCALIZED(STRING(LOC) << "Bad mesh type of mesh #"<<i <<" in file |"<<_fileName<<"|"));
      _meshes.insert( make_pair( string(meshName), meshType == MED_STRUCTURED_MESH ));
    }
  }

  // ===================================================================
  // 3. Read number of fields and their (timeStepNumber,iterationNumber)
  // ===================================================================
  {
    //    char                          fieldName[MED_NAME_SIZE+1] = "";
    char                          fieldName[MED_NAME_SIZE+1] ; // to avoid a crash if the field name is longer than MED_NAME_SIZE....
    char                          * componentName              = (char *) MED_NULL;
    char                          * unitName                   = (char *) MED_NULL;
    char                          meshName[MED_NAME_SIZE+1]  ;
    med_field_type       type;
    MESH_ENTITIES::const_iterator currentEntity; 
    list<MED_EN::medGeometryElement>::const_iterator currentGeometry;
    med_int              timeStepNumber               =  -1;
    char                          timeStepUnit[MED_LNAME_SIZE+1] ;
    double                        timeStep                     = 0.0;
    med_int              orderNumber                  =  -1;
    med_bool             meshLink;

    int numberOfFields = MEDnField(medIdt);

    for (i=1;i<=numberOfFields;i++)
    {
      int numberOfComponents = MEDfieldnComponent(medIdt,i) ;

      componentName = new char[numberOfComponents*MED_SNAME_SIZE+1] ;
      unitName      = new char[numberOfComponents*MED_SNAME_SIZE+1] ;
      int nstepsp3;
      err = MEDfieldInfo(medIdt, i, fieldName, meshName, &meshLink, &type, componentName, 
                                  unitName, timeStepUnit, &nstepsp3) ;
      delete[] componentName ;
      delete[] unitName ;

      MESSAGE_MED("Field #" << i << " is named " << fieldName);

      if (err != MED_VALID)
        throw MED_EXCEPTION(LOCALIZED(STRING(LOC) << ": can't get information about the field #" << i <<" of the file |" << _fileName << "| !"));

      std::map< string, FIELD_DATA_ >::iterator name2data=  _fields.insert( make_pair( fieldName, FIELD_DATA_() )).first;

      FIELD_DATA_& fieldData = name2data->second;
      fieldData._type = MED_EN::med_type_champ( type );

      // find all dtid of this field
      set<DT_IT_, LT_DT_IT_> set_dtit;

      for (j=1;j <= nstepsp3; j++)
        {
          err = MEDfieldComputingStepInfo(medIdt, fieldName, j, &timeStepNumber, &orderNumber, &timeStep);
          
          if (err == MED_VALID) // we have found for (*currentEntity).first and (*currentGeometry)
            {
              DT_IT_ dtIt;
              if (timeStepNumber<0)  timeStepNumber=-1 ;
              dtIt.dt  = timeStepNumber;
              dtIt.it  = orderNumber;
              
              set_dtit.insert( dtIt );
            }
        }
      fieldData._meshName = meshName;
      fieldData._vec_dtit.assign( set_dtit.begin(), set_dtit.end() );

    } // loop on numberOfFields
  }

  END_OF_MED(LOC);
}
/*! \if MEDMEM_ug @} \endif */

/*! \if MEDMEM_ug
\addtogroup MEDFILEBROWSER_query
@{
\endif
*/

//================================================================================
/*!
 * \brief Returns the name of a browsed file
 */
//================================================================================

std::string MEDFILEBROWSER::getFileName() const
{
  return _fileName;
}

//================================================================================
/*!
  \brief Gets the number of meshes in med file.
*/
//================================================================================

int MEDFILEBROWSER::getNumberOfMeshes ( void ) const
{
  return _meshes.size();
}   
    
//================================================================================
/*!
  \brief Gets the number of fields in med file.
*/
//================================================================================

int MEDFILEBROWSER::getNumberOfFields ( void ) const
{
  return _fields.size(); // we get number of field with different name
}       

//================================================================================
/*!
  \brief Gets the names of all meshes.

  meshNames is an in/out argument.

  It is a string array of size the
  number of meshes. It must be allocated before calling
  this method. All names are put in it.
*/
//================================================================================

void MEDFILEBROWSER::getMeshNames      ( std::string * meshNames ) const
{
  map< string, bool >::const_iterator name_isstruct = _meshes.begin();
  for ( int i=0; name_isstruct != _meshes.end(); ++i, ++name_isstruct )
    meshNames[i] = name_isstruct->first;
}

//================================================================================
/*!
  \brief Gets the names of all MESH objects.

  Returns a vector<string> object which contain the name of all MESH objects.
*/
//================================================================================

vector<string> MEDFILEBROWSER::getMeshNames      () const
{
  vector<string> meshNames( _meshes.size() );
  map< string, bool >::const_iterator name_isstruct = _meshes.begin();
  for ( int i=0; name_isstruct != _meshes.end(); ++i, ++name_isstruct )
    meshNames[i] = name_isstruct->first;
  return meshNames;
}

//================================================================================
/*!
  \brief Gets the names of all fields.

  fieldNames is an in/out argument.

  It is an array of string of size the
  number of fields. It must be allocated before calling
  this method. All names are put in it.
*/
//================================================================================

void MEDFILEBROWSER::getFieldNames     ( std::string * fieldNames ) const
{
  map< string, FIELD_DATA_ >::const_iterator name_data = _fields.begin();
  for( int i = 0; name_data != _fields.end(); ++name_data, ++i )
    fieldNames[i] = name_data->first;
}

//================================================================================
/*!
  \brief Gets the names of all fields.
*/
//================================================================================

vector<string> MEDFILEBROWSER::getFieldNames     () const
{
  vector<string> fieldNames( _fields.size() );
  map< string, FIELD_DATA_ >::const_iterator name_data = _fields.begin();
  for( int i = 0; name_data != _fields.end(); ++name_data, ++i )
    fieldNames[i] = name_data->first;
  return fieldNames;
}

//================================================================================
/*!
 * \brief Return true if mesh \a meshName is a structured one
 */
//================================================================================

bool MEDFILEBROWSER::isStructuredMesh(const std::string & meshName) const throw (MEDEXCEPTION)
{
  const char* LOC="MEDFILEBROWSER::isStructuredMesh(meshName)";

  map< std::string, bool >::const_iterator name_isstruct = _meshes.find(meshName);
  if ( name_isstruct == _meshes.end() )
    throw MEDEXCEPTION
      ( LOCALIZED( STRING(LOC)<< "There is no known mesh named |"<< meshName
                   << "| in file |" << _fileName <<"|"));

  return name_isstruct->second;
}

//================================================================================
/*!
 * \brief Return type of values of FIELD named \a fieldName 
 */
//================================================================================

MED_EN::med_type_champ MEDFILEBROWSER::getFieldType(const std::string & fieldName) const throw (MEDEXCEPTION)
{
  const char* LOC="MEDFILEBROWSER::getFieldIteration (fieldName)";

  std::map< std::string, FIELD_DATA_ >::const_iterator name_data = _fields.find(fieldName);

  if ( name_data == _fields.end() )
    throw MEDEXCEPTION
      ( LOCALIZED( STRING(LOC)<< "There is no known field named |"<< fieldName
                   << "| in file |" << _fileName <<"|"));

  return name_data->second._type;
}

//================================================================================
/*!
 * \brief Return name of mesh supporting the FIELD named \a fieldName 
 */
//================================================================================

std::string MEDFILEBROWSER::getMeshName (const std::string & fieldName) const throw (MEDEXCEPTION)
{
  const char* LOC="MEDFILEBROWSER::getMeshName (fieldName)";

  std::map< std::string, FIELD_DATA_ >::const_iterator name_data = _fields.find(fieldName);

  if ( name_data == _fields.end() )
    throw MEDEXCEPTION
      ( LOCALIZED( STRING(LOC)<< "There is no known field named |"<< fieldName
                   << "| in file |" << _fileName <<"|"));

  return name_data->second._meshName;
}

//================================================================================
/*!
  \brief Returns a vector<DT_IT_> which contain all iteration step for the FIELD 
  identified by its name.

  DT_IT_ definition is 
\verbatim
typedef struct { int dt; int it; } DT_IT_;
\endverbatim

\a dt represents the time iteration number, while \a it represents
the inner iteration number.
*/
//================================================================================

vector<DT_IT_> MEDFILEBROWSER::getFieldIteration (const std::string & fieldName) const
  throw (MED_EXCEPTION)
{
  const char* LOC="MEDFILEBROWSER::getFieldIteration (fieldName)";

  std::map< std::string, FIELD_DATA_ >::const_iterator name_data = _fields.find(fieldName);

  if ( name_data == _fields.end() )
    throw MEDEXCEPTION
      ( LOCALIZED( STRING(LOC)<< "There is no known field named |"<< fieldName
                   << "| in file |" << _fileName <<"|"));

  return name_data->second._vec_dtit;
}
/*!\if MEDMEM_ug @} \endif */
