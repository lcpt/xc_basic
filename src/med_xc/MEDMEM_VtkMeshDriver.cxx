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

#include "MEDMEM_VtkMeshDriver.hxx"

#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_DriverFactory.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Mesh.hxx"

#include <fcntl.h>
#include <sstream>

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

VTK_MESH_DRIVER::VTK_MESH_DRIVER(): GENDRIVER(VTK_DRIVER), 
                                    _ptrMesh((MESH * const)MED_NULL)
{
  //_vtkFile = new ofstream();
  _vtkFile = 0;
  _binaryFile = 0;
  // What about _id in Gendriver ?
  // _driverType ???
}

VTK_MESH_DRIVER::VTK_MESH_DRIVER(const string & fileName, const GMESH *  ptrMesh) :
  GENDRIVER(fileName, WRONLY, VTK_DRIVER),
  _ptrMesh(ptrMesh)
{

  // Send an exception because a VTK_MESH_DRIVER object cannot be instantied
  // from a file and there is no read for that kind of driver

  //  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "This driver is only used to write in VTK format So thie object can not be instantied using a file!"));

  //  _ptrMesh->addDriver(*this); // OU RECUPERER L'ID.
  MESSAGE_MED("VTK_MESH_DRIVER::VTK_MESH_DRIVER(const string & fileName, MESH * ptrMesh) : "
          << "WARNING this driver is only used to write in VTK format So the object can not be instantied using a file!");

  //_vtkFile = new ofstream();
  _vtkFile = 0;
  _binaryFile = 0;
}

VTK_MESH_DRIVER::VTK_MESH_DRIVER(const VTK_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  _meshName(driver._meshName)
{
  // next string commented by skl for bug NPAL14840
  //_ptrMesh->addDriver(*this);
  //_vtkFile = new ofstream();
  _vtkFile = 0;
  _binaryFile = 0;
}

VTK_MESH_DRIVER::~VTK_MESH_DRIVER()
{
  const char* LOC = "VTK_MESH_DRIVER::~VTK_MESH_DRIVER()";
  BEGIN_OF_MED(LOC);

  close();

  SCRUTE_MED(_vtkFile);

  if( _vtkFile )
    delete _vtkFile ;
  if( _binaryFile )
    delete _binaryFile;

  _vtkFile = 0;
  _binaryFile = 0;

  SCRUTE_MED(_vtkFile);

  END_OF_MED(LOC);
}

void VTK_MESH_DRIVER::openConst() const throw(MEDEXCEPTION)
{
  const char * LOC = "VTK_MESH_DRIVER::openConst() " ;
  BEGIN_OF_MED(LOC);

  MESSAGE_MED(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< _accessMode);

  if( _fileName == "" )
    throw MED_EXCEPTION( LOCALIZED( STRING(LOC) 
                                     << "_fileName is |\"\"|, please set a correct fileName before calling open()"));

  if( DRIVERFACTORY::getVtkBinaryFormatForWriting() )
    {
      if( _vtkFile )
        {
          closeConst();
          delete _vtkFile;
          _vtkFile = 0;
        }
      if( !_binaryFile )
        {
          _binaryFile = new _VTK_BinaryWriter( _fileName );
          if(!_binaryFile->open())
            {
              delete _binaryFile;
              _binaryFile = 0;
              throw MED_EXCEPTION( LOCALIZED( STRING(LOC) << "Could not open file "<< _fileName));
            }
        }
    }
  else
    {
      if( _binaryFile )
        {
          closeConst();
          delete _binaryFile;
          _binaryFile = 0;
        }
 
      if(!_vtkFile )
        _vtkFile = new ofstream();
      if(!(*_vtkFile).is_open())
        (*_vtkFile).open(_fileName.c_str()) ; 

      if(!(*_vtkFile))
        {
          delete _vtkFile;
          _vtkFile = 0;
          throw MED_EXCEPTION( LOCALIZED( STRING(LOC) << "Could not open file "<< _fileName));
        }
    }
  END_OF_MED(LOC);
}

void VTK_MESH_DRIVER::open() {
  openConst() ;
}

void VTK_MESH_DRIVER::closeConst() const throw(MEDEXCEPTION)
{
  const char * LOC = "VTK_MESH_DRIVER::closeConst() " ;
  BEGIN_OF_MED(LOC);

  if( _vtkFile )
    {
      if((*_vtkFile).is_open())
        (*_vtkFile).close();
  
      if( (*_vtkFile) && _vtkFile->is_open() )
        throw MED_EXCEPTION( LOCALIZED( STRING(LOC) << "Could not close file "<< _fileName));
    }
  if( _binaryFile )
    {
      _binaryFile->close();
      delete _binaryFile;
      ((VTK_MESH_DRIVER*)this)->_binaryFile = 0;
    }
  END_OF_MED(LOC);
}

void VTK_MESH_DRIVER::close() {
  closeConst() ;
}

void    VTK_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; }
string  VTK_MESH_DRIVER::getMeshName() const { return _meshName; }

void VTK_MESH_DRIVER::read(void) throw(MEDEXCEPTION)
{
  const char * LOC = "VTK_MESH_DRIVER::read() : " ;
  BEGIN_OF_MED(LOC);

  // Send an exception

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "This driver is only used to write in VTK format !"));

  END_OF_MED(LOC);
}


void VTK_MESH_DRIVER::write(void) const
  throw(MEDEXCEPTION)
  {

    const char * LOC = "void VTK_MESH_DRIVER::write(void) const : ";
    BEGIN_OF_MED(LOC);

    if( !_ptrMesh )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Mesh is NULL"));

    // Well we must open vtk file first, because there are
    // no other driver than MED for VTK that do it !

    const int SpaceDimension= _ptrMesh->getSpaceDimension() ;
    const int NumberOfNodes= _ptrMesh->getNumberOfNodes() ;
    if( SpaceDimension < 1 )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Mesh is empty"));

    openConst();

    const MESH* mesh = _ptrMesh->convertInMESH();
    char buf[256];
    const char* header = "# vtk DataFile Version 2.0\n";
    const char* title  = "maillage from MedMemory\n";
    const char* dataset = "DATASET UNSTRUCTURED_GRID\n";
    if( _vtkFile ) // ASCII
      {
        (*_vtkFile) << header ;
        (*_vtkFile) << title ;
        (*_vtkFile) << "ASCII" << endl ;
        (*_vtkFile) << dataset ;
        // put points(all point are in 3D, so if we are in 1D or 2D, we complete by zero !
        (*_vtkFile) << "POINTS " << NumberOfNodes << " float" << endl ;
        const double *coordinate = mesh->getCoordinates(MED_EN::MED_FULL_INTERLACE) ;
        string missingCoord = SpaceDimension==3 ? "" : SpaceDimension==2 ? "0" : "0 0";
        for(int i=0;i<NumberOfNodes;i++)
          {
            for(int j=0;j<SpaceDimension;j++)
              (*_vtkFile) << coordinate[i*SpaceDimension+j] << " ";
            (*_vtkFile) << missingCoord << endl;
          }
      }
    else // BINARY
      {
        const char* format = "BINARY\n";
        writeBinary( header,  strlen(header) );
        writeBinary( title,   strlen(title) );
        writeBinary( format,  strlen(format) );
        writeBinary( dataset, strlen(dataset) );

        // put points(all point are in 3D, so if we are in 1D or 2D, we complete by zero !
        //sprintf(buf,"POINTS %d double\n", NumberOfNodes);
        sprintf(buf,"POINTS %d float\n", NumberOfNodes);
        writeBinary( buf, strlen(buf) );
        const double *coordinate = mesh->getCoordinates(MED_EN::MED_FULL_INTERLACE) ;
        //PointerOf<double> coordBuf( SpaceDimension==3 ? 0 : NumberOfNodes * 3 );
        //double * toCoord = coordBuf;
        PointerOf<float> coordBuf( NumberOfNodes * 3 );
        float *toCoord = coordBuf;
        switch( SpaceDimension )
          {
          case 3:
            for(int i=0;i<NumberOfNodes;i++)
              {
                *toCoord++ = (float)*coordinate++;
                *toCoord++ = (float)*coordinate++;
                *toCoord++ = (float)*coordinate++;
              }
            break;
          case 2:
            for(int i=0;i<NumberOfNodes;i++)
              {
                *toCoord++ = (float)*coordinate++;
                *toCoord++ = (float)*coordinate++;
                *toCoord++ = 0;
              }
            break;
          case 1:
            for(int i=0;i<NumberOfNodes;i++)
              {
                *toCoord++ = (float)*coordinate++;
                *toCoord++ = 0;
                *toCoord++ = 0;
              }
            break;
//           coordBuf.set( coordinate );
//           break;
//         case 2:
//           for(int i=0;i<NumberOfNodes;i++)
//             {
//               *toCoord++ = *coordinate++;
//               *toCoord++ = *coordinate++;
//               *toCoord++ = 0;
//             }
//           break;
//         case 1:
//           for(int i=0;i<NumberOfNodes;i++)
//             {
//               *toCoord++ = *coordinate++;
//               *toCoord++ = 0;
//               *toCoord++ = 0;
//             }
//           break;
          }
        //writeBinary( (double*) coordBuf, NumberOfNodes * 3 );
        writeBinary( (float*) coordBuf, NumberOfNodes * 3 );
      }

    // we put connectivity
    // how many cells and how many value in connectivity :
    const int cells_types_count= mesh->getNumberOfTypes(MED_EN::MED_CELL) ;
    const int cells_sum= mesh->getNumberOfElements(MED_EN::MED_CELL,MED_ALL_ELEMENTS) ;
    const CELLMODEL *cells_type= mesh->getCellsTypes(MED_EN::MED_CELL) ;

    const int *connectivityIndex = mesh->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL) ;
    const int connectivity_sum =  connectivityIndex[cells_sum]-1 ;

    sprintf(buf,"\nCELLS %d %d\n", cells_sum, connectivity_sum+cells_sum);
    if( _vtkFile )
      (*_vtkFile) << buf ;
    else
      writeBinary( buf, strlen(buf) );

    for(int i=0;i<cells_types_count;i++)
      {
        int *filter = (int*) nullptr ; // index in vtk connectivity
        switch(cells_type[i].getType())
          {
          case MED_EN::MED_POINT1:
            {
              filter = new int[1] ;
              filter[0] = 0 ;
              break ;
            }
          case MED_EN::MED_SEG2:
            {
              filter = new int[2] ;
              filter[0] = 0 ;
              filter[1] = 1 ;
              break ;
            }
          case MED_EN::MED_SEG3:
            {  
              break;
            }
          case MED_EN::MED_TRIA3:
            {
              filter = new int[3] ;
              filter[0] = 0 ;
              filter[1] = 1 ;
              filter[2] = 2 ;
              break;
            }
          case MED_EN::MED_QUAD4:
            {
              filter = new int[4] ;
              filter[0] = 0 ;
              filter[1] = 1 ;
              filter[2] = 2 ;
              filter[3] = 3 ;
              break ;
            }
            case MED_EN::MED_TRIA6   : {
              break ;
            }
      case MED_EN::MED_QUAD8   : {
              break ;
            }
      case MED_EN::MED_TETRA4  : {
              filter = new int[4] ;
              filter[0] = 0 ;
              filter[1] = 1 ;
              filter[2] = 3 ;  // 3td element in med are 4th in vtk (array begin at 0 !)
              filter[3] = 2 ;  // 4th element in med are 3rd in vtk (array begin at 0 !)
              break ;
            }
      case MED_EN::MED_PYRA5   : {
              filter = new int[5] ;
              filter[0] = 0 ;
              filter[1] = 3 ;  // 2nd element in med are 4th in vtk (array begin at 0 !)
              filter[2] = 2 ;
              filter[3] = 1 ;  // 4th element in med are 2nd in vtk (array begin at 0 !)
              filter[4] = 4 ;
              break ;
      }
      case MED_EN::MED_PENTA6  : {
              filter = new int[6] ;
              filter[0] = 0 ;
              filter[1] = 1 ;
              filter[2] = 2 ;
              filter[3] = 3 ;
              filter[4] = 4 ;
              filter[5] = 5 ;
              break ;
      }
      case MED_EN::MED_HEXA8   : {
              filter = new int[8] ;
              filter[0] = 0 ;
              filter[1] = 3 ;
              filter[2] = 2 ;
              filter[3] = 1 ;
              filter[4] = 4 ;
              filter[5] = 7 ;
              filter[6] = 6 ;
              filter[7] = 5 ;
              break ;
      }
      case MED_EN::MED_TETRA10 : {
        break ;
      }
      case MED_EN::MED_PYRA13  : {
        break ;
      }
      case MED_EN::MED_PENTA15 : {
        break ;
      }
      case MED_EN::MED_HEXA20  : {
        break ;
      }
      default : { 
        break ;
      }
      }
    if(filter==nullptr) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].getName() ) ) ;
    int nodes_cell = cells_type[i].getNumberOfNodes();
    int numberOfCell = mesh->getNumberOfElements(MED_EN::MED_CELL,cells_type[i].getType()) ;
    const int * connectivityArray = mesh->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,cells_type[i].getType());
    if( _vtkFile )
      {
        for(int j=0;j<numberOfCell;j++)
          {
            (*_vtkFile) << nodes_cell << " " ;
            for(int k=0;k<nodes_cell;k++)
              (*_vtkFile) << connectivityArray[j*nodes_cell+filter[k]] - 1 << " " ;
            (*_vtkFile) << endl ;
          }
      }
    else // BINARY
      {
        int dataSize = numberOfCell * ( nodes_cell+1 );
        PointerOf<int> cellData( dataSize );
        for(int *pCellData = cellData, j = 0; j < numberOfCell; j++)
          {
            *pCellData++ = nodes_cell;
            for(int k=0;k<nodes_cell;k++)
              *pCellData++ = connectivityArray[j*nodes_cell+filter[k]] - 1;
          }
        writeBinary( (int*) cellData, dataSize );
      }
    if(filter != nullptr)
      delete[] filter ;
  }
  sprintf(buf,"\nCELL_TYPES %d\n", cells_sum);
  if( _vtkFile )
    (*_vtkFile) << buf ;
  else
    writeBinary( buf, strlen(buf) );

  for(int i=0;i<cells_types_count;i++) {
    int vtkType = 0 ;
    switch(cells_type[i].getType())
      {
      case MED_EN::MED_POINT1  :vtkType = 1 ;break ;
      case MED_EN::MED_SEG2    :vtkType = 3 ;break ;
      case MED_EN::MED_SEG3    :vtkType = 0 ;break ;
      case MED_EN::MED_TRIA3   :vtkType = 5 ;break ;
      case MED_EN::MED_QUAD4   :vtkType = 9 ;break ;
      case MED_EN::MED_TRIA6   :vtkType = 0 ;break ;
      case MED_EN::MED_QUAD8   :vtkType = 0 ;break ;
      case MED_EN::MED_TETRA4  :vtkType = 10 ;break ;
      case MED_EN::MED_PYRA5   :vtkType = 14 ;break ;
      case MED_EN::MED_PENTA6  :vtkType = 13 ;break ;
      case MED_EN::MED_HEXA8   :vtkType = 12 ;break ;
      case MED_EN::MED_TETRA10 :vtkType = 0 ;break ;
      case MED_EN::MED_PYRA13  :vtkType = 0 ;break ;
      case MED_EN::MED_PENTA15 :vtkType = 0 ;break ;
      case MED_EN::MED_HEXA20  :vtkType = 0 ;break ;
      default          :vtkType = 0 ;break ;
      }
    if(vtkType == 0)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].getType() ) ) ;
    int numberOfCell = mesh->getNumberOfElements(MED_EN::MED_CELL,cells_type[i].getType()) ;
    if( _vtkFile )
      {
        for(int j=0;j<numberOfCell;j++)
          (*_vtkFile) << vtkType << endl ;
      }
    else
      {
        vector< int > type( numberOfCell, vtkType );
        writeBinary( &type[0], type.size() );
      }
  }
  mesh->removeReference();

  closeConst();

  END_OF_MED(LOC);
} 

GENDRIVER * VTK_MESH_DRIVER::copy(void) const
{
  return new VTK_MESH_DRIVER(*this);
}

//////////////////////////////////////////////////////////////////////////////////////////
// _VTK_BinaryWriter
//////////////////////////////////////////////////////////////////////////////////////////

_VTK_BinaryWriter::_VTK_BinaryWriter(const std::string file):_fileName(file),_binaryFile(0)
{
}

bool _VTK_BinaryWriter::open(bool append) const
{
  if( !_binaryFile )
    {
      // workaround to create file with good access rights
      if( !append )
        fstream(_fileName.c_str(), ios_base::out);

      // opening
      _VTK_BinaryWriter* mutableMe =(_VTK_BinaryWriter*) this;
      int append_flag = 0;
#ifdef WNT
      if( append ) append_flag = _O_APPEND;
      mutableMe->_binaryFile = ::_open(_fileName.c_str(), _O_WRONLY|_O_BINARY|append_flag);
#else
      if( append ) append_flag = O_APPEND;
      mutableMe->_binaryFile = ::open(_fileName.c_str(), O_WRONLY|append_flag);
#endif
      if(_binaryFile < 0)
        mutableMe->_binaryFile = 0;
    }
  return _binaryFile;
}

bool _VTK_BinaryWriter::close() const
{
  if( _binaryFile )
    {
#ifdef WNT
      ::_close(_binaryFile);
#else
      ::close(_binaryFile);
#endif
      ((_VTK_BinaryWriter*)this)->_binaryFile = 0;
    }
  return true;
}
