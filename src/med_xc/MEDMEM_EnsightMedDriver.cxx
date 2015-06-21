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

#include "MEDMEM_define.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"

#include "MEDMEM_EnsightMedDriver.hxx"
#include "MEDMEM_EnsightFieldDriver.hxx"
#include "MEDMEM_EnsightMeshDriver.hxx"

using namespace std;
//using namespace MEDMEM;
using namespace MED_EN;
using namespace MEDMEM_ENSIGHT;

// ================================================================================
// ENSIGHT_MED_DRIVER
// ================================================================================
namespace MEDMEM {

ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER():_CaseFileDriver_User()
{
}

ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER(const string & fileName,
                                       MED_EN::med_mode_acces accessMode):
  _CaseFileDriver_User(fileName,accessMode)
{
}

ENSIGHT_MED_DRIVER::ENSIGHT_MED_DRIVER(const ENSIGHT_MED_DRIVER & driver):
  _CaseFileDriver_User(driver)
{
}

void ENSIGHT_MED_DRIVER::openConst() const
{
  const char * LOC ="ENSIGHT_MED_DRIVER::open() : ";
  BEGIN_OF_MED(LOC);

  if ( _fileName.empty() )
    throw MED_EXCEPTION
      ( LOCALIZED( STRING(LOC) << "_fileName is empty, "
                   "please set a correct fileName before calling open()"));

  if (!canOpenFile( _fileName, getAccessMode() ))
    throw MED_EXCEPTION
      ( LOCALIZED( STRING(LOC) << "Can not open main Ensight file " << _fileName));

  END_OF_MED(LOC);
}

void ENSIGHT_MED_DRIVER::open()
{
  openConst();
}
void ENSIGHT_MED_DRIVER::close()
{
}

ENSIGHT_MED_DRIVER::~ENSIGHT_MED_DRIVER()
{
  MESSAGE_MED("ENSIGHT_MED_DRIVER::~ENSIGHT_MED_DRIVER() has been destroyed");
}

// ================================================================================
// WRONLY
// ================================================================================

ENSIGHT_MED_WRONLY_DRIVER::ENSIGHT_MED_WRONLY_DRIVER() : ENSIGHT_MED_DRIVER(), _fields(0)
{
}

ENSIGHT_MED_WRONLY_DRIVER::ENSIGHT_MED_WRONLY_DRIVER(const string & fileName,
                                                     const vector< const FIELD_* >& fields)
  : ENSIGHT_MED_DRIVER(fileName, MED_EN::WRONLY ), _fields( fields )
{
}

ENSIGHT_MED_WRONLY_DRIVER::ENSIGHT_MED_WRONLY_DRIVER(const ENSIGHT_MED_WRONLY_DRIVER & driver)
  : ENSIGHT_MED_DRIVER(driver), _fields( driver._fields )
{
}

ENSIGHT_MED_WRONLY_DRIVER::~ENSIGHT_MED_WRONLY_DRIVER()
{
}

GENDRIVER * ENSIGHT_MED_WRONLY_DRIVER::copy() const
{
  return new ENSIGHT_MED_WRONLY_DRIVER(*this) ;
}

void ENSIGHT_MED_WRONLY_DRIVER::read() throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("ENSIGHT_MED_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

void ENSIGHT_MED_WRONLY_DRIVER::write() const throw (MEDEXCEPTION)
{
  const char * LOC = "ENSIGHT_MED_WRONLY_DRIVER::write() : ";
  BEGIN_OF_MED(LOC);

  openConst(); // check if can open the case file

  _CaseFileDriver caseFile( getCaseFileName(), this );

  map< const GMESH*, vector< const FIELD_* > > mesh2fields;
  for (unsigned i = 0; i < _fields.size(); ++i )
    mesh2fields[ _fields[i]->getSupport()->getMesh() ].push_back( _fields[i] );

  // Create drivers for all meshes and fields
  // and add them to be written to Case file

  list< GENDRIVER* > drivers;
  ENSIGHT_MESH_WRONLY_DRIVER *  meshDriver;
  ENSIGHT_FIELD_WRONLY_DRIVER * fieldDriver;

  map< const GMESH*, vector< const FIELD_* > >::iterator m_ff = mesh2fields.begin();
  for (; m_ff != mesh2fields.end(); ++m_ff )
  {
    const GMESH * mesh = m_ff->first ;
    meshDriver = new ENSIGHT_MESH_WRONLY_DRIVER( _fileName, mesh );
    caseFile.addMesh( meshDriver );
    drivers.push_back( meshDriver );

    // all fields on this mesh
    const vector< const FIELD_*> & fields = m_ff->second;
    for (unsigned j=0; j<fields.size(); j++)
    {
      fieldDriver = new ENSIGHT_FIELD_WRONLY_DRIVER( _fileName, fields[j] );
      caseFile.addField( fieldDriver );
      drivers.push_back( fieldDriver );
    }
  }

  // Write

  // case file
  caseFile.write();// necessary data is passed to the drivers by this method

  // meshes and fields
  list< GENDRIVER* >::iterator drv = drivers.begin();
  for ( ; drv != drivers.end(); ++drv ) {
    GENDRIVER* driver = *drv;
    driver->write();
    delete driver;
  }

  END_OF_MED(LOC);
}

// ================================================================================
// RDONLY
// ================================================================================

ENSIGHT_MED_RDONLY_DRIVER::ENSIGHT_MED_RDONLY_DRIVER() :
  ENSIGHT_MED_DRIVER(), _fields(0), _isFileStructRead(false)
{
}

ENSIGHT_MED_RDONLY_DRIVER::ENSIGHT_MED_RDONLY_DRIVER(const string &     fileName,
                                                     vector< FIELD_* >& fields)
  : ENSIGHT_MED_DRIVER( fileName, MED_EN::RDONLY), _fields( & fields ), _isFileStructRead(false)
{
}

ENSIGHT_MED_RDONLY_DRIVER::ENSIGHT_MED_RDONLY_DRIVER(const ENSIGHT_MED_RDONLY_DRIVER & driver)
  : ENSIGHT_MED_DRIVER(driver),
    _fields( driver._fields ),
    _isFileStructRead(driver._isFileStructRead)
{
}

ENSIGHT_MED_RDONLY_DRIVER::~ENSIGHT_MED_RDONLY_DRIVER()
{
}

GENDRIVER * ENSIGHT_MED_RDONLY_DRIVER::copy() const
{
  return new ENSIGHT_MED_RDONLY_DRIVER(*this) ;
}

void ENSIGHT_MED_RDONLY_DRIVER::write() const throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("ENSIGHT_MED_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

void ENSIGHT_MED_RDONLY_DRIVER::read()
{
  const char * LOC = "ENSIGHT_MED_RDONLY_DRIVER::read() : " ;
  BEGIN_OF_MED(LOC);

  if ( _isFileStructRead )
  {
    for ( unsigned i = 0; i < _fields->size(); ++i )
    {
      const GMESH* mesh = _fields->at(i)->getSupport()->getMesh();
      if ( mesh->getNumberOfNodes() < 1 )
        const_cast<GMESH*>( mesh )->read( getId() );
      _fields->at(i)->read( getId() );
    }
  }
  else
  {
    open(); // check if can open the case file

    _CaseFileDriver caseFile( getCaseFileName(), this );

    caseFile.read();

    _fields->clear();
    int nbOfFields = caseFile.getNbVariables();
    if ( nbOfFields < 1 )
      return;

    int nbOfMeshes = caseFile.getNbMeshes();
    vector<MESH*> meshes;
    for ( int i = 1; i <= nbOfMeshes; ++i )
    {
      MESH* mesh = new MESH;
      ENSIGHT_MESH_RDONLY_DRIVER meshDriver(_fileName, mesh, i);
      caseFile.setDataFileName( i, &meshDriver );
      int drv = mesh->addDriver( meshDriver );
      mesh->read( drv );
      meshes.push_back( mesh );
    }

    for ( int i = 1; i <= nbOfFields; i++ )
    {
      int nbSteps = caseFile.getNbVarSteps( i );
      for ( int step = 1; step <= nbSteps; ++step )
      {
        FIELD_* field = new FIELD<double>();
        ENSIGHT_FIELD_RDONLY_DRIVER fieldDriver( _fileName, field );
        /*int meshIndex =*/ caseFile.setDataFileName( i, step, &fieldDriver );
        fieldDriver.open();
        fieldDriver.read();
        _fields->push_back( field );
      }
    }
  }
}

//================================================================================
/*!
 * \brief Create all meshes and fields but not read them
 */
//================================================================================

void ENSIGHT_MED_RDONLY_DRIVER::readFileStruct()
{
  const char * LOC = "ENSIGHT_MED_RDONLY_DRIVER::read() : " ;
  BEGIN_OF_MED(LOC);

  open(); // check if can open the case file

  _CaseFileDriver caseFile( getCaseFileName(), this );

  caseFile.read();

  int nbOfMeshes = caseFile.getNbMeshes();
  vector<MESH*> meshes( nbOfMeshes+1, (MESH*)0 );
  for ( int i = 1; i <= nbOfMeshes; ++i )
  {
    MESH* mesh = meshes[ i ] = new MESH;
    ENSIGHT_MESH_RDONLY_DRIVER meshDriver(_fileName, mesh, i);
    caseFile.setDataFileName( i, &meshDriver ); // retrieve mesh name
    setId( mesh->addDriver( meshDriver ));
  }
  _isFileStructRead = true;

  _fields->clear();
  int nbOfFields = caseFile.getNbVariables();
  for ( int i = 1; i <= nbOfFields; i++ )
  {
    int nbSteps = caseFile.getNbVarSteps( i );
    for ( int step = 1; step <= nbSteps; ++step )
    {
      FIELD<double>* field = new FIELD<double>();
      ENSIGHT_FIELD_RDONLY_DRIVER fieldDriver( _fileName, field, step );
      int meshIndex = caseFile.setDataFileName( i, step, &fieldDriver ); // retrieve field name
      field->addDriver( fieldDriver );
      if ( const SUPPORT* sup = field->getSupport() ) {
        if ( meshIndex > nbOfMeshes || !meshes[ meshIndex ])
          meshIndex = 1;
        ((SUPPORT*) sup)->setMesh( meshes[ meshIndex ]);
      }
      _fields->push_back( field );
    }
  }
}
}
