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

#include "MEDMEM_VtkMedDriver.hxx"

#include <sstream>

#include "MEDMEM_define.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

VTK_MED_DRIVER::VTK_MED_DRIVER(): GENDRIVER(VTK_DRIVER), _fields(0)
{
}


VTK_MED_DRIVER::VTK_MED_DRIVER(const string & fileName,
                               const vector< const FIELD_* >& fields):
  GENDRIVER(fileName, MED_EN::RDWR, VTK_DRIVER), _fields( fields )
{
}

VTK_MED_DRIVER::VTK_MED_DRIVER(const VTK_MED_DRIVER & driver):
  GENDRIVER(driver), _fields(driver._fields)
{
}

VTK_MED_DRIVER::~VTK_MED_DRIVER()
{
  const char* LOC = "VTK_MED_DRIVER::~VTK_MED_DRIVER()";
  BEGIN_OF_MED(LOC);

  END_OF_MED(LOC);
}

GENDRIVER * VTK_MED_DRIVER::copy() const
{
  return new VTK_MED_DRIVER(*this) ;
}

void VTK_MED_DRIVER::openConst() const
{
  const char * LOC ="VTK_MED_DRIVER::open() : ";
  BEGIN_OF_MED(LOC);

  if( _fileName == "" )
    throw MED_EXCEPTION( LOCALIZED( STRING(LOC) 
                                    << "_fileName is |\"\"|, please set a correct fileName before calling open()"));

  // check if can open the file
  ofstream _vtkFile;
  _vtkFile.open(_fileName.c_str()); 
  if(!_vtkFile)
    throw MED_EXCEPTION( LOCALIZED( STRING(LOC) << "Could not open file "<< _fileName));
  
  END_OF_MED(LOC);
}

void VTK_MED_DRIVER::open() {
  openConst() ;
}

void VTK_MED_DRIVER::closeConst() const {

  const char* LOC = "VTK_MED_DRIVER::close() : ";
  BEGIN_OF_MED(LOC);
  END_OF_MED(LOC);
}

void VTK_MED_DRIVER::close() {
  closeConst() ;
}


void VTK_MED_DRIVER::write() const
  {
    const char* LOC = "VTK_MED_DRIVER::write() : ";
    BEGIN_OF_MED(LOC);

    // VTK supports only one dataset per a file (in Simple Legacy Formats)
    // so we write the first mesh only

    const int NumberOfMeshes = ( !_fields.empty() ) ? 1 : 0;

    for(int i=0; i<NumberOfMeshes; i++)
      {
        const GMESH *myMesh = _fields.at(i)->getSupport()->getMesh();
        writeMesh(myMesh) ;
        for(unsigned j=0; j<_fields.size(); j++)
          {
            const FIELD_ * myField = _fields.at(j);
            if( myMesh == myField->getSupport()->getMesh() )
              {
                if(MED_EN::MED_NODE == myField->getSupport()->getEntity())
                  {
                    if(myField->getSupport()->isOnAllElements())
                      {
                        writeField(myField,STRING(myField->getName()) << "_" << myField->getIterationNumber() << "_" << myField->getOrderNumber() ) ;
                      }
                    else
                      {
                        MESSAGE_MED(PREFIX_MED << "Could not write field "<<myField->getName()<<" which is not on all nodes !");
                      }
                  }
              }
           }

      // second : field on cell
      for(unsigned j=0; j<_fields.size(); j++)
        {
          const FIELD_ *myField = _fields.at(j);
          if( myMesh == myField->getSupport()->getMesh() )
            if(MED_EN::MED_CELL == myField->getSupport()->getEntity())
              {
                if(myField->getSupport()->isOnAllElements())
                  {
                    writeField(myField,STRING(myField->getName()) << "_" << myField->getIterationNumber() << "_" << myField->getOrderNumber() );
                  }
                else
                  {
                    MESSAGE_MED(PREFIX_MED << "Could not write field "<<myField->getName()<<" which is not on all cells !");
                  }
              }
	}
      } // loop on meshes
    END_OF_MED(LOC);
  }

void VTK_MED_DRIVER::writeMesh(const GMESH * myMesh) const
{
  const char * LOC = "VTK_MED_DRIVER::writeMesh() : ";
  BEGIN_OF_MED(LOC);

  VTK_MESH_DRIVER meshDriver( _fileName, myMesh );
  meshDriver.write();

  END_OF_MED(LOC);
}

void VTK_MED_DRIVER::writeField(const FIELD_ * myField,string name) const
{
  const char* LOC = "VTK_MED_DRIVER::writeField() : ";
  BEGIN_OF_MED(LOC);

  MED_EN::med_type_champ type = myField->getValueType() ;
  GENDRIVER* driver = 0;
  switch(type)
    {
    case MED_EN::MED_INT32 :

      if( myField->getInterlacingType() == ::MED_FULL_INTERLACE )
        driver = new VTK_FIELD_DRIVER<int>(_fileName,
                                           static_cast< const FIELD<int,FullInterlace>* >(myField));

      else if( myField->getInterlacingType() == MED_NO_INTERLACE_BY_TYPE )
        driver = new VTK_FIELD_DRIVER<int>(_fileName,
                                           static_cast< const FIELD<int,NoInterlaceByType>* >(myField));

      else
        driver = new VTK_FIELD_DRIVER<int>(_fileName,
                                           static_cast< const FIELD<int,NoInterlace>* >(myField));
      break;

    case MED_EN::MED_REEL64 : 

      if( myField->getInterlacingType() == ::MED_FULL_INTERLACE )
        driver = new VTK_FIELD_DRIVER<double>(_fileName,
                                              static_cast< const FIELD<double,FullInterlace>* >(myField));

      else if( myField->getInterlacingType() == MED_NO_INTERLACE_BY_TYPE )
        driver = new VTK_FIELD_DRIVER<double>(_fileName,
                                              static_cast< const FIELD<double,NoInterlaceByType>*>(myField));

      else
        driver = new VTK_FIELD_DRIVER<double>(_fileName,
                                              static_cast< const FIELD<double,NoInterlace>* >(myField));
      break;

    default :
      {
        MESSAGE_MED(PREFIX_MED << "Could not write field "<<name<<" the type is not int or double !");
      }
    }

  if( driver )
    {
      driver->writeAppend();
      delete driver;
    }
  END_OF_MED(LOC);
}

// void VTK_MED_DRIVER::writeSupport(SUPPORT * mySupport) const {
//   const char* LOC = "VTK_MED_DRIVER::writeSupport(SUPPORT *) : ";
//   BEGIN_OF_MED(LOC);
//   MESSAGE_MED(PREFIX_MED << "Not yet implemented, acting on the object " << *mySupport);
//   END_OF_MED(LOC);
// }
