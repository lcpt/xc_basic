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

#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_MedVersion.hxx"

using namespace MED_EN;
using namespace MEDMEM;

// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor

MED_MESH_DRIVER::MED_MESH_DRIVER():
  GENDRIVER(MED_DRIVER),
  _ptrMesh(( GMESH *)MED_NULL),
  _meshName(""),
  _medIdt(MED_INVALID)
{
}

MED_MESH_DRIVER::MED_MESH_DRIVER(const std::string &    fileName,
                                 GMESH *                ptrMesh,
                                 MED_EN::med_mode_acces accessMode): 
  GENDRIVER(fileName, accessMode, MED_DRIVER),
  _ptrMesh(ptrMesh), 
  _meshName(""),
  _medIdt(MED_INVALID)
{
}

MED_MESH_DRIVER::MED_MESH_DRIVER(const MED_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  _meshName(driver._meshName),
  _medIdt(driver._medIdt)
{

}

MED_MESH_DRIVER::~MED_MESH_DRIVER()
{
  MESSAGE_MED("MED_MESH_DRIVER::~MED_MESH_DRIVER()has been destroyed");
}

void MED_MESH_DRIVER::setMeshName(const string & meshName) 
{ 
  _meshName = meshName; 
}

string  MED_MESH_DRIVER::getMeshName() const 
{ 
  return _meshName; 
}

void MED_MESH_DRIVER::open()
  {
    const char LOC[] = "MED_MESH_DRIVER::open()";
    BEGIN_OF_MED(LOC);

    int accessMode = getMedAccessMode( _accessMode );
    MESSAGE_MED(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< accessMode);
    _medIdt = MEDfileOpen(_fileName.c_str(),(med_access_mode) accessMode);
    MESSAGE_MED(LOC<<" _medIdt : "<< _medIdt );
    if(_medIdt > 0)
       _status = MED_OPENED;
    else
      {
        _medIdt = MED_INVALID;
        _status = MED_CLOSED;
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName<<" in mode "<<_accessMode));
      }
    END_OF_MED(LOC);
  }

void MED_MESH_DRIVER::close()
{
  const char * LOC = "MED_MESH_DRIVER::close() ";
  BEGIN_OF_MED(LOC);
  int err = 0;
  if( _status == MED_OPENED) {

    err = MEDfileClose(_medIdt);
    // san -- MED5873 : Calling H5close() here leads to failure of SALOMEDS::StudyManager_i::_SaveAs()
    // method during study saving process. MEDfermer() seems sufficient for closing a file.
    //H5close(); // If we call H5close() all the files are closed.
    if(err)
      throw MEDEXCEPTION( LOCALIZED(STRING(LOC)<<" Error when closing file ! " << err));

    MESSAGE_MED(LOC <<": _medIdt= " << _medIdt );
    MESSAGE_MED(LOC<<": MEDfermer : err    = " << err );
    _status = MED_CLOSED;
    _medIdt = MED_INVALID;
  }
  END_OF_MED(LOC);
}

//A FAIRE UTILISER LES MAPS...
const med_geometry_type  MED_MESH_DRIVER::all_cell_type[MED_N_CELL_GEO_FIXED_CON]=
  { MED_EN::MED_POINT1,MED_EN::MED_SEG2,MED_EN::MED_SEG3,MED_SEG4,MED_EN::MED_TRIA3,MED_EN::MED_QUAD4,MED_EN::MED_TRIA6,
    MED_TRIA7,MED_EN::MED_QUAD8,MED_QUAD9, MED_EN::MED_TETRA4,MED_EN::MED_PYRA5,MED_EN::MED_PENTA6,MED_EN::MED_HEXA8,
    MED_EN::MED_TETRA10,MED_OCTA12,MED_EN::MED_PYRA13, MED_EN::MED_PENTA15, MED_EN::MED_HEXA20,MED_HEXA27};

const char * const MED_MESH_DRIVER::all_cell_type_tab [MED_N_CELL_GEO_FIXED_CON]=
  { "MED_POINT1","MED_SEG2","MED_SEG3","MEDMEM_SEG4","MED_TRIA3","MED_QUAD4","MED_TRIA6","MEDMEM_TRIA7","MED_QUAD8","MEDMEM_QUAD9",
    "MED_TETRA4","MED_PYRA5","MED_PENTA6","MED_HEXA8","MED_TETRA10","MEDMEM_OCTA12","MED_PYRA13",
    "MED_PENTA15","MED_HEXA20","MEDMEM_HEXA27"};


//---------------------------------- RDONLY PART -------------------------------------------------------------

MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER():MED_MESH_DRIVER(),_computeFaces(true)
{
  this->GENDRIVER::_accessMode = MED_EN::RDONLY;
}

MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const string & fileName,
                                               GMESH *        ptrMesh):
  MED_MESH_DRIVER(fileName,ptrMesh,RDONLY),
  _computeFaces(true)
{
  MESSAGE_MED("MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

MED_MESH_RDONLY_DRIVER::MED_MESH_RDONLY_DRIVER(const MED_MESH_RDONLY_DRIVER & driver):
  MED_MESH_DRIVER(driver),
  _computeFaces(driver._computeFaces)
{
}

MED_MESH_RDONLY_DRIVER::~MED_MESH_RDONLY_DRIVER()
{
  //MESSAGE_MED("MED_MESH_RDONLY_DRIVER::~MED_MESH_RDONLY_DRIVER() has been destroyed");
}

GENDRIVER * MED_MESH_RDONLY_DRIVER::copy(void) const
{
  return new MED_MESH_RDONLY_DRIVER(*this);
}

void MED_MESH_RDONLY_DRIVER::merge ( const GENDRIVER& driver )
{
  MED_MESH_DRIVER::merge( driver );

  const MED_MESH_RDONLY_DRIVER* other =
    dynamic_cast< const MED_MESH_RDONLY_DRIVER* >( &driver );
  if( other ) {
    _computeFaces = other->_computeFaces;
  }
}

void MED_MESH_RDONLY_DRIVER::write( void ) const
{
  throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

void MED_MESH_RDONLY_DRIVER::read(void)
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER::read() : ";
  BEGIN_OF_MED(LOC);
  if(_status != MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName
                                 << " is : " << _medIdt << " (the file is not opened)."));

  if( ( _meshName.empty() ) && ( _ptrMesh->_name.empty() ) )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
                                 <<" neither <meshName> is set in driver nor in object MESH."));

  // If _meshName is not set in driver, try to use _ptrMesh->_name
  if( ( _meshName.empty() ) && ( !_ptrMesh->_name.empty() ) )
    _meshName = _ptrMesh->_name;

  if( _meshName.size() > MED_NAME_SIZE )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
                                 <<" <meshName> size in object driver MESH is > MED_TAILLE_NOM ."));

  _ptrMesh->_name = _meshName;

  // 0020058: Check version of med, which was used to save the file.
  // 0020058: An assertion happens in MEDcoordLire(), if this version
  // 0020058: is higher than the currently used version of med product.
  med_int aMajor, aMinor, aRelease;
  med_int aMajorCurr=3, aMinorCurr=1, aReleaseCurr=0;

  med_err aRet = MEDfileNumVersionRd(_medIdt, &aMajor, &aMinor, &aRelease);

  int aVersionHex     = (aMajor << 16 | aMinor << 8 | aRelease);
  int aVersionHexCurr = (aMajorCurr << 16 | aMinorCurr << 8 | aReleaseCurr);

  if(aRet != 0 || aVersionHex > aVersionHexCurr)
    {
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " cannot read file " << _fileName
                                 << " of version (" << aMajor << "." << aMinor << "." << aRelease
                                 << ") higher than the currently used version of med ("
                                 << aMajorCurr << "." << aMinorCurr << "." << aReleaseCurr << ")."));
    }
  // 0020058: end of version check

  SCRUTE_MED(_ptrMesh->getIsAGrid());

  int numberOfMeshes = MEDnMesh(_medIdt);
  if( numberOfMeshes < 1 )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " no meshes at all in file " << _fileName));

  // check mesh nature, unstructured or not (PAL14113)
  // and set space dimension
    {
      int naxis= MEDmeshnAxisByName(_medIdt,_meshName.c_str());
      if( naxis < 0 )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " no mesh |" << _meshName
                                   << "| in file " << _fileName));
      if( naxis == 0 )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Mesh |" << _meshName
                                   << "| has invalid space dimension 0."));

      med_int spaceDimension,meshDimension;
      med_mesh_type meshType;
      char commentp3[MED_COMMENT_SIZE+1];
      char dtunittp3[MED_LNAME_SIZE+1];
      med_sorting_type sttp3;
      int nstep;
      med_axis_type axtypp3;
      naxis = std::max(3, naxis); // safe enough?
      char *t1pp3=new char[naxis*MED_SNAME_SIZE+1];
      char *t2pp3=new char[naxis*MED_SNAME_SIZE+1];
      MEDmeshInfoByName(_medIdt,_meshName.c_str(),&spaceDimension,&meshDimension,&meshType,commentp3,dtunittp3,&sttp3,&nstep,&axtypp3,t1pp3,t2pp3);
      delete [] t1pp3;
      delete [] t2pp3;

      if((meshType == MED_STRUCTURED_MESH ) != _ptrMesh->getIsAGrid())
        {
          if( _ptrMesh->getIsAGrid() )
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Mesh type mismatch. "
                                       "Class MESH must be used for an unstructured mesh"));
          else
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Mesh type mismatch. "
                                       "Class GRID must be used for a structured mesh"));
        }
      if( meshDimension > spaceDimension )
        spaceDimension = meshDimension;

      if( spaceDimension < 1 )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The space dimension |" << spaceDimension
                                   << "| seems to be incorrect for the mesh : |" << _meshName << "|"));
      _ptrMesh->_spaceDimension = spaceDimension;
    
    }

  if(_ptrMesh->getIsAGrid())
    {
      getGRID();
      {
        if(getFAMILY() != MED_VALID)
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getFAMILY when the mesh is a grid"));
        buildAllGroups(_ptrMesh->_groupNode, _ptrMesh->_familyNode);
      }
      END_OF_MED(LOC);
      return;
    }

    if(getCOORDINATE() != MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getCOORDINATE"  ));

  if(getCONNECTIVITY() != MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getCOONECTIVITY"));

  if(getFAMILY() != MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getFAMILY"      ));

  if(_computeFaces)
    updateFamily();

  // we build all groups
  // on node
  buildAllGroups(_ptrMesh->_groupNode,_ptrMesh->_familyNode);
  // on cell
  buildAllGroups(_ptrMesh->_groupCell,_ptrMesh->_familyCell);

  if(_ptrMesh->getMeshDimension() == 3)
    // on face
    buildAllGroups(_ptrMesh->_groupFace,_ptrMesh->_familyFace);
//   else if(_ptrMesh->getMeshDimension() == 2) -- PAL13414
  if(_ptrMesh->getMeshDimension() > 1)
    // on edge
    buildAllGroups(_ptrMesh->_groupEdge,_ptrMesh->_familyEdge);

  _ptrMesh->_name = healName( _ptrMesh->_name );

  END_OF_MED(LOC);
}

//=======================================================================
//function : getGRID
//purpose  :
//=======================================================================

void MED_MESH_RDONLY_DRIVER::getGRID()
  {
    const char * LOC = "MED_MESH_RDONLY_DRIVER::getGRID() : ";
    BEGIN_OF_MED(LOC);

    if(_status!=MED_OPENED)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "med file is not opened"));

    GRID * ptrGrid = (GRID *) _ptrMesh;

    SCRUTE_MED(ptrGrid);

    int err, i;

    int numberOfMeshesInFile = MEDnMesh(_medIdt);

    if(numberOfMeshesInFile == MED_INVALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Problem in File where the mesh " << _meshName << " is supposed to be stored"));
    int MeshDimension= -1;
    int SpaceDimensionRead= 0;
    string tmp_nom_coord (MED_SNAME_SIZE*(_ptrMesh->_spaceDimension)+1,' ');
    string tmp_unit_coord(MED_SNAME_SIZE*(_ptrMesh->_spaceDimension)+1,' ');
    char * tmp_nom = (const_cast <char *> ( tmp_nom_coord.c_str())  );
    char * tmp_unit= (const_cast <char *> ( tmp_unit_coord.c_str()) );
    med_axis_type rep= MED_UNDEF_AXIS_TYPE;
    med_int dtp3,itp3;
    med_float ttpp3;
    for (int index = 0; index < numberOfMeshesInFile; index++)
      {
        char meshName[MED_NAME_SIZE+1]="";
        char meshDescription[MED_COMMENT_SIZE+1]="";
        med_int meshDim;
        med_mesh_type meshType;
        med_int spaceDimp3;
        char dtunittp3[MED_LNAME_SIZE+1];
        med_sorting_type stypp3;
        med_int nstepp3;
        int naxis= MEDmeshnAxis(_medIdt,(index+1));
        char *axisnamep3=new char[naxis*MED_SNAME_SIZE+1];
        char *axisunitp3=new char[naxis*MED_SNAME_SIZE+1];
        med_axis_type axtpp3;
        err = MEDmeshInfo(_medIdt,(index+1),meshName, &spaceDimp3, &meshDim,&meshType, meshDescription, dtunittp3,&stypp3,&nstepp3,&axtpp3,axisnamep3,axisunitp3);
        MESSAGE_MED(LOC<<": Mesh n°"<< (index+1) <<" nammed "<< meshName << " with the description " << meshDescription << " is structured");
        if(_meshName == string(meshName))
          {
            _ptrMesh->_description = meshDescription;
            _ptrMesh->_name = meshName;
            MeshDimension= meshDim;
            SpaceDimensionRead=spaceDimp3;
            rep=axtpp3;
            strncpy(tmp_nom,axisnamep3,naxis*_ptrMesh->_spaceDimension+1);
            strncpy(tmp_unit,axisunitp3,naxis*_ptrMesh->_spaceDimension+1);
	    MEDmeshComputationStepInfo(_medIdt,meshName,1,&dtp3,&itp3,&ttpp3);
          }
        delete [] axisnamep3;
        delete [] axisunitp3;
      }

    MED_EN::med_grid_type gridType = ptrGrid->getGridType();
    if( ptrGrid->_is_default_gridType )
    {
      ::med_grid_type type;
      MEDmeshGridTypeRd(_medIdt,_ptrMesh->_name.c_str(),&type);
      gridType = ptrGrid->_gridType = (MED_EN::med_grid_type) type;
      ptrGrid->_is_default_gridType = false;
    }

    MESSAGE_MED(LOC<<": Mesh processed is nammed "<< _ptrMesh->_name << " with the description " << _ptrMesh->_description << " is structured with the type " << gridType);


    if(MeshDimension == MED_INVALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The mesh dimension |" <<
                                   MeshDimension << "| seems to be incorrect " <<
                                   "for the mesh : |" << _meshName << "|"));

    // Read or get the dimension of the space for the mesh <_meshName>
    int SpaceDimension= MeshDimension;

    if(SpaceDimensionRead != MED_INVALID) SpaceDimension = SpaceDimensionRead;

    _ptrMesh->_spaceDimension = SpaceDimension;


    // Read Array length
    int * ArrayLen[] = { & ptrGrid->_iArrayLength,
                         & ptrGrid->_jArrayLength,
                         & ptrGrid->_kArrayLength  };

    MESSAGE_MED(LOC << "The mesh is a " << _ptrMesh->getMeshDimension() << "D mesh on a " << _ptrMesh->_spaceDimension << "D space");

    int NumberOfNodes;

    //  if(gridType == MED_EN::MED_GRILLE_STANDARD)
    if(gridType == MED_EN::MED_BODY_FITTED)
      {
        med_int * structure = new med_int[MeshDimension];

        err = MEDmeshGridStructRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,structure);

        if(err != MED_VALID)
          throw MEDEXCEPTION(STRING(LOC) <<"Error in reading the structure of grid : |" << _meshName << "|" );

        NumberOfNodes = 1;

        for (int idim = 0; idim < MeshDimension; idim++)
          {
            MESSAGE_MED(LOC<<"structure dim " << idim << " " << structure[idim]);

            ArrayLen [idim][0] = structure[idim];
            NumberOfNodes = NumberOfNodes*structure[idim];
          }

        delete [] structure;

        if( NumberOfNodes == MED_INVALID )
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The number of nodes |" <<
                                       NumberOfNodes <<
                                       "| seems to be incorrect "
                                       << "for the mesh : |" <<
                                       _meshName << "|" ));

        // create coordinates and its structure
        ptrGrid->_coordinate = new COORDINATE(SpaceDimension,NumberOfNodes,
                                              MED_EN::MED_FULL_INTERLACE);

        err = MEDmeshNodeCoordinateRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,::MED_FULL_INTERLACE,const_cast<double *>(ptrGrid->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE)));

        if(err != MED_VALID)
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't read coordinates of the |" <<
                                       NumberOfNodes << "| nodes for the mesh : |" <<
                                       _meshName << "| of space dimension |" <<
                                       SpaceDimension << "| with units names |" <<
                                       tmp_nom << "| and units |" <<
                                       tmp_unit << " |"));

      }
    else if((gridType == MED_EN::MED_CARTESIAN) ||
             (gridType == MED_POLAR))
      {
        NumberOfNodes = 1;

        double * Array[] = { (double*) 0, (double*) 0, (double*) 0 };

        for (int idim = 0; idim < _ptrMesh->getMeshDimension(); ++idim)
          {
	  med_data_type table= MED_UNDEF_DATATYPE;
            if(idim == 0)
              table = MED_COORDINATE_AXIS1;
            else if(idim == 1)
              table = MED_COORDINATE_AXIS2;
            else if(idim == 2)
              table = MED_COORDINATE_AXIS3;
            
            med_bool chgtpp3,trsfpp3;
            int length = MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,::MED_NODE,MED_EN::MED_NONE,table,MED_NO_CMODE,&chgtpp3,&trsfpp3);

            if( length <= MED_VALID )
              throw MEDEXCEPTION(STRING(LOC) <<"The number of nodes |" << length <<
                                 "| seems to be incorrect "
                                 << "for the mesh : |" << _meshName << "|" );

            ArrayLen [idim][0] = length;
            NumberOfNodes *= length;

            Array [idim] = new double [ length ];

            err = MEDmeshGridIndexCoordinateRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,idim+1,Array [idim]);

            if(err != MED_VALID)
              throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Error in reading coordinates indices " <<
                                           idim << "of the grid : |" <<
                                           _meshName << "|" ));
          }

        ptrGrid->_iArray = Array[0];
        ptrGrid->_jArray = Array[1];
        ptrGrid->_kArray = Array[2];

        // create coordinates
        ptrGrid->_coordinate = new COORDINATE(SpaceDimension,NumberOfNodes,
                                              MED_EN::MED_FULL_INTERLACE);

        if(gridType == MED_EN::MED_CARTESIAN)
          rep = ::MED_CARTESIAN;
        else if(gridType == MED_POLAR)
          {
            if(SpaceDimension == 2) rep = MED_CYLINDRICAL;
            else if(SpaceDimension == 3) rep = MED_SPHERICAL;
          }
      }
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<" bad grid type : " << gridType));

    // set coordinate names

    for (i=0; i<_ptrMesh->_spaceDimension; ++i )
    {
      string myStringName(tmp_nom_coord,i*MED_SNAME_SIZE,MED_SNAME_SIZE);
      string myStringUnit(tmp_unit_coord,i*MED_SNAME_SIZE,MED_SNAME_SIZE);
      // suppress space at the end
      int j;
      for(j=MED_SNAME_SIZE-1;j>=0;j--)
        if(myStringName[j] != ' ') break;
      ptrGrid->_coordinate->_coordinateName[i]=string(myStringName,0,j+1);
      for(j=MED_SNAME_SIZE-1;j>=0;j--)
        if(myStringUnit[j] != ' ') break;
      ptrGrid->_coordinate->_coordinateUnit[i]=string(myStringUnit,0,j+1);
    }

    string coordinateSystem = "UNDEFINED";

    if( rep == ::MED_CARTESIAN) coordinateSystem = "CARTESIAN";
    else if( rep == MED_CYLINDRICAL) coordinateSystem = "CYLINDRICAL";
    else if( rep == MED_SPHERICAL) coordinateSystem = "SPHERICAL";

    ptrGrid->_coordinate->setCoordinatesSystem(coordinateSystem);

    END_OF_MED(LOC);
}

//=======================================================================
//function : getCOORDINATE
// A FAIRE : RENVOYER DU VOID
//=======================================================================
int  MED_MESH_RDONLY_DRIVER::getCOORDINATE()
{
    const char * LOC = "MED_MESH_RDONLY_DRIVER::getCOORDINATE() : ";

    BEGIN_OF_MED(LOC);

    if(_status==MED_OPENED)
    {
      int err;
      int MeshDimension;
      int SpaceDimensionRead;
      med_mesh_type meshTypepp3;
      char meshDescription[MED_COMMENT_SIZE+1]="";
      char dtunit[MED_LNAME_SIZE+1];
      med_sorting_type sortTypepp3;
      int nstepp3;
      med_axis_type axtypepp3;
      //
      string tmp_nom_coord (MED_SNAME_SIZE*_ptrMesh->_spaceDimension+1,'\0');
      string tmp_unit_coord(MED_SNAME_SIZE*_ptrMesh->_spaceDimension+1,'\0');
      char * tmp_nom = (const_cast <char *> ( tmp_nom_coord.c_str())  );
      char * tmp_unit= (const_cast <char *> ( tmp_unit_coord.c_str()) );
      //
      err= MEDmeshInfoByName(_medIdt,_meshName.c_str(),
                                     &SpaceDimensionRead,&MeshDimension,
                                     &meshTypepp3,meshDescription,
                                     dtunit,&sortTypepp3,&nstepp3,
                                     &axtypepp3,tmp_nom,tmp_unit);
      int dtp3,itp3;
      med_float ttpp3;
      MEDmeshComputationStepInfo(_medIdt,_meshName.c_str(),1,&dtp3,&itp3,&ttpp3);

      MESH* ptrMesh = dynamic_cast<MESH*>(_ptrMesh);

      // Read the number of nodes used in the mesh <_meshName>
      // to be able to create a COORDINATE object
      med_bool chgtpp3,trsfpp3;
      int NumberOfNodes = MEDmeshnEntity(_medIdt,_meshName.c_str(),
				       dtp3,itp3,::MED_NODE,MED_EN::MED_NONE,
						MED_COORDINATE,
                                                  MED_NO_CMODE,
                                                  &chgtpp3,&trsfpp3);
      if( NumberOfNodes <= MED_VALID )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The number of nodes |" << NumberOfNodes
                                     << "| seems to be incorrect for the mesh : |" << _meshName << "|" ));
      ptrMesh->_numberOfNodes = NumberOfNodes;

      // create a COORDINATE object
      if(ptrMesh->_coordinate)
        delete ptrMesh->_coordinate;
      ptrMesh->_coordinate = new COORDINATE(ptrMesh->_spaceDimension, NumberOfNodes, MED_EN::MED_FULL_INTERLACE);

      med_axis_type rep=axtypepp3; // ATTENTION ---> DOIT ETRE INTEGRE DS MESH EF: FAIT NON?

      err= MEDmeshNodeCoordinateRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,::MED_FULL_INTERLACE,const_cast<double *>(ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE)));

      if(err != MED_VALID)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't read coordinates of the |" << NumberOfNodes
                                     << "| nodes for the mesh : |" << _meshName
                                     << "| of space dimension |" << ptrMesh->_spaceDimension
                                     << "| with units names |"   << tmp_nom
                                     << "| and units |"          << tmp_unit
                                     << " |"));

      for (int i=0;i<_ptrMesh->_spaceDimension;i++) {
        string myStringName(tmp_nom_coord,i*MED_SNAME_SIZE,MED_SNAME_SIZE);
        string myStringUnit(tmp_unit_coord,i*MED_SNAME_SIZE,MED_SNAME_SIZE);
        // suppress space at the end
        int j;
        for(j=MED_SNAME_SIZE-1;j>=0;j--)
          if(myStringName[j] != ' ') break;
        ptrMesh->_coordinate->_coordinateName[i]=string(myStringName,0,j+1);
        for(j=MED_SNAME_SIZE-1;j>=0;j--)
          if(myStringUnit[j] != ' ') break;
        ptrMesh->_coordinate->_coordinateUnit[i]=string(myStringUnit,0,j+1);
      }

      // Pourquoi le stocker sous forme de chaîne ?
      switch (rep)
      {
      case ::MED_CARTESIAN :
        {
          ptrMesh->_coordinate->_coordinateSystem = "CARTESIAN";
          break;
        }
      case MED_CYLINDRICAL :
        {
          ptrMesh->_coordinate->_coordinateSystem = "CYLINDRICAL";
          break;
        }
      case MED_SPHERICAL  :
        {
          ptrMesh->_coordinate->_coordinateSystem = "SPHERICAL";
          break;
        }
      default :
        {
          ptrMesh->_coordinate->_coordinateSystem = "UNDEFINED"; // ?Erreur ?
          break;
        }
      }

      // Read the unused optional node Names
      if(MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,::MED_NODE,MED_EN::MED_NONE,MED_NAME,MED_NO_CMODE,&chgtpp3,&trsfpp3)>0)
        MESSAGE_MED(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : WARNING : Nodes have names but we do not read them !");

      // ??? Read the unused optional node Numbers ???
      med_int * tmp_node_number = new med_int[NumberOfNodes];
      if(MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,::MED_NODE,MED_EN::MED_NONE,MED_NUMBER,MED_NO_CMODE,&chgtpp3,&trsfpp3)>0)
        {
          err= MEDmeshEntityNumberRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,::MED_NODE,MED_EN::MED_NONE,tmp_node_number);

          MESSAGE_MED(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : Nodes have numbers, we DO TAKE care of them !");
          ptrMesh->_coordinate->_nodeNumber.set(NumberOfNodes);
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
          for(med_int i2=0;i2<NumberOfNodes;i2++)
            ptrMesh->_coordinate->_nodeNumber[i2]=(int)(tmp_node_number[i2]);
#else
          memcpy((int*)ptrMesh->_coordinate->_nodeNumber,tmp_node_number,sizeof(int)*NumberOfNodes);
#endif

        //////////////////////////////////////////////////////////////////////////////////////
        ///  Modification pour prise en compte de la numérotation optionnelle des noeuds  ///
        //////////////////////////////////////////////////////////////////////////////////////
        ///
        /// Calcule _optionnalToCanonicNodesNumbers de telle sorte que _optionnalToCanonicNodesNumbers[OptionnalNumber]==CanonicNumber

        //        ptrMesh->_arePresentOptionnalNodesNumbers=1;
        //        for (int canonicNumber=1;canonicNumber<=NumberOfNodes;canonicNumber++) ptrMesh->_optionnalToCanonicNodesNumbers[tmp_node_number[canonicNumber-1]]=canonicNumber;
        // ICI RETOUR A LA NORMALE::: AUCUNE PRISE EN COMPTE D'UN NUMEROTATION OPTIONNEL
        ptrMesh->_arePresentOptionnalNodesNumbers=0;
      }
      else ptrMesh->_arePresentOptionnalNodesNumbers=0;

      //////////////////////////////////////////////////////////////////////////////////////

      delete[] tmp_node_number;

      END_OF_MED(LOC);
      return MED_VALID;
    }
    return MED_ERROR;
}


int MED_MESH_RDONLY_DRIVER::getCONNECTIVITY()
{
    const char * LOC = "MED_MESH_RDONLY_DRIVER::getCONNECTIVITY : ";
    BEGIN_OF_MED(LOC);

    if(_status==MED_OPENED)
      {

        int err = 0;
        // read MED_EN::MED_CELL connectivity
        CONNECTIVITY * Connectivity     = new CONNECTIVITY(MED_EN::MED_CELL);
        Connectivity->_numberOfNodes    = _ptrMesh->getNumberOfNodes();   // EF : Pourquoi cet attribut est-il dans MESH et non dans COORDINATE ?

        // Try to read nodal connectivity of the cells <Connectivity->_nodal>
        // then try to read descending connectivity    <Connectivity->_descending>
        // if neither nodal nor descending connectivity exists
        // throw an exception.
        err = getNodalConnectivity(Connectivity);
        if(err!=MED_VALID)
          {
            Connectivity->_typeConnectivity= MED_EN::MED_DESCENDING;
            err = getDescendingConnectivity(Connectivity);
          }
        else
          getDescendingConnectivity(Connectivity); // we read it if there is one

        if(err!=MED_VALID)
          {
            delete Connectivity;
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "We could not read " <<
                                         "any Connectivity"));
          }

        // commented since _ptrMesh->getMeshDimension() is based on connectivity
        // which is not yet set
//       if(Connectivity->_entityDimension != _ptrMesh->getMeshDimension())
//         MESSAGE_MED(LOC << "Small mesh dimension problem on the med file mounted in memory : diim stored " << _ptrMesh->getMeshDimension() << " dim computed using the connectivity " << Connectivity->_entityDimension);

        // At this point Connectivity->_typeConnectivity is either NODAL or DESCENDING
        // If both connectivities are found Connectivity->_typeConnectivity is NODAL
        // If space dimension is 3
        // try to read the nodal connectivity of the faces <ConnectivityFace->_nodal> then
        // try to read the descending connectivity <ConnectivityFace->_descending>
        // if there is no descending connectivity and the CELLS are
        // defined in descending mode then throw an exception

        // PROVISOIRE : if we have some face or edge in MED_MAILLE, we don't read more. There could not be have face or edge !!!!

        if(Connectivity->_constituent==NULL) {

        if(Connectivity->_entityDimension == 3) {
          MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DES FACES..." );
          CONNECTIVITY * ConnectivityFace = new CONNECTIVITY(MED_EN::MED_FACE);
          ConnectivityFace->_numberOfNodes    = _ptrMesh->getNumberOfNodes();
          ConnectivityFace->_typeConnectivity = Connectivity->_typeConnectivity; // NODAL or DESCENDING
          SCRUTE_MED(ConnectivityFace->_typeConnectivity);
          if(Connectivity->_typeConnectivity == MED_EN::MED_DESCENDING) {
            MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DESCENDANTE DES FACES" );
            err = getDescendingConnectivity(ConnectivityFace);
            if(err!=MED_VALID)
              throw MEDEXCEPTION ( LOCALIZED(STRING(LOC) << "No FACE in descending connectivity"));
            getNodalConnectivity(ConnectivityFace); // if any !
          } else {
            MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE NODALE DES FACES" );
            err = getNodalConnectivity(ConnectivityFace);
            if(err!=MED_VALID) { // or error ????? we are in NODAL mode.
              err = getDescendingConnectivity(ConnectivityFace);
            } else
              getDescendingConnectivity(ConnectivityFace); // if any !
          }
          if(err!=MED_VALID) {
            delete ConnectivityFace;
            MESSAGE_MED(LOC<<"No FACE defined.");
          } else {
            MESSAGE_MED(LOC<<" SAUVEGARDE DE LA CONNECTIVITE DES FACES DANS L'OBJET CONNECTIVITY" );
            delete Connectivity->_constituent;
            Connectivity->_constituent=ConnectivityFace;
          }
        }

        // read MED_EN::MED_EDGE connectivity
        if(Connectivity->_entityDimension > 1) { // we are in 3 or 2D
          MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DES ARRETES...." );
          CONNECTIVITY * ConnectivityEdge = new CONNECTIVITY(MED_EN::MED_EDGE);
          ConnectivityEdge->_numberOfNodes    = _ptrMesh->getNumberOfNodes();
          ConnectivityEdge->_typeConnectivity = Connectivity->_typeConnectivity;
          if(Connectivity->_typeConnectivity == MED_EN::MED_DESCENDING) {
            MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DESCENDANTE DES ARRETES" );
            err = getDescendingConnectivity(ConnectivityEdge);
            if(err!=MED_VALID)
              throw MEDEXCEPTION ( LOCALIZED(STRING(LOC) << "No EDGE in descending connectivity"));
            getNodalConnectivity(ConnectivityEdge); // if any !
          } else {
            MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE NODALE DES ARRETES" );
            err = getNodalConnectivity(ConnectivityEdge);
            if(err!=MED_VALID) { // or error ????? we are in NODAL mode.
              err = getDescendingConnectivity(ConnectivityEdge);
            } else
              getDescendingConnectivity(ConnectivityEdge); // if any !
          }
          if(err!=MED_VALID) {
            delete ConnectivityEdge;
            MESSAGE_MED(LOC<<"No EDGE defined.");
          } else {
            if(Connectivity->_entityDimension == 3)
              if(Connectivity->_constituent != NULL)
                Connectivity->_constituent->_constituent=ConnectivityEdge;
              else
                throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<< "EDGE defined but there are no FACE !"));
            else { // IN 2D
              MESSAGE_MED(LOC<<" SAUVEGARDE DE LA CONNECTIVITE DES ARETES DANS L'OBJET CONNECTIVITY" );
              Connectivity->_constituent=ConnectivityEdge;
            }
          }
        }
        }
        MESH* ptrMesh = dynamic_cast<MESH*>(_ptrMesh);
        if(ptrMesh->_connectivity)
          delete ptrMesh->_connectivity;
        ptrMesh->_connectivity  = Connectivity;


        END_OF_MED(LOC);
        return MED_VALID;
      }
    return MED_ERROR;
}

int MED_MESH_RDONLY_DRIVER::getNodalConnectivity(CONNECTIVITY * Connectivity)
{
    const char * LOC = "MED_MESH_RDONLY_DRIVER::getNodalConnectivity : ";
    BEGIN_OF_MED(LOC);
    med_bool chgtp3,trfp3;
    if(_status==MED_OPENED)
      {
        int dtp3,itp3;
        med_float ttpp3;
        MEDmeshComputationStepInfo(_medIdt,_meshName.c_str(),1,&dtp3,&itp3,&ttpp3);
        // Get the type of entity to work on (previously set in the Connectivity Object)
        med_entity_type Entity = (med_entity_type) Connectivity->getEntity();

        // Get the number of cells of each type & store it in <tmp_cells_count>.
        vector<int> tmp_cells_count;
        vector<CELLMODEL> tmp_cell_models; // models of present types
        int i;
        const list<MED_EN::medGeometryElement>& all_cell_type = meshEntities[ Connectivity->getEntity() ];
        list<MED_EN::medGeometryElement>::const_iterator type_iter;
        for ( type_iter = all_cell_type.begin(); type_iter != all_cell_type.end(); ++type_iter )
          {
            int nb_cells;
            if( med_geometry_type(*type_iter) != MED_POLYGON &&
                 med_geometry_type(*type_iter) != MED_POLYHEDRON )
              nb_cells= MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),
                                               dtp3,itp3,
				     ::MED_CELL,
                                               med_geometry_type(*type_iter),
				     MED_CONNECTIVITY,::MED_NODAL,
                                               &chgtp3,&trfp3);
            else if( med_geometry_type(*type_iter) == MED_POLYGON )
              nb_cells= MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),
                                               dtp3,itp3,
				     ::MED_CELL,
                                               MED_POLYGON,
						 MED_INDEX_NODE,
				     ::MED_NODAL,
                                               &chgtp3,&trfp3) - 1;
            else
              nb_cells= MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),
                                               dtp3,itp3,
				     ::MED_CELL,
                                               MED_POLYHEDRON,
                                               MED_INDEX_FACE,
				     ::MED_NODAL,
                                               &chgtp3,&trfp3)-1;
            /*MED_CONN,Entity,
              med_geometry_type(*type_iter),MED_NOD);*/

            // Get the greatest dimension of the cells : Connectivity->_entityDimension
            // We suppose there is no cells used as faces in MED 2.2.x , this is forbidden !!!
            // In version prior to 2.2.x, it is possible
            if(nb_cells>0)
              {
                tmp_cells_count.push_back(nb_cells);
                tmp_cell_models.push_back( CELLMODEL_Map::retrieveCellModel( *type_iter ));
                Connectivity->_entityDimension=tmp_cell_models.back().getDimension();
                Connectivity->_numberOfTypes++;
              }
          }

        if(Connectivity->_numberOfTypes > 0)
          {
            // if MED version < 2.2.x, we read only entity with
          // dimention = Connectivity->_entityDimension. Lesser dimension are face or edge !

          med_int major, minor, release;

          if( MEDfileNumVersionRd(_medIdt, &major, &minor, &release) != 0 )
            // error : we suppose we have not a good med file !
            return MED_ERROR;

          // we get MED version number
          // If MED version is < 2.2 then the cells which dimension
          // is lesser than the main dimension ( Connectivity->_entityDimension )
          // are either faces or edges

          vector<int> tmpEdgeCount, tmpFaceCount;
          vector<MED_EN::medGeometryElement> edgeTypes, faceTypes;
          if(Entity==MED_EN::MED_CELL)
          {
            Connectivity->_numberOfTypes=0;

            for ( i=0;i<int(tmp_cell_models.size());i++)
            {
              int dimension = tmp_cell_models[i].getDimension();
              if(Connectivity->_entityDimension == dimension)
                Connectivity->_numberOfTypes++;
              if(Connectivity->_entityDimension > dimension)
              {
                if(dimension == 2 )
                {
                  faceTypes.push_back( tmp_cell_models[i].getType() );
                  tmpFaceCount.push_back( tmp_cells_count[i] );
                  tmp_cells_count[i] = 0;
                }
                else if(dimension == 1 )
                  {
                    edgeTypes.push_back( tmp_cell_models[i].getType() );
                    tmpEdgeCount.push_back( tmp_cells_count[i] );
                    tmp_cells_count[i] = 0;
                  }
                else if(dimension == 0 )
                  {
                    tmp_cells_count[i] = 0;
                  }
              }
            }
          }

          // Retrieve connectivity size of poly elements
          med_int polygonConnSize, polyhedraFacesIndexSize=0, polyhedraConnSize;
          int polyhedraConnSizeWithFaceSeparators;
          if( tmp_cell_models.back().getType() == MED_EN::MED_POLYGON ||
               (!faceTypes.empty() && faceTypes.back() == MED_EN::MED_POLYGON ))
            {
              polygonConnSize= MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),
                                                        dtp3,itp3,
					    ::MED_CELL,
                                                        MED_POLYGON,
                                                        MED_CONNECTIVITY,
					    ::MED_NODAL,
                                                        &chgtp3,&trfp3);
            }
          if( tmp_cell_models.back().getType() == MED_EN::MED_POLYHEDRA )
            {
              polyhedraFacesIndexSize= MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),
                                                     dtp3,itp3,
						    ::MED_CELL,
                                                     MED_POLYHEDRON,
                                                     MED_INDEX_NODE,
						    ::MED_NODAL,
                                                     &chgtp3,&trfp3);
              polyhedraConnSize= MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),
                                               dtp3,itp3,
					      ::MED_CELL,
                                               MED_POLYHEDRON,
                                               MED_CONNECTIVITY,
					      ::MED_NODAL,
                                               &chgtp3,&trfp3);

              int nbPolyherda = tmp_cells_count.back();
              int nbFaces     = polyhedraFacesIndexSize - 1;
              // connectivity of each but last face of each polyhedron ends with -1
              polyhedraConnSizeWithFaceSeparators = polyhedraConnSize + nbFaces - nbPolyherda;
            }

          // bloc to read CELL :
          {
            // Prepare an array of indexes on the different cell types to create a MEDSKYLINEARRAY
            // We use <tmp_cells_count> to calculate <Connectivity->_count> then we release it
            Connectivity->_geometricTypes = new MED_EN::medGeometryElement [Connectivity->_numberOfTypes]; // Double emploi pour des raisons pratiques
            Connectivity->_type           = new CELLMODEL                  [Connectivity->_numberOfTypes]; //
            if(Connectivity->_count) delete [] Connectivity->_count;
            Connectivity->_count          = new int                        [Connectivity->_numberOfTypes+1];
            Connectivity->_count[0]       = 1;

            int size = 0;
            int typeNumber=1;
            vector<int> connSizeByType;
            for ( i=0; i < (int)tmp_cells_count.size(); i++)
            { // no point1 cell type (?)
              if( !tmp_cells_count[i] ) continue; // faces or edges

              Connectivity->_count[typeNumber]=Connectivity->_count[typeNumber-1]+tmp_cells_count[i];

              Connectivity->_type[typeNumber-1] = tmp_cell_models[i];

              Connectivity->_geometricTypes[typeNumber-1] = tmp_cell_models[i].getType();

              // probleme avec les mailles de dimension < a dimension du maillage :
              // Il faut oter le zero a la lecture est le remettre a l'ecriture : ce n'est pas fait !!!!!
              // On interdit ce cas pour l'instant !!!

              switch (tmp_cell_models[i].getType() )
              {
              case MED_EN::MED_POLYGON:
                connSizeByType.push_back( polygonConnSize ); break;
              case MED_EN::MED_POLYHEDRA:
                connSizeByType.push_back( polyhedraConnSizeWithFaceSeparators ); break;
              default:
                connSizeByType.push_back
                  (tmp_cells_count[i] * tmp_cell_models[i].getNumberOfNodes() );
              }
              size += connSizeByType.back();
              typeNumber++;
              MESSAGE_MED(LOC << Connectivity->_count[typeNumber-1]-1 << " cells of type "
                          << tmp_cell_models[i].getName() );
            }

            // Creation of the MEDSKYLINEARRAY
            PointerOf <int> NodalValue(size);
            PointerOf <int> NodalIndex(Connectivity->_count[Connectivity->_numberOfTypes]);
            NodalIndex[0]=1;

            // Fill the MEDSKYLINEARRAY by reading the MED file.
            int j=0;
            for ( i=0;i<Connectivity->_numberOfTypes;i++)
            {
              int tmp_numberOfCells = Connectivity->_count[i+1]-Connectivity->_count[i];
              PointerOf< med_int > tmp_ConnectivityArray( connSizeByType[i] );

              med_err err2;
              switch ( Connectivity->_geometricTypes[i] )
              {
              case MED_EN::MED_POLYGON:
                {
                  PointerOf <med_int> PolygonsConnIndex( tmp_numberOfCells+1 );
                  err2= MEDmeshPolygonRd(_medIdt,_ptrMesh->_name.c_str(),
                                                 dtp3,itp3,
				       ::MED_CELL,
				       ::MED_NODAL,
                                                 PolygonsConnIndex,
                                                 tmp_ConnectivityArray);
                  if(err2 != MED_VALID)
                  {
                    MESSAGE_MED(LOC<<": MEDpolygoneConnLire returns "<<err2);
                    return MED_ERROR;
                  }
                  int* polyindex = (int*)NodalIndex + Connectivity->_count[i] - 1;
                  int delta = polyindex[0] - PolygonsConnIndex[0];
                  for ( j=0; j<=tmp_numberOfCells; j++)
                    polyindex[j]= delta + PolygonsConnIndex[ j ];
                  break;
                }
              case MED_EN::MED_POLYHEDRA:
                {
                  PointerOf< med_int> FacesIndex( polyhedraFacesIndexSize );
                  PointerOf< med_int> PolyhedronIndex( tmp_numberOfCells+1 );
                  err2= MEDmeshPolyhedronRd(_medIdt,_ptrMesh->_name.c_str(),
                                                    dtp3,itp3,
					  ::MED_CELL,
					  ::MED_NODAL,
                                                    PolyhedronIndex,
                                                    FacesIndex,
                                                    tmp_ConnectivityArray);
                  if(err2 != MED_VALID)
                  {
                    MESSAGE_MED(LOC<<": MEDpolyedreConnLire returns "<<err2);
                    return MED_ERROR;
                  }
                  // insert face separators
                  int* polyindex = NodalIndex + Connectivity->_count[i] - 1;
                  int k = NodalIndex[ Connectivity->_count[i]-1 ] - 1;
                  for ( j=0; j<tmp_numberOfCells; j++)
                  {
                    int k0 = k;
                    for ( int iface = PolyhedronIndex[j]; iface < PolyhedronIndex[j+1]; ++iface)
                    {
                      for ( int inode = FacesIndex[iface-1]; inode < FacesIndex[iface]; ++inode)
                        NodalValue[k++] = tmp_ConnectivityArray[inode-1];
                      if( iface+1 < PolyhedronIndex[j+1] )
                        NodalValue[k++] = -1;
                    }
                    polyindex[j+1] = polyindex[j] + k - k0;
                  }
                  continue; // no need to copy tmp_ConnectivityArray - already done
                  //break;
                }
              default:
                med_bool withname=MED_FALSE,withnumber=MED_FALSE,withfam=MED_FALSE;
                int curNbOfElemp3= MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),
                                                          dtp3,itp3,
						::MED_CELL,
                                                          med_geometry_type(Connectivity->_geometricTypes[i]),
                                                          MED_CONNECTIVITY,
						::MED_NODAL,
                                                          &chgtp3,&trfp3);
                char *nomsp3=new char[MED_SNAME_SIZE*curNbOfElemp3+1];
                int *globArrp3=new int[curNbOfElemp3];
                int *famp3=new int[curNbOfElemp3];
                err2=MEDmeshElementRd(_medIdt,_ptrMesh->_name.c_str(),
                                               dtp3,itp3,
				    ::MED_CELL,
                                               med_geometry_type(Connectivity->_geometricTypes[i]),
				    ::MED_NODAL,
				    ::MED_FULL_INTERLACE,
                                               tmp_ConnectivityArray,
                                               &withname,nomsp3,&withnumber,
                                               globArrp3,&withfam,famp3);
                delete [] nomsp3;
                delete [] globArrp3;
                delete [] famp3;
                if( err2 != MED_VALID)
                {
                  MESSAGE_MED(LOC<<": MEDconnLire returns "<<err2);
                  return MED_ERROR;
                }
                int NumberOfNodeByCell = Connectivity->_type[i].getNumberOfNodes();
                // initialise index
                for ( j=Connectivity->_count[i]; j<Connectivity->_count[i+1];j++)
                  NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByCell;
              }

              // version originale sans prise en compte des numeros optionnels
              //
              int * ConnectivityArray = NodalValue + NodalIndex[Connectivity->_count[i]-1]-1;
              for ( j=0; j<connSizeByType[i]; j++)
                ConnectivityArray[j] = tmp_ConnectivityArray[j];

              //////////////////////////////////////////////////////////////////////////////
              // Modification pour prise en compte de la numerotation optionnelle des noeuds ///
              //////////////////////////////////////////////////////////////////////////////
              //
              // Renumerote le tableau temporaire tmp_ConnectivityArray
              // en utilisant _optionnalToCanonicNodesNumbers
              // Le traitement est identique a la version originelle
              // s'il n'y a pas de numerotation optionnelle
              //
              //if(_ptrMesh->_arePresentOptionnalNodesNumbers==1)
              //{
              //  for ( j=0; j<tmp_numberOfCells; j++) for (int k=0; k<NumberOfNodeByCell; k++)
              //    ConnectivityArray[j*NumberOfNodeByCell+k] = _ptrMesh->
              //      _optionnalToCanonicNodesNumbers[tmp_ConnectivityArray[j*(NumberOfNodeByCell+multi)+k]];
              //}
              //else
              //{
              //  for ( j=0; j<tmp_numberOfCells; j++) for (int k=0; k<NumberOfNodeByCell; k++)
              //    ConnectivityArray[j*NumberOfNodeByCell+k] =
              //      tmp_ConnectivityArray[j*(NumberOfNodeByCell+multi)+k];
              //}
              ////////////////////////////////////////////////////////////////////////////

            }

            Connectivity->_nodal = new MEDSKYLINEARRAY(Connectivity->_count[Connectivity->_numberOfTypes]-1,
                                                       size,
                                                       NodalIndex,
                                                       NodalValue);

          } // end of bloc to read CELL

          // Get Face if any
          // ===============
          if(!faceTypes.empty())
          {
            // Create a CONNECTIVITY constituent to put in the top level CONNECTIVITY recursive class
            CONNECTIVITY * constituent = new CONNECTIVITY(faceTypes.size(),MED_EN::MED_FACE);
            constituent->_numberOfNodes = _ptrMesh->getNumberOfNodes();
            constituent->_entityDimension = 2;
            constituent->_count[0]=1;

            // In order to create the MEDSKYLINEARRAY of the constituent object we need :
            // 1:
            // To initialize the _count array of the constituent object
            // (containning cumulated face count by geometric type)
            // _count[0]=1 and _count[_numberOfTypes] give the size of NodalIndex
            // 2:
            // To calculate the total number of face nodes whatever the geometric type is.
            // The result is the size of the array containning all the nodes : NodalValue
            // 3 :
            // To calculate the starting indexes of the different face types in NodalValue,
            // this is the NodalIndex array.

            int size = 0;
            vector<int> connSizeByType;
            for ( i=0; i < (int)faceTypes.size(); i++)
            {
              constituent->_count[i+1] = constituent->_count[i] + tmpFaceCount[i];
              constituent->_type[i] = CELLMODEL_Map::retrieveCellModel( faceTypes[i] );
              constituent->_geometricTypes[i] = faceTypes[i];

              if( faceTypes[i] == MED_EN::MED_POLYGON )
                connSizeByType.push_back( polygonConnSize );
              else
                connSizeByType.push_back( tmpFaceCount[i] * constituent->_type[i].getNumberOfNodes());
              size += connSizeByType.back();
            }

            // Creation of the MEDSKYLINEARRAY
            PointerOf<int> NodalValue (size);
            PointerOf<int> NodalIndex (constituent->_count[constituent->_numberOfTypes]);
            NodalIndex[0]=1;

            // Fill the MEDSKYLINEARRAY by reading the MED file.
            for ( i=0; i<constituent->_numberOfTypes; i++)
            {
                int NumberOfNodeByFace = constituent->_type[i].getNumberOfNodes();

                // Il faut ajouter 1 pour le zero a la lecture !!!
              // ATTENTION UNIQUEMENT POUR MED < 2.2.x
              PointerOf< med_int> tmp_constituentArray;

              MESSAGE_MED(LOC << "Med file version used here " << major << " " << minor << " " << release);

              if((major == 2) && (minor <= 1))
                tmp_constituentArray.set( connSizeByType[i] + tmpFaceCount[i] );
              else if((major == 2) && (minor >= 2))
              {
                tmp_constituentArray.set( connSizeByType[i] );
                MESSAGE_MED(LOC<<": WE ARE USING MED2.2 so there is no +1 for calculating the size of  tmp_constituentArray !");
              }
              else
                tmp_constituentArray.set( connSizeByType[i] );

              if( constituent->_geometricTypes[i] == MED_EN::MED_POLYGON )
              {
                PointerOf< med_int> PolygonsConnIndex( tmpFaceCount[i]+1 );
                int err2 = MEDmeshPolygonRd(_medIdt,_ptrMesh->_name.c_str(),
                                                     dtp3,itp3,
					  ::MED_CELL,
					  ::MED_NODAL,
                                                     PolygonsConnIndex,
                                                     tmp_constituentArray);

                if(err2 != MED_VALID)
                {
                  MESSAGE_MED(LOC<<": MEDpolygoneConnLire returns "<<err2);
                  delete constituent;
                  return MED_ERROR;
                }
                int* polyindex = (int*)NodalIndex + constituent->_count[i] - 1;
                int delta = polyindex[0] - PolygonsConnIndex[0];
                for ( int j=0; j<=tmpFaceCount[i]; j++)
                  polyindex[j]= delta + PolygonsConnIndex[ j ];
              }
              else
              {
                med_geometry_type med_type =
                  (med_geometry_type) constituent->_type[i].getType();

                
                med_bool withname=MED_FALSE,withnumber=MED_FALSE,withfam=MED_FALSE;
                int curNbOfElemp3= MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),
                                                          dtp3,itp3,
						::MED_CELL,med_type,
                                                          MED_CONNECTIVITY,
						::MED_NODAL,
                                                          &chgtp3,&trfp3);
                char *nomsp3=new char[MED_SNAME_SIZE*curNbOfElemp3+1];
                int *globArrp3=new int[curNbOfElemp3];
                int *famp3=new int[curNbOfElemp3];
                int err=MEDmeshElementRd(_medIdt,_ptrMesh->_name.c_str(),
                                                  dtp3,itp3,
				       ::MED_CELL,med_type,
				       ::MED_NODAL,
				       ::MED_FULL_INTERLACE,
                                                  tmp_constituentArray,
                                                  &withname,nomsp3,&withnumber,
                                                  globArrp3,&withfam,famp3);
                delete [] nomsp3;
                delete [] globArrp3;
                delete [] famp3;
                
                if( err != MED_VALID)
                {
                  MESSAGE_MED(LOC<<": MEDconnLire returns "<<err);
                  delete constituent;
                  return MED_ERROR;
                }
                // initialise NodalIndex
                for (int j=constituent->_count[i]; j<constituent->_count[i+1];j++)
                  NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByFace;
              }

              int tmp_numberOfFaces = constituent->_count[i+1]-constituent->_count[i];

              int * constituentArray = NodalValue + NodalIndex[constituent->_count[i]-1]-1;

              MESSAGE_MED(LOC << "Med file version used here " << major << " " << minor << " " << release);
              if((major == 2) && (minor <= 1))
                for (int j=0; j<tmp_numberOfFaces; j++)
                  for (int k=0; k<NumberOfNodeByFace; k++)
                    constituentArray[j*NumberOfNodeByFace+k]=tmp_constituentArray[j*(NumberOfNodeByFace+1)+k];
              else if(((major == 2) && (minor >= 2)) || (major > 2))
                for ( int j = 0; j < connSizeByType[i]; ++j )
                  constituentArray[j]=tmp_constituentArray[j];
            }

            constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,
                                                      size,
                                                      NodalIndex,
                                                      NodalValue);
            Connectivity->_constituent = constituent;
          }

          // get Edge if any
          // ===============
          if( !edgeTypes.empty() )
          {
            CONNECTIVITY * constituent = new CONNECTIVITY(tmpEdgeCount.size() ,MED_EN::MED_EDGE);
            constituent->_numberOfNodes = _ptrMesh->getNumberOfNodes();
            constituent->_entityDimension = 1;
            constituent->_count[0]=1;

            int size = 0;
            for ( i=0; i<(int)edgeTypes.size(); i++)
            {
              constituent->_count[i+1]=constituent->_count[i]+tmpEdgeCount[i];
              constituent->_type[i]=CELLMODEL_Map::retrieveCellModel( edgeTypes[i] );
              constituent->_geometricTypes[i] = edgeTypes[i];

              size+=tmpEdgeCount[i]*constituent->_type[i].getNumberOfNodes();
            }

            // Creation of the MEDSKYLINEARRAY
            PointerOf< int > NodalValue( size );
            PointerOf< int > NodalIndex( constituent->_count[constituent->_numberOfTypes] );
            NodalIndex[0]=1;

            // Fill the MEDSKYLINEARRAY by reading the MED file.
            for ( i=0; i<constituent->_numberOfTypes; i++)
            {
              med_geometry_type med_type =
                (med_geometry_type) constituent->_type[i].getType();
              int NumberOfNodeByEdge = constituent->_type[i].getNumberOfNodes();

              // initialise index
              for (int j=constituent->_count[i]; j<constituent->_count[i+1];j++)
                NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByEdge;

              int tmp_numberOfEdges = constituent->_count[i+1]-constituent->_count[i];
              // Il faut ajouter 1 pour le zero a la lecture !!!

              // ATTENTION UNIQUEMENT POUR MED < 2.2.x
              med_int * tmp_constituentArray = NULL;

              MESSAGE_MED(LOC << "Med file version used here " << major << " " << minor << " " << release);

              if((major == 2) && (minor <= 1))
                tmp_constituentArray = new med_int[(NumberOfNodeByEdge+1)*tmp_numberOfEdges];
              else if((major == 2 && minor >= 2) || ( major > 2 ))
              {
                tmp_constituentArray = new med_int[NumberOfNodeByEdge*tmp_numberOfEdges];
                MESSAGE_MED(LOC<<": WE ARE USING MED2.2 so there is no +1 for calculating the size of  tmp_constituentArray !");
              }


              med_bool withname=MED_FALSE,withnumber=MED_FALSE,withfam=MED_FALSE;
              int curNbOfElemp3= MEDmeshnEntity(_medIdt,_ptrMesh->_name.c_str(),
                                                        dtp3,itp3,
					      ::MED_CELL,
                                                        med_type,
                                                        MED_CONNECTIVITY,
					      ::MED_NODAL,
                                                        &chgtp3,&trfp3);
              char *nomsp3  =new char[MED_SNAME_SIZE*curNbOfElemp3+1];
              int *globArrp3=new int[curNbOfElemp3];
              int *famp3    =new int[curNbOfElemp3];
              int err=MEDmeshElementRd(_medIdt,_ptrMesh->_name.c_str(),
                                                dtp3,itp3,
				     ::MED_CELL,
                                                med_type,
				     ::MED_NODAL,
				     ::MED_FULL_INTERLACE,
                                                tmp_constituentArray,
                                                &withname,nomsp3,&withnumber,
                                                globArrp3,&withfam,famp3);
              delete [] nomsp3;
              delete [] globArrp3;
              delete [] famp3;

              if( err != MED_VALID)
              {
                MESSAGE_MED(LOC<<": MEDconnLire returns "<<err);
                delete constituent;
                delete[] tmp_constituentArray;
                return MED_ERROR;
              }

              int * constituentArray = NodalValue + NodalIndex[constituent->_count[i]-1]-1;

              // version originale sans prise en compte des numéros optionnels
              //
              int multi = 0; // quand est-ce que multi vaut 1 ?? en MED-fichier < 2.2 ??
              MESSAGE_MED(LOC << "Med file version used here " << major << " " << minor << " " << release);

              if((major == 2) && (minor <= 1))
                for (int j=0; j<tmp_numberOfEdges; j++)
                  for (int k=0; k<NumberOfNodeByEdge; k++)
                    constituentArray[j*NumberOfNodeByEdge+k] =
                      tmp_constituentArray[j*(NumberOfNodeByEdge+multi)+k];
              else if((major == 2 && minor >= 2) || (major > 2))
                for (int j=0; j<tmp_numberOfEdges; j++)
                  for (int k=0; k<NumberOfNodeByEdge; k++)
                    constituentArray[j*NumberOfNodeByEdge+k] = tmp_constituentArray[j*(NumberOfNodeByEdge)+k];

              //////////////////////////////////////////////////////////////////////////////////////
              ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
              //////////////////////////////////////////////////////////////////////////////////////
              ///
              /// Rénumérote le tableau temporaire tmp_constituentArray en utilisant
              /// _optionnalToCanonicNodesNumbers
              /// Le traitement est identique à la version originelle
              /// s'il n'y a pas de numérotation optionnelle
              //
              //if(_ptrMesh->_arePresentOptionnalNodesNumbers)
              //{
              //  for (int j=0; j<tmp_numberOfEdges; j++) for (int k=0; k<NumberOfNodeByEdge; k++)
              //    constituentArray[j*NumberOfNodeByEdge+k] = _ptrMesh->
              //      _optionnalToCanonicNodesNumbers[tmp_constituentArray[j*(NumberOfNodeByEdge+1)+k]];
              //}
              //else
              //{
              //  for (int j=0; j<tmp_numberOfEdges; j++) for (int k=0; k<NumberOfNodeByEdge; k++)
              //    constituentArray[j*NumberOfNodeByEdge+k]=tmp_constituentArray[j*(NumberOfNodeByEdge+1)+k];
              //}
              //////////////////////////////////////////////////////////////////////////////////////

              delete[] tmp_constituentArray;
            }

            constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,
                                                      size,
                                                      NodalIndex,
                                                      NodalValue);

            if(Connectivity->_entityDimension == 3)
            {
              if(Connectivity->_constituent==NULL)
                throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Edges are defined but there are no Faces !"));
              Connectivity->_constituent->_constituent = constituent;
            } else
              Connectivity->_constituent = constituent;
          }

        }

        // If there is no nodal connectivity, we return MED_ERROR !
        if(Connectivity->_numberOfTypes == 0)
          return MED_ERROR;
        else
          return MED_VALID;
      }
    return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER::getFAMILY()
{
    const char * LOC = "MED_MESH_RDONLY_DRIVER::getFAMILY() : ";
    BEGIN_OF_MED(LOC);

    if(_status==MED_OPENED)
    {
      int err = 0;

      int * MEDArrayNodeFamily = NULL;
      int ** MEDArrayCellFamily = NULL;
      int ** MEDArrayFaceFamily = NULL;
      int ** MEDArrayEdgeFamily = NULL;

      // NODE :
      MEDArrayNodeFamily = new int[_ptrMesh->getNumberOfNodes()];

      err = getNodesFamiliesNumber(MEDArrayNodeFamily);
      // error only if(_status!=MED_OPENED), other case exeception !
      // CELL

      MESSAGE_MED(LOC << "error returned from getNodesFamiliesNumber " << err);

      MEDArrayCellFamily = new int* [_ptrMesh->getNumberOfTypes(MED_EN::MED_CELL)];
      // ET SI IL N'Y A PAS DE CELLS ?

      const medGeometryElement * myTypes = _ptrMesh->getTypes(MED_EN::MED_CELL);
      for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EN::MED_CELL);i++)
        MEDArrayCellFamily[i] = new
          int[_ptrMesh->getNumberOfElements(MED_EN::MED_CELL,myTypes[i])];

      err = getCellsFamiliesNumber(MEDArrayCellFamily,MED_EN::MED_CELL);
      MESSAGE_MED(LOC << "error returned from getCellsFamiliesNumber for Cells " << err);

      if(_ptrMesh->getNumberOfElements( MED_EN::MED_FACE, MED_EN::MED_ALL_ELEMENTS ))
      {
        // FACE
        MEDArrayFaceFamily = new int* [_ptrMesh->getNumberOfTypes(MED_EN::MED_FACE)];

        myTypes = _ptrMesh->getTypes(MED_EN::MED_FACE);
        for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EN::MED_FACE);i++)
          MEDArrayFaceFamily[i] = new int[ _ptrMesh->getNumberOfElements( MED_EN::MED_FACE,myTypes[i])];

        err = getCellsFamiliesNumber(MEDArrayFaceFamily,MED_EN::MED_FACE);
        MESSAGE_MED(LOC << "error returned from getCellsFamiliesNumber for Faces " << err);
      }
      if(_ptrMesh->getNumberOfElements( MED_EN::MED_EDGE, MED_EN::MED_ALL_ELEMENTS))
      {
        // EDGE in 3D or 2D
        MEDArrayEdgeFamily = new int* [_ptrMesh->getNumberOfTypes(MED_EN::MED_EDGE)];

        const medGeometryElement *myTypes2 = _ptrMesh->getTypes(MED_EN::MED_EDGE);
        for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EN::MED_EDGE);i++)
          MEDArrayEdgeFamily[i] = new
            int[_ptrMesh->getNumberOfElements(MED_EN::MED_EDGE,myTypes2[i])];

        err = getCellsFamiliesNumber(MEDArrayEdgeFamily,MED_EN::MED_EDGE);

        MESSAGE_MED(LOC << "error returned from getCellsFamiliesNumber for Edges " << err);
      }

      int NumberOfFamilies = MEDnFamily(_medIdt,_meshName.c_str());

      if( NumberOfFamilies < 1 ) // at least family 0 must exist
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"There is no FAMILY, FAMILY 0 must exists" ));

      SCRUTE_MED(NumberOfFamilies);

      vector<FAMILY*> &NodeFamilyVector = _ptrMesh->_familyNode;
      vector<FAMILY*> &CellFamilyVector = _ptrMesh->_familyCell;
      vector<FAMILY*> &FaceFamilyVector = _ptrMesh->_familyFace;
      vector<FAMILY*> &EdgeFamilyVector = _ptrMesh->_familyEdge;

      int numberOfNodesFamilies = 0;
      int numberOfCellsFamilies = 0;
      int numberOfFacesFamilies = 0;
      int numberOfEdgesFamilies = 0;


      for (int i=0;i<NumberOfFamilies;i++)
      {
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        med_int tmp_NumberOfAttributes = MEDnFamily23Attribute(_medIdt,_meshName.c_str(),i+1);
        med_int NumberOfAttributes = tmp_NumberOfAttributes;
#else
        int NumberOfAttributes = MEDnFamily23Attribute(_medIdt,_meshName.c_str(),(i+1));
#endif

        if(NumberOfAttributes < 0)
          throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::getFAMILY() : NumberOfAttributes" );

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        med_int tmp_NumberOfGroups = MEDnFamilyGroup(_medIdt,_meshName.c_str(),i+1);
        int NumberOfGroups = tmp_NumberOfGroups;
#else
        int NumberOfGroups = MEDnFamilyGroup(_medIdt,_meshName.c_str(),i+1);
#endif

        if(NumberOfGroups < 0)
          throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::getFAMILY() : NumberOfGroups" );

        int FamilyIdentifier;
        string FamilyName(MED_NAME_SIZE,'\0');
        // 0020071: Crash of V4_1_4rc2 (of testMedMemGeneral.py)
        // Pb with Mistrat_import22.med: it's zero family has non-empty AttributesIdentifier's and
        // AttributesValues' but MEDnAttribut() returns 0 (0 is hardcoded for zero family).
        // So we allocate nothing but MEDfamInfo() reads file contents so overwritting
        // stranger's memory. Stupid solution: allocate more than MEDnAttribut()
        const int iSafe = 10;
        int *  AttributesIdentifier = new int[NumberOfAttributes+iSafe];
        int *  AttributesValues     = new int[NumberOfAttributes+iSafe];
        string AttributesDescription(MED_COMMENT_SIZE*(NumberOfAttributes+iSafe),' ');
        string GroupsNames(MED_LNAME_SIZE*NumberOfGroups+1,'\0');
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        med_int tmp_FamilyIdentifier;
        med_int *  tmp_AttributesIdentifier = new med_int[NumberOfAttributes+iSafe];
        med_int *  tmp_AttributesValues     = new med_int[NumberOfAttributes+iSafe];

        err=MEDfamily23Info(_medIdt,_meshName.c_str(),i+1,const_cast <char *>(FamilyName.c_str()),
                                     tmp_AttributesIdentifier,tmp_AttributesValues,const_cast <char *>(AttributesDescription.c_str()),&tmp_FamilyIdentifier,const_cast <char *>(GroupsNames.c_str()));

        FamilyIdentifier = tmp_FamilyIdentifier;
        int ii;
        for ( ii = 0; ii < NumberOfAttributes; ii++ ) {
          AttributesIdentifier[ii] = tmp_AttributesIdentifier[ii];
          AttributesValues[ii] = tmp_AttributesValues[ii];
        }
        NumberOfAttributes = tmp_NumberOfAttributes;
        NumberOfGroups = tmp_NumberOfGroups;
        delete [] tmp_AttributesIdentifier;
        delete [] tmp_AttributesValues;
#else
        err = MEDfamily23Info(_medIdt,_meshName.c_str(),i+1,const_cast <char *>(FamilyName.c_str()),
                                       AttributesIdentifier,AttributesValues,const_cast <char *>(AttributesDescription.c_str()),&FamilyIdentifier,const_cast <char *>(GroupsNames.c_str()));
#endif


        SCRUTE_MED(GroupsNames);
        SCRUTE_MED(FamilyName);
        SCRUTE_MED(err);
        SCRUTE_MED(i);

        if(err != MED_VALID)
          throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER::getFAMILY() : ERROR when get FAMILY informations" );

        if(FamilyIdentifier != 0 )
        {
          FAMILY * Family = new FAMILY(_ptrMesh,FamilyIdentifier,FamilyName,
                                       NumberOfAttributes,
                                       AttributesIdentifier,
                                       AttributesValues,
                                       AttributesDescription,
                                       NumberOfGroups,GroupsNames,
                                       MEDArrayNodeFamily,
                                       MEDArrayCellFamily,
                                       MEDArrayFaceFamily,
                                       MEDArrayEdgeFamily);

          // All good ?
          // if nothing found, delete Family

          /*! \todo TODO : once unit tests are ready, check that there is no side effect
            and let the driver read empty families by removing the if case of the
            few following lines */

          if(Family->getNumberOfTypes() == 0)
          {
            MESSAGE_MED(LOC<<"Nothing found for family "<<FamilyName<<
                        " : skip");
            Family->removeReference();
          }
          else
            switch (Family->getEntity())
            {
            case MED_EN::MED_NODE :
              _ptrMesh->removeReference();
              NodeFamilyVector.push_back(Family);
              numberOfNodesFamilies++;
              break;
            case MED_EN::MED_CELL :
              _ptrMesh->removeReference();
              CellFamilyVector.push_back(Family);
              numberOfCellsFamilies++;
              break;
            case MED_EN::MED_FACE :
              _ptrMesh->removeReference();
              FaceFamilyVector.push_back(Family);
              numberOfFacesFamilies++;
              break;
            case MED_EN::MED_EDGE :
              _ptrMesh->removeReference();
              EdgeFamilyVector.push_back(Family);
              numberOfEdgesFamilies++;
              break;
            }
        }

        delete [] AttributesIdentifier;
        delete [] AttributesValues;
      }

      if(MEDArrayNodeFamily != NULL)
        delete[] MEDArrayNodeFamily;

      if(MEDArrayCellFamily != NULL)
      {
        for (int i=0; i<_ptrMesh->getNumberOfTypes(MED_EN::MED_CELL); i++)
          delete[] MEDArrayCellFamily[i];
        delete[] MEDArrayCellFamily;
      }

      if(MEDArrayFaceFamily != NULL)
      {
        for (int i=0; i<_ptrMesh->getNumberOfTypes(MED_EN::MED_FACE); i++)
          delete[] MEDArrayFaceFamily[i];
        delete[] MEDArrayFaceFamily;
      }

      if(MEDArrayEdgeFamily != NULL)
      {
        for (int i=0; i<_ptrMesh->getNumberOfTypes(MED_EN::MED_EDGE); i++)
          delete[] MEDArrayEdgeFamily[i];
        delete[] MEDArrayEdgeFamily;
      }

      END_OF_MED(LOC);
      return MED_VALID;
    }

    return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER::getNodesFamiliesNumber(int * MEDArrayNodeFamily)
{
    const char * LOC = "MED_MESH_RDONLY_DRIVER::getNodesFamiliesNumber() : ";

    BEGIN_OF_MED(LOC);

    if(_status==MED_OPENED)
      {

        int dtp3,itp3;
        med_float ttpp3;
        MEDmeshComputationStepInfo(_medIdt,_ptrMesh->_name.c_str(),1,&dtp3,&itp3,&ttpp3);

        int err = 0;

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        med_int * tmp_MEDArrayNodeFamily = new med_int[_ptrMesh->getNumberOfNodes()];
        err=MEDmeshEntityFamilyNumberRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,::MED_NODE,MED_EN::MED_NONE,tmp_MEDArrayNodeFamily);
        int i;
        for ( i = 0; i < _ptrMesh->getNumberOfNodes(); i++ )
          MEDArrayNodeFamily[i] = tmp_MEDArrayNodeFamily[i];
        delete [] tmp_MEDArrayNodeFamily;
#else
        err=MEDmeshEntityFamilyNumberRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,::MED_NODE,MED_EN::MED_NONE,MEDArrayNodeFamily);
#endif

        if( err != MED_VALID)
          {
            std::fill(MEDArrayNodeFamily,MEDArrayNodeFamily+_ptrMesh->getNumberOfNodes(),0);
            //throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "There is no family for the |"<< _ptrMesh->getNumberOfNodes() << "| nodes in mesh |" << _ptrMesh->_name.c_str() << "|"));
          }

        END_OF_MED(LOC);
        return MED_VALID;
      }

    return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER::getCellsFamiliesNumber(int **MEDArrayFamily,
                                                      MED_EN::medEntityMesh entity)
{
    const char * LOC = "MED_MESH_RDONLY_DRIVER::getCellsFamiliesNumber ";

    BEGIN_OF_MED(LOC);

    if(_status==MED_OPENED)
      {
        int dtp3,itp3;
        med_float ttpp3;
        MEDmeshComputationStepInfo(_medIdt,_ptrMesh->_name.c_str(),1,&dtp3,&itp3,&ttpp3);

        int i, err = 0;
        const MED_EN::medGeometryElement *types=_ptrMesh->getTypes(entity);
        for(i=0;i<_ptrMesh->getNumberOfTypes(entity);i++)
          {
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
            int NumberOfCell=_ptrMesh->getNumberOfElements(entity,types[i]);
            med_int * tmp_MEDArrayFamily = new med_int[NumberOfCell];
            err=MEDmeshEntityFamilyNumberRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,(med_entity_type) entity,(med_geometry_type)types[i],tmp_MEDArrayFamily);
            if(err != MED_VALID && !_ptrMesh->getIsAGrid()) // it's normal for a grid (PAL14113)
              {
                err=MEDmeshEntityFamilyNumberRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,::MED_CELL,(med_geometry_type)types[i],tmp_MEDArrayFamily);
                if(err != MED_VALID )
                  {
                    std::fill(tmp_MEDArrayFamily,tmp_MEDArrayFamily+NumberOfCell,0);
                    //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Family not found for entity "<<entity<<" and geometric type "<<types[i]));
                  }
              }
        if(err == MED_VALID) {
          int ii;
          for ( ii = 0; ii < NumberOfCell; ii++ )
            MEDArrayFamily[i][ii] = tmp_MEDArrayFamily[ii];
        }
        delete [] tmp_MEDArrayFamily;
#else
        err=MEDmeshEntityFamilyNumberRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,(med_entity_type) entity,(med_geometry_type)types[i],MEDArrayFamily[i]);

        if(err != MED_VALID
            && !_ptrMesh->getIsAGrid() ) // it's normal for a grid (PAL14113)
        {
          err=MEDmeshEntityFamilyNumberRd(_medIdt,_ptrMesh->_name.c_str(),dtp3,itp3,::MED_CELL,(med_geometry_type)types[i],MEDArrayFamily[i]);

          if(err != MED_VALID)
            {
              int NumberOfCell=_ptrMesh->getNumberOfElements(entity,types[i]);
              std::fill(MEDArrayFamily[i],MEDArrayFamily[i]+NumberOfCell,0);
              //throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Family not found for entity "<<entity<<" and geometric type "<<types[i]));
            }
        }
#endif
      }
      return err ? MED_INVALID : MED_VALID;
    }
    return MED_ERROR;
}

int MED_MESH_RDONLY_DRIVER::getDescendingConnectivity(CONNECTIVITY * Connectivity) 
{
    if(_status==MED_OPENED)
      {
        MESSAGE_MED("MED_MESH_RDONLY_DRIVER::getDescendingConnectivity : "<<"call on the object " << Connectivity);
        MESSAGE_MED("MED_MESH_RDONLY_DRIVER::getDescendingConnectivity : "<<"Not yet implemented !");
      }
    return MED_ERROR;
}

void MED_MESH_RDONLY_DRIVER::buildAllGroups(vector<GROUP*> & Groups, vector<FAMILY*> & Families) 
{
    const char* LOC = "MED_MESH_RDONLY_DRIVER::buildAllGroups ";
    BEGIN_OF_MED(LOC);

    int numberOfFamilies = Families.size() ;
    map< string,list<FAMILY*> > groupsNames ;
    for(int i=0; i<numberOfFamilies; i++) {
      FAMILY * myFamily = Families[i] ;
      int numberOfGroups_ = myFamily->getNumberOfGroups();
      for (int j=0; j<numberOfGroups_; j++) {
        groupsNames[myFamily->getGroupName(j+1)].push_back(myFamily);
      }
    }
    int numberOfGroups = groupsNames.size() ;
    SCRUTE_MED(numberOfGroups);
    Groups.resize(numberOfGroups);
    map< string,list<FAMILY*> >::const_iterator currentGroup ;
    int it = 0 ;
    for(currentGroup=groupsNames.begin();currentGroup!=groupsNames.end();currentGroup++) {
      GROUP * myGroup = new GROUP(healName((*currentGroup).first),(*currentGroup).second) ;
      _ptrMesh->removeReference();
      Groups[it]=myGroup;
      it++;
    }

    END_OF_MED(LOC);
}

void MED_MESH_RDONLY_DRIVER::updateFamily()
{
    const char* LOC = "MED_MESH_RDONLY_DRIVER::updateFamily() ";
    BEGIN_OF_MED(LOC);

    // we need to update family on constituent if we have constituent, but no 
    // descending connectivity, so, we must calculate all constituent and
    // numbering correctly family !
    if( !_ptrMesh->getIsAGrid() )
    {
      MESH* mesh = (MESH*) _ptrMesh;
      mesh->_connectivity->updateFamily(_ptrMesh->_familyFace) ; // in 2d, do nothing
      mesh->_connectivity->updateFamily(_ptrMesh->_familyEdge) ; // in 3d, do nothing
    }
    END_OF_MED(LOC);
}

/*--------------------- WRONLY PART -------------------------------*/

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER():MED_MESH_DRIVER()
{
    this->GENDRIVER::_accessMode = MED_EN::WRONLY;
}

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string &         fileName,
                                                 GMESH *                ptrMesh,
                                                 MED_EN::med_mode_acces access):
    MED_MESH_DRIVER(fileName,ptrMesh,access)
{
    MESSAGE_MED("MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const MED_MESH_WRONLY_DRIVER & driver):
    MED_MESH_DRIVER(driver)
{
}

MED_MESH_WRONLY_DRIVER::~MED_MESH_WRONLY_DRIVER()
{
    //MESSAGE_MED("MED_MESH_WRONLY_DRIVER::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
}

GENDRIVER * MED_MESH_WRONLY_DRIVER::copy(void) const
{
    return new MED_MESH_WRONLY_DRIVER(*this);
}

void MED_MESH_WRONLY_DRIVER::read (void)
{
    throw MEDEXCEPTION("MED_MESH_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

/*
   * - Nom de la fonction : _MEDdatagroupFermer
   * - Description : fermeture d'un datagroup HDF
   * - Parametres :
   *     - id     (IN)     : l'ID du datagroup
   * - Resultat : 0 en cas de succes, -1 sinon
   */ 
med_err _MEDdatagroupFermer(med_idt id)
{

    if( H5Gclose(id) < 0) return -1;
    return 0;
}

/*
   * - Nom de la fonction : _MEDdatagroupOuvrir
   * - Description : ouverture d'un datagroup HDF
   * - Parametres :
   *     - pid     (IN)    : l'ID de l'objet pere
   *     - nom     (IN)    : le nom de l'objet fils
   * - Resultat : l'ID du fils en cas de succes, -1 sinon
   */ 
med_idt _MEDdatagroupOuvrir(med_idt pid, const char * const nom)
    {
      med_idt retval;
      if((retval = H5Gopen(pid,nom)) < 0)
        retval= -1;
      return retval;
    }


void MED_MESH_WRONLY_DRIVER::write(void) const
    {
      if(_ptrMesh==NULL || _ptrMesh->getNumberOfNodes() < 1 )
        throw MEDEXCEPTION("Error trying to write an empty mesh");

      const char LOC[] = "void MED_MESH_WRONLY_DRIVER::write(void) const : ";
      BEGIN_OF_MED(LOC);

      // we must first create mesh !!
      MESSAGE_MED(LOC << "MeshName : |" << _meshName << "| FileName : |"<<_fileName<<"| MedIdt : | "<< _medIdt << "|");

      if(_status!=MED_OPENED)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "File "<<_fileName<<" is not open. Open it before write !"));


      string fieldName;
      if( ( _meshName.empty() ) && ( _ptrMesh->_name.empty() )    )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
                                   <<" neither <meshName> is set in driver nor in object MESH."));

      // If _meshName is not set in driver, try to use _ptrmesh->_meshName
      if( ( _meshName.empty() ) && ( !_ptrMesh->_name.empty() )    )
        _meshName = healName(_ptrMesh->_name );

      if( _meshName.size() > MED_NAME_SIZE )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)
                                   <<" <meshName> size in object driver MESH is > MED_NAME_SIZE ."));


      if(_ptrMesh->getIsAGrid())
        {
          if( writeGRID() != MED_VALID )
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeGRID()"  ));
        }
      else
        {
          if(writeCoordinates()!=MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeCoordinates()"  ));

          if(writeConnectivities(MED_EN::MED_CELL)!=MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_EN::MED_CELL)"  ));
          if(writeConnectivities(MED_EN::MED_FACE)!=MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_EN::MED_FACE)"  ));
          if(writeConnectivities(MED_EN::MED_EDGE)!=MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_EN::MED_EDGE)"  ));
        }
      if(writeFamilyNumbers() !=MED_VALID)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilyNumbers()"  ));


      // well we must first write zero family :
      if(_status==MED_OPENED)
        {
          int err;
          // test if the family already exists (HDF trick waiting a MED evolution to be replaced)
          string dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/FAMILLE_ZERO/";
          MESSAGE_MED("|"<<dataGroupFam<<"|");
          err =_MEDdatagroupOuvrir(_medIdt,const_cast <char *> (dataGroupFam.c_str()) );
          if( err < MED_VALID )
            {
              SCRUTE_MED(err);

              char familyName[MED_NAME_SIZE+1];
              strcpy(familyName,"FAMILLE_ZERO");
              err = MEDfamilyCr(_medIdt,_meshName.c_str(),familyName,0,0,0);

              SCRUTE_MED(familyName);

              if( err != MED_VALID)
                throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't create family |FAMILLE_ZERO| with identifier |0| groups names || and  attributes descriptions ||"));
             }
           else
             _MEDdatagroupFermer(_medIdt);
         }

       MESSAGE_MED(LOC<<"writeFamilies(_ptrMesh->_familyNode)");
       if(writeFamilies(_ptrMesh->_familyNode) !=MED_VALID)
         throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyNode)"  ));

       MESSAGE_MED(LOC<<"writeFamilies(_ptrMesh->_familyCell)");
       if(writeFamilies(_ptrMesh->_familyCell) !=MED_VALID)
         throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyCell)"  ));

       MESSAGE_MED(LOC<<"writeFamilies(_ptrMesh->_familyFace)");
       if(writeFamilies(_ptrMesh->_familyFace) !=MED_VALID)
         throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyFace)"  ));

       MESSAGE_MED(LOC<<"writeFamilies(_ptrMesh->_familyEdge)");
       if(writeFamilies(_ptrMesh->_familyEdge) !=MED_VALID)
         throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyEdge)"  ));
       END_OF_MED(LOC);
    }

//=======================================================================
//function : writeGRID
//purpose  :
//=======================================================================

int MED_MESH_WRONLY_DRIVER::writeGRID() const
    {
      const char *LOC = "MED_MESH_WRONLY_DRIVER::writeGRID() : ";
      BEGIN_OF_MED(LOC);

      if(_status!=MED_OPENED)
        {
          MESSAGE_MED (LOC<<" Not open !!!");
          return MED_ERROR;
        }
      GRID * ptrGrid = (GRID*) _ptrMesh;

      med_err err = MED_ERROR;
      med_axis_type rep;
      string tmp_name(_ptrMesh->_spaceDimension*MED_SNAME_SIZE,' ');
      string tmp_unit(_ptrMesh->_spaceDimension*MED_SNAME_SIZE,' ');

      // Test if the mesh <_meshName> already exists
      // If it doesn't exists create it
      // If it already exists verify if its space and mesh dimensions are the same
      // as <_ptrMesh->_spaceDimension>, <_ptrMesh->getMeshDimension()> respectively
      // rem : <_meshName> is the driver meshName not <ptrMesh->_meshName>

      med_int dtpp3,itpp3;
      med_float ttpp3;

      err=MEDmeshComputationStepInfo(_medIdt,_meshName.c_str(),1,&dtpp3,&itpp3,&ttpp3);

      int spaceDimension=-1;
      int meshDimension=-1;
      if(!err)
        {
          med_mesh_type ttmp3;
          char commentp3[MED_COMMENT_SIZE+1];
          char dtunittp3[MED_LNAME_SIZE+1];
          med_sorting_type sttp3;
          int nstep;
          med_axis_type axtypp3;
          int naxis=std::max( 3, MEDmeshnAxisByName(_medIdt,_meshName.c_str()));
          char *t1pp3= new char[naxis*MED_SNAME_SIZE+1];
          char *t2pp3= new char[naxis*MED_SNAME_SIZE+1];
          MEDmeshInfoByName(_medIdt,_meshName.c_str(),&spaceDimension,&meshDimension,&ttmp3,commentp3,dtunittp3,&sttp3,&nstep,&axtypp3,t1pp3,t2pp3);
          delete [] t1pp3;
          delete [] t2pp3;
        }
      // Recompose the <_spaceDimension> strings in 1 string
      int lengthString;
      string valueString;
      for(int i=0;i<_ptrMesh->_spaceDimension;i++)
        {
          SCRUTE_MED(i);
          valueString = ptrGrid->_coordinate->_coordinateName[i];
          lengthString = (MED_SNAME_SIZE<valueString.size())?MED_SNAME_SIZE:valueString.size();
          tmp_name.replace(i*MED_SNAME_SIZE,i*MED_SNAME_SIZE+lengthString,valueString,0,lengthString);
          valueString = ptrGrid->_coordinate->_coordinateUnit[i];
          lengthString = (MED_SNAME_SIZE<valueString.size())?MED_SNAME_SIZE:valueString.size();
          tmp_unit.replace(i*MED_SNAME_SIZE,i*MED_SNAME_SIZE+lengthString,valueString,0,lengthString);
        }

      if(err) // create a mesh in the file
        {
          // Pourquoi le stocker sous forme de chaîne ?
          const string & coordinateSystem = ptrGrid->_coordinate->_coordinateSystem;
          if(coordinateSystem  == "CARTESIAN")
            rep = ::MED_CARTESIAN;
          else if( coordinateSystem == "CYLINDRICAL")
            rep = MED_CYLINDRICAL;
          else if( coordinateSystem == "SPHERICAL" )
            rep = MED_SPHERICAL;
          else
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Grid |" << _meshName.c_str() <<
                                         "| doesn't have a valid coordinate system : |"
                                         << ptrGrid->_coordinate->_coordinateSystem
                                         << "|" ));

          _ptrMesh->_description.resize(MED_COMMENT_SIZE+1,'\0');
          char dtunitp3[MED_LNAME_SIZE+1];
          std::fill(dtunitp3,dtunitp3+MED_LNAME_SIZE+1,'\0');

          err = MEDmeshCr(_medIdt,_meshName.c_str(),
                                   _ptrMesh->getSpaceDimension(),
                                   _ptrMesh->getMeshDimension(),
      		      MED_STRUCTURED_MESH,
                                   _ptrMesh->_description.c_str(),
		      dtunitp3,MED_SORT_DTIT,
                                   rep,
                                   const_cast <char *> (tmp_name.c_str()),
                                   const_cast <char *> (tmp_unit.c_str()));

          if(err != MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Unable to create Grid"));
          else
            MESSAGE_MED(LOC<<"Grid "<<_meshName<<" created in file "<<_fileName<<" !");

          err = MEDmeshGridTypeWr(_medIdt,_meshName.c_str(),::med_grid_type(ptrGrid->getGridType()));
          if(err != MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Error in MEDmeshGridTypeWr()"));

          meshDimension =  _ptrMesh->getMeshDimension();
        }
      else if((spaceDimension != _ptrMesh->_spaceDimension)  &&
               (meshDimension != _ptrMesh->getMeshDimension()))
        {
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Grid |" << _meshName.c_str() <<
                                       "| already exists in file |" << _fileName <<
                                       "| with space dimension |" << spaceDimension <<
                                       "| and mesh dimension |" << meshDimension <<
                                       "| but the space dimension and the mesh dimension of "
                                       "the mesh we want to write are respectively |"
                                       << _ptrMesh->_spaceDimension <<"|" <<
                                       _ptrMesh->getMeshDimension() <<"|" ));
        }

      MED_EN::med_grid_type gridType = ptrGrid->getGridType();

      if(err != MED_VALID)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Unable to write the type of the Grid"));


      med_int ArrayLen[] = { (med_int) ptrGrid->_iArrayLength,
                                      (med_int) ptrGrid->_jArrayLength,
                                      (med_int) ptrGrid->_kArrayLength  };

      // Write node coordinates for MED_BODY_FITTED grid
      if(gridType == MED_EN::MED_BODY_FITTED)
        {
          // Write Coordinates and families

          err = MEDmeshNodeCoordinateWr(_medIdt,_meshName.c_str(),
                                                 MED_NO_DT,MED_NO_IT,MED_NO_DT,
				    ::MED_FULL_INTERLACE,
                                                 ptrGrid->getNumberOfNodes(),
                                                 ptrGrid->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE));

          if(err != MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write coordinates of the grid |" << _meshName.c_str() << "| in file |" << _fileName
                                         << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and"
                                         << " with units names |"  << tmp_name
                                         << "| and units |"       << tmp_unit
                                         << " |"));

          med_int* structure = new med_int [meshDimension];

          for (int idim = 0; idim < meshDimension; ++idim)
            structure[idim] = ArrayLen [idim];


          err = MEDmeshGridStructWr(_medIdt,_meshName.c_str(),
                                             MED_NO_DT,MED_NO_IT,MED_NO_DT,
                                             structure);

          if(err != MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"error in writing the structure of the grid |" << _meshName.c_str()));

          delete [] structure;
        }
      else if((gridType == MED_EN::MED_CARTESIAN) ||
               (gridType == MED_POLAR))
        {
          // Write Arrays of Cartesian or Polar Grid

          double * Array[] = { ptrGrid->_iArray,
                               ptrGrid->_jArray,
                               ptrGrid->_kArray };

          for (int idim = 0; idim < _ptrMesh->getMeshDimension(); ++idim)
            {
              string str_name = string (tmp_name,idim*MED_SNAME_SIZE,
                                        MED_SNAME_SIZE);
              string str_unit = string (tmp_unit,idim*MED_SNAME_SIZE,
                                        MED_SNAME_SIZE);

              err = MEDmeshGridIndexCoordinateWr(_medIdt,_meshName.c_str(),
                                                          MED_NO_DT,MED_NO_IT,MED_NO_DT,
                                                          idim+1,
                                                          ArrayLen[idim],
                                                          Array[idim]);

              if(err != MED_VALID)
                throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<
                                             "Can't write grid coordinates for " <<
                                             idim << "-th dimention"));
            }
      } // end Write  Cartesian or Polar Grid

      END_OF_MED(LOC);
      return MED_VALID;
    }

//=======================================================================
//function : writeCoordinates
//purpose  :
//=======================================================================

int MED_MESH_WRONLY_DRIVER::writeCoordinates() const 
    {
      const char LOC[] = "int MED_MESH_WRONLY_DRIVER::writeCoordinates() const : ";
      BEGIN_OF_MED(LOC);

      MESH *ptrMesh = (MESH*) _ptrMesh;

      med_err err = MED_ERROR;
      med_axis_type rep;
      string tmp_name(ptrMesh->_spaceDimension*MED_SNAME_SIZE+1,' ');
      string tmp_unit(ptrMesh->_spaceDimension*MED_SNAME_SIZE+1,' ');

      // Recompose the <_spaceDimension> strings in 1 string
      int lengthString;
      string valueString;
      for(int i=0;i<ptrMesh->_spaceDimension;i++)
        {
          valueString = ptrMesh->_coordinate->_coordinateName[i];
          lengthString = (MED_SNAME_SIZE<valueString.size())?MED_SNAME_SIZE:valueString.size();
          tmp_name.replace(i*MED_SNAME_SIZE,i*MED_SNAME_SIZE+lengthString,valueString,0,lengthString);
          valueString = ptrMesh->_coordinate->_coordinateUnit[i];
          lengthString = (MED_SNAME_SIZE<valueString.size())?MED_SNAME_SIZE:valueString.size();
          tmp_unit.replace(i*MED_SNAME_SIZE,i*MED_SNAME_SIZE+lengthString,valueString,0,lengthString);
        }
      // Pourquoi le stocker sous forme de chaîne ?
      const string & coordinateSystem = ptrMesh->_coordinate->_coordinateSystem;
      if(coordinateSystem  == "CARTESIAN")
        rep = ::MED_CARTESIAN;
      else if( coordinateSystem == "CYLINDRICAL")
        rep = MED_CYLINDRICAL;
      else if( coordinateSystem == "SPHERICAL" )
        rep = MED_SPHERICAL;
      else
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Mesh |" << _meshName.c_str() << "| doesn't have a valid coordinate system : |"
                                   << ptrMesh->_coordinate->_coordinateSystem
                                   << "|" ));
      // Test if the mesh <_meshName> already exists
      // If it doesn't exists create it
      // If it already exists verify if its space and mesh dimensions are the same
      // as <ptrMesh->_spaceDimension>, <ptrMesh->getMeshDimension()> respectively
      // rem : <_meshName> is the driver meshName not <ptrMesh->_meshName>

      med_int dtpp3,itpp3;
      med_float ttpp3;
      std::cout << "MedMeshDriver ************ here 3" << std::endl;
      std::cout << "_medIdt= " << _medIdt << std::endl;
      err=MEDmeshComputationStepInfo(_medIdt,_meshName.c_str(),1,&dtpp3,&itpp3,&ttpp3);
      std::cout << "MedMeshDriver ************ here 4" << std::endl;

    int spaceDimension=-1;
    int meshDimension=-1;
    if(!err)
      {
        med_mesh_type ttmp3;
        char commentp3[MED_COMMENT_SIZE+1];
        char dtunittp3[MED_LNAME_SIZE+1];
        med_sorting_type sttp3;
        int nstep;
        med_axis_type axtypp3;
        int naxis= MEDmeshnAxisByName(_medIdt,_meshName.c_str());
        if( naxis > 0 )
          {
            char *t1pp3=new char[naxis*MED_SNAME_SIZE+1];
            char *t2pp3=new char[naxis*MED_SNAME_SIZE+1];
            MEDmeshInfoByName(_medIdt,_meshName.c_str(),&spaceDimension,&meshDimension,&ttmp3,commentp3,dtunittp3,&sttp3,&nstep,&axtypp3,t1pp3,t2pp3);
            delete [] t1pp3;
            delete [] t2pp3;
          }
        else
          {
            err = MED_INVALID;
          }
      }
    SCRUTE_MED(spaceDimension);
    SCRUTE_MED(meshDimension);
    SCRUTE_MED(ptrMesh->_spaceDimension);
    SCRUTE_MED(ptrMesh->getMeshDimension());

    if(err)
      {
        _ptrMesh->_description.resize(MED_COMMENT_SIZE+1,'\0');
        char dtunitp3[MED_LNAME_SIZE+1];
        std::fill(dtunitp3,dtunitp3+MED_LNAME_SIZE+1,'\0');

        err = MEDmeshCr(_medIdt,_meshName.c_str(),
                                 _ptrMesh->getSpaceDimension(),
                                 _ptrMesh->getMeshDimension(),
                                 MED_UNSTRUCTURED_MESH,
                                 _ptrMesh->_description.c_str(),
                                 dtunitp3,
                                 MED_SORT_DTIT,
                                 rep,
                                 const_cast <char *> (tmp_name.c_str()),
                                 const_cast <char *> (tmp_unit.c_str()));

        if(err < MED_VALID)
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Unable to create Mesh : |" << _meshName << "|"));
        else
          MESSAGE_MED(LOC<<"Mesh "<<_meshName<<" created in file "<<_fileName<<" !");
      }
    else if((spaceDimension != ptrMesh->_spaceDimension)  &&
             (meshDimension != ptrMesh->getMeshDimension()))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Mesh |" << _meshName.c_str() <<
                                   "| already exists in file |" << _fileName <<
                                   "| with space dimension |" << spaceDimension <<
                                   "| and mesh dimension |" << meshDimension <<
                                   "| but the space dimension and the mesh dimension of the mesh we want to write are respectively |"
                                   << ptrMesh->_spaceDimension <<"|" <<
                                   ptrMesh->getMeshDimension() << "|"));
    
    err= MEDmeshNodeCoordinateWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,MED_NO_DT,::MED_FULL_INTERLACE,ptrMesh->getNumberOfNodes(),
                                         const_cast <double *> ( ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE)));

    if(err<0)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write coordinates of mesh |" << _meshName.c_str() << "| in file |" << _fileName
                                   << "| with dimension |"  << ptrMesh->_spaceDimension <<"| and"
                                   << " with units names |"  << tmp_name
                                   << "| and units |"       << tmp_unit
                                   << " |"));


         //////////////////////////////////////////////////////////////////////////////////////
         ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
         //////////////////////////////////////////////////////////////////////////////////////
         ///
         /// Ecrit les numéros optionnels des noeuds
         /// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle


        if(ptrMesh->_arePresentOptionnalNodesNumbers==1) {

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
          const int * NodesNumbers = ptrMesh->_coordinate->getNodesNumbers();
          med_int * tmp_NodesNumbers = new med_int[ptrMesh->getNumberOfNodes()];
          int ii;
          for ( ii = 0; ii < ptrMesh->getNumberOfNodes(); ii++ )
             tmp_NodesNumbers[ii] = NodesNumbers[ii];
          err= MEDmeshEntityNumberWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,::MED_NODE,MED_EN::MED_NONE,ptrMesh->getNumberOfNodes(),
                                             tmp_NodesNumbers);
          delete [] tmp_NodesNumbers;
#else
          err= MEDmeshEntityNumberWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,::MED_NODE,MED_EN::MED_NONE,ptrMesh->getNumberOfNodes(),
                                             const_cast<med_int *>(ptrMesh->_coordinate->getNodesNumbers()));
#endif

          if(err != MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write optionnal numbers of mesh |" <<
                                         _meshName.c_str() << "| in file |" <<
                                         _fileName  << " |"));
        }
        //////////////////////////////////////////////////////////////////////////////////////

    END_OF_MED(LOC);

    return MED_VALID;
}




int MED_MESH_WRONLY_DRIVER::writeConnectivities(medEntityMesh entity) const
{
    const char * LOC="int MED_MESH_WRONLY_DRIVER::writeConnectivities() const : ";
    BEGIN_OF_MED(LOC);

    med_err err;

    MESH * ptrMesh = (MESH*) _ptrMesh;

    if( ptrMesh->_connectivity == (CONNECTIVITY *) NULL )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The connectivity is not defined in the MESH object "));


    if( ptrMesh->existConnectivity(MED_EN::MED_NODAL,entity) )
      {
        int numberOfTypes = ptrMesh->getNumberOfTypes(entity);
        const medGeometryElement * types = ptrMesh->getTypes(entity);
        const int * index = ptrMesh->getConnectivityIndex(MED_EN::MED_NODAL, entity);

        for (int i=0; i<numberOfTypes; i++)
          {
            int numberOfElements = ptrMesh->getNumberOfElements(entity,types[i]);
            const int * connectivity = ptrMesh->getConnectivity(MED_EN::MED_NODAL, entity, types[i]);
            int size = ptrMesh->getConnectivityLength(MED_EN::MED_NODAL, entity, types[i]);

            switch ( types[i] )
              {
              case MED_EN::MED_POLYGON:
                {
                  int nbStdCells = ptrMesh->getGlobalNumberingIndex(entity)[i]-1;
                  vector< med_int > connectivityArray( connectivity, connectivity + size );
                  vector< med_int > tmp_Index( numberOfElements+1 );
                  for ( unsigned j = 0; j < tmp_Index.size(); ++ j )
                    tmp_Index[j] = index[ nbStdCells + j ] - index[ nbStdCells ] + 1;
                  err= MEDmeshPolygonWr(_medIdt,_meshName.c_str(),
                                                MED_NO_DT,MED_NO_IT,MED_NO_DT,
				      ::MED_CELL,
				      ::MED_NODAL,
                                                numberOfElements+1,
                                                &tmp_Index[0],
                                                &connectivityArray[0]);

                  if(err<0)
                    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write polygons nodal connectivities of mesh |"
                                                 <<  _meshName.c_str() << "| in file |" << _fileName
                                                 << "| with dimension |"  << ptrMesh->_spaceDimension <<"| and"
                                                 ));
                  break;
                }
              case MED_EN::MED_POLYHEDRA:
                {
                  vector< med_int > PolyhedronIndex, PolyhedronFacesIndex, PolyhedronConnectivity;
                  PolyhedronIndex.push_back(1);
                  PolyhedronFacesIndex.push_back(1);
                  connectivity = ptrMesh->getConnectivity(MED_EN::MED_NODAL, entity, MED_ALL_ELEMENTS);
                  int nbStdCells = ptrMesh->getGlobalNumberingIndex(entity)[i]-1;
                  for ( int j = nbStdCells; j < nbStdCells+numberOfElements; ++j )
                    {
                      for ( int k = index[j]; k < index[j+1]; ++k )
                        if( connectivity[k-1] == -1 )
                          PolyhedronFacesIndex.push_back( PolyhedronConnectivity.size()+1 );
                        else
                          PolyhedronConnectivity.push_back( connectivity[k-1] );
                      PolyhedronIndex.push_back( PolyhedronFacesIndex.size()+1 );
                      PolyhedronFacesIndex.push_back( PolyhedronConnectivity.size()+1 );
                    }
                  err= MEDmeshPolyhedronWr(_medIdt,_meshName.c_str(),
                                                   MED_NO_DT,MED_NO_IT,MED_NO_DT,
					 ::MED_CELL,
					 ::MED_NODAL,
                                                   PolyhedronIndex.size(),
                                                   &PolyhedronIndex[0],
                                                   PolyhedronFacesIndex.size(),
                                                   &PolyhedronFacesIndex[0],
                                                   &PolyhedronConnectivity[0]);
                  if(err<0)
                    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write polyhedron nodal connectivities of mesh |"
                                                 << _meshName.c_str() << "| in file |" << _fileName
                                                 << "| with dimension |"  << ptrMesh->_spaceDimension <<"| and"
                                                 ));
                  break;
                }
              default:

                vector< med_int > connectivityArray( connectivity, connectivity + size );
                err=MEDmeshElementConnectivityWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,MED_NO_DT,::MED_CELL,(med_geometry_type)types[i],
					       ::MED_NODAL,::MED_FULL_INTERLACE,numberOfElements,&connectivityArray[0]);

                if(err<0) // ETENDRE LES EXPLICATIONS
                  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write connectivities of mesh |"
                                               << _meshName.c_str() << "| in file |" << _fileName
                                               << "| with dimension |"  << ptrMesh->_spaceDimension <<"| and"));
              }
          }
      }


    // Descending connectivity for classic geometric types
    if( ptrMesh->existConnectivity(MED_EN::MED_DESCENDING,entity) )
      {
        int numberOfTypes                = ptrMesh->getNumberOfTypes (entity);
        const medGeometryElement * types = ptrMesh->getTypes         (entity);

        for (int i=0; i<numberOfTypes; i++)
          {
            int    numberOfElements = ptrMesh->getNumberOfElements (entity,types[i]);
            const int * connectivity = ptrMesh->getConnectivity(MED_EN::MED_DESCENDING, entity, types[i]);

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
            int lgth = ptrMesh->getConnectivityLength(MED_EN::MED_DESCENDING, entity, types[i]);
            vector< med_int > tmp_Connectivity( connectivity, connectivity + lgth );
            err=MEDmeshElementConnectivityWr(_medIdt,_meshName.c_str(),
                                                      MED_NO_DT,MED_NO_IT,MED_NO_DT,
					   ::MED_CELL,
                                                      (med_geometry_type)types[i],
					   ::MED_DESCENDING,
					   ::MED_FULL_INTERLACE,
                                                      numberOfElements,
                                                      &tmp_Connectivity[0]);
#else
            err=MEDmeshElementConnectivityWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,MED_NO_DT,
					   ::MED_CELL,
                                                      (med_geometry_type)types[i],
					   ::MED_DESCENDING,
					   ::MED_FULL_INTERLACE,
                                                      numberOfElements,
                                                      const_cast<int *>(connectivity));
#endif

            if(err<0) // ETENDRE LES EXPLICATIONS
              throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write connectivities of mesh |"
                                           << _meshName.c_str() << "| in file |" << _fileName
                                           << "| with dimension |"  << ptrMesh->_spaceDimension <<"| and"
                                           ));

          }
      }

    END_OF_MED(LOC);
    return MED_VALID;
}

/*!creates a vector of families from a vector of groups
   *
   * \param myGroups input : vector of groups
   * \param myFamilies output vector of families
   *
   * Routine is meant to be called to reconstruct families from existing groups
   * typically for writing meshes created by MEDMEM::MESHING containing overlapping groups
   *
   * if the groups do not overlap, the vector of families will have the same size
   * as the vector of groups
   * otherwise, the size of the family vector will be larger
*/

void MED_MESH_WRONLY_DRIVER::groupFamilyConverter(const vector <GROUP*>& myGroups, vector <FAMILY*>& myFamilies ) const
{

    const char* LOC = "MED_MESH_WRONLY_DRIVER::groupFamilyConverter";
    BEGIN_OF_MED(LOC);
    if(myGroups.empty()) return;

//      if(!myFamilies.empty())
//      throw MEDEXCEPTION(LOCALIZED("Family vector must be empty on call "
//      << "to groupFamilyConverter"));
//
   //mapping elements to all the groups containing it
    std::multimap <int,int> elem2groups;
    for (int igroup=0; igroup< (int)myGroups.size(); igroup++)
    {
      const int*Number = myGroups[igroup]->getNumber(MED_ALL_ELEMENTS);
      for (int ielem = 0; ielem< myGroups[igroup]->getNumberOfElements(MED_ALL_ELEMENTS); ielem++)
      {
        elem2groups.insert(make_pair(Number[ielem], igroup));
      }
    }

    //creating a set of signatures for the groups intersections
    std::multimap<vector<int>,int> signatures;

    typedef multimap<int,int>::iterator MI;
    MI iter=elem2groups.begin();

    while (iter!=elem2groups.end())
    {
      vector<int> sign (1, iter -> second );
      int key = iter -> first;
      iter ++;
      while (iter!=elem2groups.end()&&iter-> first == key)
      {
        sign.push_back(iter -> second);
        iter++;
      }
      signatures.insert(make_pair(sign,key));
    }

    elem2groups.clear();

    //creating the families from the signatures mapping
    //each signature will correspond to a new family
    std::multimap<vector<int>,int>::const_iterator iter_signatures = signatures.begin();

    //retrieving the entity type (all the groups have the same)
    // node families are numbered above 0
    // cell families are numbered from  -1 to -MAX_NB_GROUPS
    // face falimies are numbered from -MAX_NB_GROUPS-1 to -2*MAX_NB_GROUPS
    // edge families are numbered from -2*MAX_NB_GROUPS to -3*MAX_NB_GROUPS
    // MAX_NB_GROUPS is defined in MEDMEM_define.hxx

    medEntityMesh entity = myGroups[0]->getEntity();
    MESH* mesh=(MESH*)myGroups[0]->getMesh();

    int ifamily= 0;
    switch (entity)
    {
    case MED_EN::MED_NODE:
      ifamily=1;
      break;
    case MED_EN::MED_CELL:
      ifamily=-MAX_NB_GROUP;
      break;
    case MED_EN::MED_FACE:
      ifamily=-2*MAX_NB_GROUP;
      break;
    case MED_EN::MED_EDGE:
      ifamily=-3*MAX_NB_GROUP;
      break;
    }

    //browsing signatures to build all the families
    //each signature corresponds to a new family

    while (iter_signatures!= signatures.end())
    {
      const vector<int>& key= iter_signatures->first;
      int size = signatures.count(key);

      list<int> numbers;

      for (int i=0; i< size; i++)
      {
        numbers.push_back(iter_signatures->second);
        iter_signatures++;
      }

      //TODO : see if build SupportOnElementFromElementList could not be built from another container

      // for nodes, the family is built directly from the node list
      // for elements, it is built from the buildSupportOnElementsFromElementList
      //               which allocates a support
      FAMILY* myFamily;
      if(entity!=MED_EN::MED_NODE)
      {
        SUPPORT* support;
        support=mesh->buildSupportOnElementsFromElementList(numbers, entity);
        myFamily=new FAMILY(*support);
        support->removeReference();
      }
      else
      {
        myFamily= new FAMILY();
        myFamily->setMesh(mesh);
        myFamily->fillFromNodeList(numbers);
      }


      // the identifier and the groups are set
      myFamily->setIdentifier(ifamily);
      myFamily->setNumberOfGroups(key.size());
      char family_name[MED_NAME_SIZE];

      //if the family has one group to which only one family
      //is associated, the name of the family underlying the group
      //is given back to the family
      if(key.size()==1 && myGroups[key[0]]->getNumberOfFamilies()==0)
      {
        vector<FAMILY*> families;
        families.push_back(myFamily);
        myGroups[key[0]]->setFamilies(families);
        //it is necessary to use strncpy because group and family
        //have different name sizes
        strncpy(family_name,myGroups[key[0]]->getName().c_str(),MED_NAME_SIZE);
        family_name[MED_NAME_SIZE-1]='\0';
      }
      else
        sprintf(family_name,"family%d",ifamily);

      myFamily->setName( healName( family_name ));

      string* groupnames=new string[key.size()];

      for (int igroup=0; igroup<(int)key.size(); igroup++)
      {
        groupnames[igroup]=myGroups[key[igroup]]->getName();
      }

      myFamily->setGroupsNames(groupnames,true);
      if(myFamily->getMesh()==_ptrMesh)
        _ptrMesh->removeReference();
      myFamilies.push_back(myFamily);
      ifamily++;
    }

    //adding empty families corresponding to empty groups
    for (size_t i=0; i<myGroups.size(); i++)
    {
      if(myGroups[i]->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS)==0)
      {
        FAMILY* myFamily=new FAMILY(*(myGroups[i]));
        char family_name[MED_NAME_SIZE];
        //it is necessary to use strncpy because group and family
        //have different name sizes
        strncpy(family_name,myGroups[i]->getName().c_str(),MED_NAME_SIZE);
        family_name[MED_NAME_SIZE-1]='\0';
        myFamily->setName( healName( family_name ));
        myFamily->setIdentifier(ifamily);
        ifamily++;
        vector<string> groupnames;
        groupnames.push_back(myGroups[i]->getName());
        myFamily->setNumberOfGroups(1);
        myFamily->setGroupsNames(&groupnames[0]);
        if(myFamily->getMesh()==_ptrMesh)
          _ptrMesh->removeReference();
        myFamilies.push_back(myFamily);
      }
    }
    END_OF_MED(LOC);
}

int MED_MESH_WRONLY_DRIVER::writeFamilyNumbers() const {

    const char * LOC="int MED_MESH_WRONLY_DRIVER::writeFamilyNumbers() const : ";
    BEGIN_OF_MED(LOC);

    med_err err;

    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArrayNodeFamily DOIT ETRE ENLEVER DE LA CLASSE MESH

    { // Node related block

      // We build the array from the families list objects :
      int NumberOfNodes = _ptrMesh->getNumberOfNodes();
      med_int * MEDArrayNodeFamily = new med_int[NumberOfNodes];
      // family 0 by default
      for (int i=0; i<NumberOfNodes; i++)
        MEDArrayNodeFamily[i]=0;
      vector<FAMILY*> * myFamilies = &_ptrMesh->_familyNode;
      int NumberOfNodesFamilies = myFamilies->size();
      if(0 == NumberOfNodesFamilies) {
        vector<GROUP*> myGroups = _ptrMesh->getGroups(MED_EN::MED_NODE);

        groupFamilyConverter(myGroups,*myFamilies);

        NumberOfNodesFamilies=myFamilies->size();
      }
      for (int i=0; i<NumberOfNodesFamilies; i++) {
        int FamilyIdentifier = (*myFamilies)[i]->getIdentifier();
        int TotalNumber = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS);
        const int * Number = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS);
        for (int j=0; j<TotalNumber; j++)
          MEDArrayNodeFamily[Number[j]-1]=FamilyIdentifier;
      }
      err=MEDmeshEntityFamilyNumberWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,::MED_NODE,MED_EN::MED_NONE,NumberOfNodes,MEDArrayNodeFamily);


      if( err != MED_VALID)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write node family for the |"<< NumberOfNodes
                                     << "| nodes in mesh |"
                                     << _ptrMesh->_name.c_str() << "|" ));
      delete[] MEDArrayNodeFamily;
    }

    { // CELLS RELATED BLOCK
      medEntityMesh entity=MED_EN::MED_CELL;
      // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
      if((_ptrMesh->getIsAGrid()) || // PAL18712, GRID::existConnectivity() calls GRID::fillConnectivity() but it is not needed for writing GRID
          (((MESH*)_ptrMesh)->existConnectivity(MED_EN::MED_NODAL,entity)) ||
          (((MESH*)_ptrMesh)->existConnectivity(MED_EN::MED_DESCENDING,entity)))
      {
        int numberOfTypes                 = _ptrMesh->getNumberOfTypes(entity);
        const medGeometryElement  * types = _ptrMesh->getTypes(entity);

        // We build the array from the families list objects :
        int NumberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS);
        int * MEDArrayFamily = new int[NumberOfElements];
        // family 0 by default
        for (int i=0; i<NumberOfElements; i++)
          MEDArrayFamily[i]=0;
        vector<FAMILY*> * myFamilies = &_ptrMesh->_familyCell;
        int NumberOfFamilies = myFamilies->size();
        if(0 == NumberOfFamilies) {
          vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);
          groupFamilyConverter(myGroups,*myFamilies);
          NumberOfFamilies=myFamilies->size();
        }
        for (int i=0; i<NumberOfFamilies; i++) {
          int FamilyIdentifier = (*myFamilies)[i]->getIdentifier();
          int TotalNumber = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS);
          const int * Number = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS);
          for (int ii=0; ii<TotalNumber; ii++)
            MEDArrayFamily[Number[ii]-1]=FamilyIdentifier;
        }

        int offset=0;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        vector < med_int > temp( MEDArrayFamily, MEDArrayFamily + NumberOfElements );
#endif
        for (int i=0; i<numberOfTypes; i++)
        {
          int typeNumberOfElements=_ptrMesh->getNumberOfElements(entity,types[i]);
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
          err=MEDmeshEntityFamilyNumberWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,::MED_CELL,(med_geometry_type) types[i],typeNumberOfElements,&temp[0]+offset);
#else
          err=MEDmeshEntityFamilyNumberWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,::MED_CELL,(med_geometry_type) types[i],typeNumberOfElements,MEDArrayFamily+offset);
#endif
          MESSAGE_MED("OK "<<i);
          if( err != MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< typeNumberOfElements
                                         << "| cells of geometric type |" << geoNames[types[i]] <<"|in mesh |"
                                         << _ptrMesh->_name.c_str() << "|" ));
          offset+=typeNumberOfElements;
        }
        delete[] MEDArrayFamily;
      }
    }

    if(!_ptrMesh->getIsAGrid() || _ptrMesh->getMeshDimension() == 3 ) // for grid - only in 3D mesh

    { // FACE RELATED BLOCK
      medEntityMesh entity=MED_EN::MED_FACE;
      // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
      if((_ptrMesh->getIsAGrid()) || // PAL18712, GRID::existConnectivity() calls GRID::fillConnectivity() but it is not needed for writing GRID
          (((MESH*)_ptrMesh)->existConnectivity(MED_EN::MED_NODAL,entity)) ||
          (((MESH*)_ptrMesh)->existConnectivity(MED_EN::MED_DESCENDING,entity)))
      {
        int numberOfTypes                 = _ptrMesh->getNumberOfTypes(entity);
        const medGeometryElement  * types = _ptrMesh->getTypes(entity);
        SCRUTE_MED(numberOfTypes);

        int numberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS);
        int * familyArray = new int[numberOfElements];
        for (int i=0;i<numberOfElements;i++)
          familyArray[i]=0;

        int numberOfFamilies = _ptrMesh->getNumberOfFamilies(entity);
        vector<FAMILY*> * myFamilies = &_ptrMesh->_familyFace;
        if(0 == numberOfFamilies) {
          vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);

          groupFamilyConverter(myGroups,*myFamilies);

          numberOfFamilies=myFamilies->size();
        }
        for (int i=0;i<numberOfFamilies;i++) {
          int familyNumber = (*myFamilies)[i]->getIdentifier();
          int numberOfFamilyElements = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS);
          const int * myFamilyElements = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS);
          for (int ii=0;ii<numberOfFamilyElements;ii++)
            familyArray[myFamilyElements[ii]-1]=familyNumber;
        }

        for (int i=0;i<numberOfElements;i++)
          SCRUTE_MED(familyArray[i]);

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        vector< med_int > temp( familyArray, familyArray + numberOfElements);
#endif
        int offset=0;
        for (int i=0; i<numberOfTypes; i++)
        {
          int typeNumberOfElements = _ptrMesh->getNumberOfElements(entity,types[i]);
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
          err=MEDmeshEntityFamilyNumberWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,::MED_CELL,(med_geometry_type) types[i],typeNumberOfElements,&temp[0]+offset);
#else
          err=MEDmeshEntityFamilyNumberWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,::MED_CELL,(med_geometry_type) types[i],typeNumberOfElements,familyArray+offset);
#endif
          if( err != MED_VALID)
            {
              if( _ptrMesh->getIsAGrid() )
                {
                  if( numberOfFamilies > 0 )
                    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<
                                                 "Groups and families of FACEs in the structured "
                                                 "mesh are not supported by med file" ));
                }
              else
                {
                  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<
                                               "Can't write family for the |"<< typeNumberOfElements
                                               << "| faces of geometric type |" << geoNames[types[i]]
                                               << "| in mesh |" << _meshName << "|" ));
                }
            }
          offset+=typeNumberOfElements;
        }
        delete[] familyArray;
      }
    }

    if(!_ptrMesh->getIsAGrid() || _ptrMesh->getMeshDimension() > 1 ) // for grid - only in 3D and 2D mesh

    { // EDGE RELATED BLOCK
      medEntityMesh entity=MED_EN::MED_EDGE;
      // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
      if((_ptrMesh->getIsAGrid()) || // PAL18712, GRID::existConnectivity() calls GRID::fillConnectivity() but it is not needed for writing GRID
           (((MESH*)_ptrMesh)->existConnectivity(MED_EN::MED_NODAL,entity) ) ||
           (((MESH*)_ptrMesh)->existConnectivity(MED_EN::MED_DESCENDING,entity) ) ) {

        int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity);
        const medGeometryElement  * types = _ptrMesh->getTypes         (entity);

        int numberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS);
        int * familyArray = new int[numberOfElements];
        for (int i=0;i<numberOfElements;i++)
          familyArray[i]=0;

        int numberOfFamilies = _ptrMesh->getNumberOfFamilies(entity);
        vector<FAMILY*> * myFamilies = &_ptrMesh->_familyEdge;
        if(0 == numberOfFamilies) {
          vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);

          groupFamilyConverter(myGroups,*myFamilies);

          numberOfFamilies=myFamilies->size();

        }

        for (int i=0;i<numberOfFamilies;i++) {
          int familyNumber = (*myFamilies)[i]->getIdentifier();
          int numberOfFamilyElements = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS);
          const int * myFamilyElements = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS);
          for (int ii=0;ii<numberOfFamilyElements;ii++)
            familyArray[myFamilyElements[ii]-1]=familyNumber;
        }


        for (int i=0;i<numberOfElements;i++)
          SCRUTE_MED(familyArray[i]);


        int typeCount = 0;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        vector< med_int > temp (familyArray, familyArray+numberOfElements );
#endif

        for (int i=0; i<numberOfTypes; i++)
          {
            int typeNumberOfElements = _ptrMesh->getNumberOfElements(entity, types[i]);
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
            err=MEDmeshEntityFamilyNumberWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,::MED_CELL, types[i],typeNumberOfElements,&temp[0]+typeCount);
#else
            err=MEDmeshEntityFamilyNumberWr(_medIdt,_meshName.c_str(),MED_NO_DT,MED_NO_IT,::MED_CELL, types[i],typeNumberOfElements,familyArray+typeCount);
#endif
            if( err != MED_VALID)
              {
                if( _ptrMesh->getIsAGrid() )
                  {
                    if( numberOfFamilies > 0 )
                      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<
                                                   "Groups and families of EDGEs in the structured "
                                                   " mesh are not supported by med file" ));
                  }
                else
                  {
                    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<  "Can't write family for the |"
                                                 << _ptrMesh->getNumberOfElements(entity, types[i])
                                                 << "| edges of geometric type |"
                                                 << geoNames[types[i]] <<"|in mesh |"
                                                 << _ptrMesh->_name.c_str() << "|" ));
                  }
              }
            typeCount += typeNumberOfElements;
          }
        delete[] familyArray;
      }
    }

    END_OF_MED(LOC);
    return MED_VALID;
}

int MED_MESH_WRONLY_DRIVER::writeFamilies(vector<FAMILY*> & families ) const
{

    const char * LOC="int MED_MESH_WRONLY_DRIVER::writeFamilies(vector<FAMILY*> families) const : ";
    BEGIN_OF_MED(LOC);

    med_err err;

    MESSAGE_MED(LOC<<" families.size() :"<<families.size());

    for (unsigned int i=0; i< families.size(); i++) {

      int      numberOfAttributes = families[i]->getNumberOfAttributes ();
      string   attributesDescriptions (numberOfAttributes*MED_COMMENT_SIZE,'\0');

      // Recompose the attributes descriptions arg for MED
      for (int j=0; j < numberOfAttributes; j++)
      {
        string attributeDescription = families[i]->getAttributeDescription(j+1);

        if( attributeDescription.size() > MED_COMMENT_SIZE )
          throw MEDEXCEPTION( LOCALIZED(STRING(LOC) << "The size of the attribute description n° |"
                                        << j+1 << "| of the family |" << families[i]->getName()
                                        << "| with identifier |" << families[i]->getIdentifier()
                                        << "| is |" << attributeDescription.size()
                                        <<"| and is more than |" <<  MED_COMMENT_SIZE << "|"));

        int length = min(MED_COMMENT_SIZE,(int)attributeDescription.size());
        attributesDescriptions.replace(j*MED_COMMENT_SIZE,length, attributeDescription,0,length);
      }


      int      numberOfGroups  = families[i]->getNumberOfGroups();
      string   groupsNames(numberOfGroups*MED_LNAME_SIZE,'\0');

      // Recompose the groups names arg for MED
      for (int j=0; j < numberOfGroups; j++)
      {
        string groupName = families[i]->getGroupName(j+1);

        if( groupName.size() > MED_LNAME_SIZE )
          throw MEDEXCEPTION( LOCALIZED(STRING(LOC) << "The size of the group name  n° |"
                                        << j+1 << "| of the family |" << families[i]->getName()
                                        << "| with identifier |" << families[i]->getIdentifier()
                                        << "| is |" << groupName.size()
                                        <<"| and is more than |" << MED_LNAME_SIZE  << "|"));

        int length = min(MED_LNAME_SIZE,(int)groupName.size());
        groupsNames.replace(j*MED_LNAME_SIZE,length, groupName,0,length);
      }

      // test if the family already exists (HDF trick waiting a MED evolution to be replaced)

      string dataGroupFam;
      if(families[i]->getEntity() == MED_EN::MED_NODE)
        dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/NOEUD/"+families[i]->getName()+"/";
      else
        dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/ELEME/"+families[i]->getName()+"/";

      SCRUTE_MED("|"<<dataGroupFam<<"|");
      err = _MEDdatagroupOuvrir(_medIdt,const_cast <char *> (dataGroupFam.c_str()) );
      if( err < MED_VALID ) {
        SCRUTE_MED(err);
        if( families[i]->getName().size() > MED_NAME_SIZE )
          throw MEDEXCEPTION
            ( LOCALIZED(STRING(LOC) << "The size of the name of the family |" << i+1
                        << "| |" << families[i]->getName()
                        << "| with identifier |" << families[i]->getIdentifier()  << "| is |"
                        <<  families[i]->getName().size()  <<"| and is more than |"
                        << MED_NAME_SIZE << "|"));

        MESSAGE_MED(LOC<<"families[i]->getName().c_str() : "<<families[i]->getName().c_str());
        MESSAGE_MED(LOC<<"_meshName.c_str() : "<<_meshName.c_str());
        MESSAGE_MED(LOC<<"families[i]->getIdentifier() : "<<families[i]->getIdentifier());
        MESSAGE_MED(LOC<<"numberOfAttributes : "<<numberOfAttributes);

        MESSAGE_MED(LOC<<"attributesDescriptions.c_str() : "<<attributesDescriptions.c_str());
        MESSAGE_MED(LOC<<"numberOfGroups : "<<numberOfGroups);
        MESSAGE_MED(LOC<<"groupsNames.c_str() : "<<groupsNames.c_str());
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        int lgth=families[i]->getNumberOfAttributes();
        med_int *  AttributesIdentifier2 = new med_int[lgth];
        med_int *  AttributesValues2     = new med_int[lgth];
        for(med_int i2=0;i2<lgth;i2++)
          {
            AttributesIdentifier2[i2]=(med_int)(families[i]->getAttributesIdentifiers()[i2]);
            AttributesValues2[i2]=(med_int)(families[i]->getAttributesValues()[i2]);
          }
        err = MEDfamilyCr(_medIdt,_meshName.c_str(),healName(families[i]->getName()).c_str(),
                                   families[i]->getIdentifier(),numberOfGroups,groupsNames.c_str());
        /*err = MEDfamCr( _medIdt,
                                const_cast <char *> ( _meshName.c_str() ),
                                const_cast <char *> ( healName(families[i]->getName()).c_str() ),
                                families[i]->getIdentifier(),
                                AttributesIdentifier2,
                                AttributesValues2,
                                const_cast <char *> (attributesDescriptions.c_str()),
                                numberOfAttributes,
                                const_cast <char *> (groupsNames.c_str()),
                                numberOfGroups);*/
        delete [] AttributesIdentifier2;
        delete [] AttributesValues2;
#else
        err = MEDfamilyCr(_medIdt,_meshName.c_str(),healName(families[i]->getName()).c_str(),
                                   families[i]->getIdentifier(),numberOfGroups,groupsNames.c_str());
        /*const_cast <char *> ( healName(families[i]->getName()).c_str() ),
          families[i]->getIdentifier(),
          (med_int*)families[i]->getAttributesIdentifiers(),
          (med_int*)families[i]->getAttributesValues(),
          const_cast <char *> (attributesDescriptions.c_str()),
          numberOfAttributes,
          const_cast <char *> (groupsNames.c_str()),
          numberOfGroups); -------- attributes for MED3 ...*/
#endif
        SCRUTE_MED(err);
        if(err != MED_VALID)
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't create family |" << families[i]->getName()
                                       << "| with identifier |" << families[i]->getIdentifier()
                                       << "| groups names |" << groupsNames
                                       << "| and  attributes descriptions |" << attributesDescriptions << "|"));
      }
      else
        _MEDdatagroupFermer(_medIdt);

    }

    END_OF_MED(LOC);

    return MED_VALID;
  }




/*--------------------- RDWR PART -------------------------------*/

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER()
  {
    this->GENDRIVER::_accessMode = MED_EN::RDWR;
  }

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName,
                                             GMESH *        ptrMesh):
    MED_MESH_DRIVER(fileName,ptrMesh,RDWR),
    MED_MESH_RDONLY_DRIVER(fileName,ptrMesh),
    MED_MESH_WRONLY_DRIVER(fileName,ptrMesh)
  {
    MESSAGE_MED("MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
  }

MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const MED_MESH_RDWR_DRIVER & driver):
    MED_MESH_DRIVER(driver),
    MED_MESH_RDONLY_DRIVER(driver),
    MED_MESH_WRONLY_DRIVER(driver)
  {}

MED_MESH_RDWR_DRIVER::~MED_MESH_RDWR_DRIVER()
  {
    //MESSAGE_MED("MED_MESH_RDWR_DRIVER::MED_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
  }

GENDRIVER * MED_MESH_RDWR_DRIVER::copy(void) const
  {
    return new MED_MESH_RDWR_DRIVER(*this);
  }

void MED_MESH_RDWR_DRIVER::write(void) const
  {
    MED_MESH_WRONLY_DRIVER::write();
  }
void MED_MESH_RDWR_DRIVER::read (void)
  {
    MED_MESH_RDONLY_DRIVER::read();
  }
