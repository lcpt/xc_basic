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

#include "MEDMEM_PorflowMeshDriver.hxx"
#include "MEDMEM_DriversDef.hxx"
#include "MEDMEM_DriverTools.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_define.hxx"
#include <string.h>
#include <ctype.h>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;
using namespace MED_EN;
using namespace MEDMEM;

// geometric types conversion from PORFLOW -> MED
//const size_t PORFLOW_MESH_DRIVER::nb_geometrie_porflow;
const medGeometryElement PORFLOW_MESH_DRIVER::geomPORFLOWtoMED[nb_geometrie_porflow] =
  {   /*  1  */ MED_EN::MED_TRIA3   ,/*  2  */ MED_EN::MED_QUAD4  ,/*  3  */ MED_EN::MED_TETRA4  ,/*  4  */ MED_EN::MED_PYRA5  ,
      /*  5  */ MED_EN::MED_PENTA6  ,/*  6  */ MED_EN::MED_HEXA8   };
// indirection table from PORFLOW order to MED one for nodes numerotation in all PORFLOW geometries
//const size_t PORFLOW_MESH_DRIVER::nb_nodes_max;

// conversion from porflow connectivity to MED connectivity
const int PORFLOW_MESH_DRIVER::numPORFLOWtoMED[nb_geometrie_porflow] [nb_nodes_max] = {
      /*  TRI3   */ { /*1*/ 1, /*2*/ 3, /*3*/ 2 },
      /*  QUA4   */ { /*1*/ 1, /*2*/ 4, /*3*/ 3, /*4*/ 2 },
      /*  TETRA4 */ { /*1*/ 1, /*2*/ 4, /*3*/ 3, /*4*/ 2 },
      /*  PYRA5  */ { /*1*/ 1, /*2*/ 4, /*3*/ 3, /*4*/ 2, /*5*/ 5 },
      /*  PENTA6 */ { /*1*/ 1, /*2*/ 3, /*3*/ 2, /*4*/ 4, /*5*/ 6, /*6*/ 5 },
      /*  HEXA8  */ { /*1*/ 1, /*2*/ 4, /*3*/ 3, /*4*/ 2, /*5*/ 5, /*6*/ 8, /*7*/ 7, /*8*/ 6 } };

// Porflox nodal connectivity of faces - documented in Porflow user's manual
//const size_t PORFLOW_MESH_DRIVER::nb_faces_max;
//const size_t PORFLOW_MESH_DRIVER::nb_nodes2_max;
const int PORFLOW_MESH_DRIVER::connectivityPORFLOW[nb_geometrie_porflow][nb_faces_max][nb_nodes2_max]={
  //{/* TRI3   */ {3,1,0,0}, {1,2,0,0}, {2,3,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0} },
  //{/* QUA4   */ {1,4,0,0}, {2,3,0,0}, {1,2,0,0}, {3,4,0,0}, {0,0,0,0}, {0,0,0,0} },
    {/* TRI3   */ {1,3,0,0}, {2,1,0,0}, {3,2,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0} },
    {/* QUA4   */ {1,4,0,0}, {3,2,0,0}, {2,1,0,0}, {4,3,0,0}, {0,0,0,0}, {0,0,0,0} },
    {/* TETRA4 */ {1,3,4,0}, {1,4,2,0}, {4,3,2,0}, {1,2,3,0}, {0,0,0,0}, {0,0,0,0} },
    {/* PYRA5  */ {1,4,5,0}, {1,5,2,0}, {3,2,5,0}, {4,3,5,0}, {1,2,3,4}, {0,0,0,0} },
    {/* PENTA6 */ {1,3,6,4}, {2,1,4,5}, {3,2,5,6}, {1,2,3,0}, {4,6,5,0}, {0,0,0,0} },
    {/* HEXA8  */ {1,4,8,5}, {3,2,6,7}, {2,1,5,6}, {4,3,7,8}, {1,2,3,4}, {5,8,7,6} },
};

// helper function
inline static bool isKeyWord(const string & line, const char * key);
inline static void locateFileName(const string& line, string::size_type& pos1, string::size_type& pos2);
inline static string getPorflowFileName(const string& line, const char * context);
inline static medGeometryElement get2DMedGeomType(int nbSommets);


// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor 

PORFLOW_MESH_DRIVER::PORFLOW_MESH_DRIVER():
  GENDRIVER(PORFLOW_DRIVER),
  _ptrMesh(( MESH *) NULL),
  // A VOIR _medIdt(MED_INVALID),
  _meshName("")
{
}

PORFLOW_MESH_DRIVER::PORFLOW_MESH_DRIVER(const string & fileName,
                                         GMESH *        ptrMesh,
                                         MED_EN::med_mode_acces accessMode): 
  GENDRIVER(fileName, accessMode, PORFLOW_DRIVER),
  _ptrMesh(0)
{
  if (ptrMesh)
    _ptrMesh = const_cast<MESH*>( ptrMesh->convertInMESH() );
  // mesh name construction from fileName
  const string ext=".inp"; // expected extension
  string::size_type pos=fileName.find(ext,0);
  string::size_type pos1=fileName.rfind('/');
#ifdef WNT
  if (pos1 == string::npos ) pos1=fileName.rfind('\\');
#endif
  if (pos1 != string::npos )
    _meshName = string(fileName,pos1+1,pos-pos1-1); //get rid of directory & extension
  else
    _meshName = string(fileName,0,pos); //get rid of directory & extension
  SCRUTE_MED(_meshName);
}

PORFLOW_MESH_DRIVER::PORFLOW_MESH_DRIVER(const PORFLOW_MESH_DRIVER & driver): 
  GENDRIVER(driver),
  _ptrMesh(driver._ptrMesh),
  // A VOIR _medIdt(MED_INVALID), 
  _meshName(driver._meshName)
{
  if (_ptrMesh)
    _ptrMesh->addReference();
}

PORFLOW_MESH_DRIVER::~PORFLOW_MESH_DRIVER()
{
  if (_ptrMesh)
    _ptrMesh->removeReference();
}

void PORFLOW_MESH_DRIVER::open()
  throw (MEDEXCEPTION)
{
    const char * LOC = "PORFLOW_MESH_DRIVER::open()" ;
    BEGIN_OF_MED(LOC);

    if (_status == MED_OPENED)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"File "<<_fileName<<" is already open"));

    _porflow.open(_fileName.c_str(), ios::in);
    if(_porflow)
        _status = MED_OPENED;
    else
    {
        _status = MED_CLOSED;
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName<<" in mode ios::in"));
    }
  END_OF_MED(LOC);
}
  
void PORFLOW_MESH_DRIVER::close()
  throw (MEDEXCEPTION)
{
  const char* LOC = "PORFLOW_MESH_DRIVER::close() ";
  BEGIN_OF_MED(LOC);
  if ( _status == MED_OPENED) {
    _porflow.close();
    _status = MED_CLOSED;
  }
  END_OF_MED(LOC);
}

void    PORFLOW_MESH_DRIVER::setMeshName(const string & meshName) { _meshName = meshName; }
string  PORFLOW_MESH_DRIVER::getMeshName() const { return _meshName; }


//---------------------------------- RDONLY PART -------------------------------------------------------------

PORFLOW_MESH_RDONLY_DRIVER::PORFLOW_MESH_RDONLY_DRIVER(): PORFLOW_MESH_DRIVER()
{
}
  
PORFLOW_MESH_RDONLY_DRIVER::PORFLOW_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh):
  PORFLOW_MESH_DRIVER(fileName,ptrMesh,RDONLY)
{
  MESSAGE_MED("PORFLOW_MESH_RDONLY_DRIVER::PORFLOW_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}
  
PORFLOW_MESH_RDONLY_DRIVER::PORFLOW_MESH_RDONLY_DRIVER(const PORFLOW_MESH_RDONLY_DRIVER & driver): 
  PORFLOW_MESH_DRIVER(driver)
{
}

PORFLOW_MESH_RDONLY_DRIVER::~PORFLOW_MESH_RDONLY_DRIVER()
{
  //MESSAGE_MED("PORFLOW_MESH_RDONLY_DRIVER::~PORFLOW_MESH_RDONLY_DRIVER() has been destroyed");
}
  
GENDRIVER * PORFLOW_MESH_RDONLY_DRIVER::copy(void) const
{
  return new PORFLOW_MESH_RDONLY_DRIVER(*this);
}

// helper function to check if line starts with key
inline static bool isKeyWord(const string & line, const char * key)
{
    const int SIZE_KEY=4;
    const char * pt=line.c_str();
    while( isspace(*pt) ) // get rid of leading blanks
        ++pt;
    return !strncmp(pt,key,SIZE_KEY);
}

inline static void locateFileName(const string& line, string::size_type& pos1, string::size_type& pos2)
{
    pos1 = line.find('"',0);
    if ( pos1 != string::npos ) // if we found "
        pos2=line.find('"',pos1+1); // search a second "
    else // we didn't succeed with double quotes, so we try single quotes
    {
        pos1 = line.find('\'',0);
        if ( pos1 != string::npos ) // if we found '
            pos2=line.find('\'',pos1+1); // search a second '
    }
}


// helper fonction : inversion of PORFLOW_MESH_DRIVER::geomPORFLOWtoMED
// return the Porflow geometric type, -1 if not found
inline int PORFLOW_MESH_DRIVER::geomMEDtoPorflow(medGeometryElement medGeo)
{
    for (int g=0; g!=nb_geometrie_porflow; ++g)
        if( medGeo == geomPORFLOWtoMED[g] )
            return g;
    return -1;
}


// Correspondance between the number of nodes and the Med face geometric type
inline static medGeometryElement get2DMedGeomType(int nbSommets)
{
    switch (nbSommets)
    {
        case 2:
            return MED_EN::MED_SEG2;
            break;
        case 3:
            return MED_EN::MED_TRIA3;
            break;
        case 4:
            return MED_EN::MED_QUAD4;
            break;
    }
    return MED_EN::MED_NONE;
}

// helper function to read a porflow file name (enclosed in single or double quotes) in line
inline static string getPorflowFileName(const string& line, const char * context)
{
    string::size_type pos1=string::npos;
    string::size_type pos2=string::npos;

    locateFileName(line, pos1, pos2);

    // if we found a not empty filename
    if (pos1 != string::npos && pos2 != string::npos && pos2>pos1+1)
        return string(line,pos1+1,pos2-pos1-1);

    // else, we throw an exception
    string diagnosis("PORFLOW_MESH_RDONLY_DRIVER::read() : Error after reading keyword ");
    diagnosis+=context;
    diagnosis+="\nNo filename was found enclosed in single or double quotes\n";
    throw MEDEXCEPTION(diagnosis.c_str());
}


// helper function to read a porflow coordinate file ( .xyz )
void PORFLOW_MESH_RDONLY_DRIVER::readPorflowCoordinateFile(const string & coorFileName,_intermediateMED & medi,const int space_dimension)
{

    ifstream coorFile(coorFileName.c_str(), ios::in);
    if(!coorFile)
    {
        string diagnosis("PORFLOW_MESH_RDONLY_DRIVER::read()\nError, can't open coordinates file :");
        diagnosis+=coorFileName;
        throw MEDEXCEPTION(diagnosis.c_str());
    }
    
    // scan of the coord file
    _noeud node;
    node.coord.resize(space_dimension);
    string buf_ligne; // to read a line
    while ( getline(coorFile, buf_ligne) ) // we can't use input stream directly because some 2D files have "0.0" in third coordinate
      {
        istringstream buf(buf_ligne.c_str());
        buf >> node.number;
        if (!buf) // for spaces at the end of the file
          break;
        for (int i=0; i!=space_dimension; ++i)
          buf >> node.coord[i];
        medi.points.insert(make_pair(node.number,node));
      }
    coorFile.close();
}

// helper function to read a porflow connectivity file ( .cnc )
void PORFLOW_MESH_RDONLY_DRIVER::readPorflowConnectivityFile(bool hybride,const string & connecFileName,_intermediateMED & medi,std::vector<std::set<_maille>::iterator>& p_ma_table,int mesh_dimension)
{

    ifstream connFile(connecFileName.c_str(), ios::in);
    if(!connFile)
    {
        string diagnosis("PORFLOW_MESH_RDONLY_DRIVER::read()\nError, can't open connectivity file :");
        diagnosis+=connecFileName;
        throw MEDEXCEPTION(diagnosis.c_str());
    }

    _maille maille;
    unsigned int code, nodes_number, node, ordre;
    set<_maille>::iterator p_ma;

    if (hybride) // "HYBR" key-word
      while (connFile)
        {
          connFile >> ordre;
          maille.setOrdre( ordre );
          if (!connFile) // for spaces at the end of the file
            break;
          connFile >> code;
          connFile >> nodes_number;
          maille.geometricType = geomPORFLOWtoMED[code-1];
          if(maille.geometricType%100!=(int)nodes_number) // following incohenrences founded in some porflow files!
          {
              MESSAGE_MED("WARNING : the read node number don't seem to be compatible with geometric type!");
              SCRUTE_MED(maille.geometricType);
              SCRUTE_MED(nodes_number);
          }
          maille.sommets.resize(nodes_number);
          for (unsigned i=0; i!=nodes_number; ++i)
            {
              connFile >> node;
              maille.sommets[numPORFLOWtoMED[code-1][i]-1] = medi.points.find(node);
            }
          p_ma = medi.insert(maille);
          if (maille.ordre() > p_ma_table.size()-1) // construction of a vector of iterators on _maille structures
            p_ma_table.resize(2*maille.ordre());
          p_ma_table[maille.ordre()] = p_ma;
        }
    else // default case (or "VERT" key-word)
      {
        if (mesh_dimension == 2) // we have QUA4
          {
            code = 2;
            maille.geometricType = geomPORFLOWtoMED[code-1];
            nodes_number = 4;
            maille.sommets.resize(nodes_number);
          }
        else if (mesh_dimension == 3) // we have HEXA8
          {
            code = 6;
            maille.geometricType = geomPORFLOWtoMED[code-1];
            nodes_number = 8;
            maille.sommets.resize(nodes_number);
          }
        else
          throw MEDEXCEPTION("PORFLOW_MESH_RDONLY_DRIVER::read()\nError, can't determine geometric type for this VERT mesh");
        while (connFile)
          {
            connFile >> ordre; maille.setOrdre(ordre);
            if (!connFile) // for spaces at the end of the file
              break;
            for (unsigned i=0; i!=nodes_number; ++i)
              {
                connFile >> node;
                maille.sommets[numPORFLOWtoMED[code-1][i]-1] = medi.points.find(node);
              }
            p_ma = medi.insert(maille);
            if (maille.ordre() > p_ma_table.size()-1) // construction of a vector of iterators on _maille structures
              p_ma_table.resize(2*maille.ordre());
            p_ma_table[maille.ordre()] = p_ma;
          }
      }
    connFile.close();
}

void PORFLOW_MESH_RDONLY_DRIVER::read(void)
  throw (MEDEXCEPTION)
{
    const char * LOC = "PORFLOW_MESH_RDONLY_DRIVER::read() : " ;
    BEGIN_OF_MED(LOC);

    if (_status!=MED_OPENED)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName 
                    << " is : "  <<  " (the file is not opened)." )) ;

    // if _fileName contains a directory, extract the directory name
    // the directory name will be used to locate the files included in _fileName.
    string dirName;
    string::size_type pos=_fileName.rfind('/');
#ifdef WNT
    if (pos == string::npos ) pos=_fileName.rfind('\\');
#endif
    if (pos != string::npos )
        dirName=string(_fileName, 0, pos+1);

    _intermediateMED medi; // structure de données intermédiaire pour conversion porflow->med
    string buf_ligne; // pour lire une ligne
    unsigned numberOfGridElement=0; // number of grid elements
    unsigned space_dimension=2;  // by default, the problem is assumed to be 2D
    string coordinateSystem="CARTESIAN";
    bool hybride=false;
    _groupe currentGroup;
    vector<bool> isGroupAList; // vector to store the type of a group (ie PAIR or LIST)
    bool processLoca=false;
    string connFileName;
    string coorFileName;
    std::vector<std::set<_maille>::iterator> p_ma_table(10000); // store iterators for direct access by index
    string small("abcdefghijklmnopqrstuvwxyz");

    while ( getline(_porflow, buf_ligne) ) // scan .inp file and look for main keywords (LOCA, GRID, COOR & CONN)
    {

        // Skip comments
        if(buf_ligne[0]=='/')
            continue; 
        
        // look if there is a filename
        string::size_type pos1=string::npos;
        string::size_type pos2=string::npos;
        locateFileName(buf_ligne, pos1, pos2);
        
        pos=0;
        if (pos1!=string::npos && pos2!=string::npos) // if there is a file name in buf_line
            // convert every char to upper, except those between pos1-pos2 (which correspond to a file name)
            while ( (pos=buf_ligne.find_first_of(small,pos)) != string::npos )
                if (pos<pos1 || pos>pos2 )
                    buf_ligne[pos]=toupper(buf_ligne[pos]);
                else
                    ++pos;
        else
            // convert every char to upper
            while ( (pos=buf_ligne.find_first_of(small,pos)) != string::npos )
                buf_ligne[pos]=toupper(buf_ligne[pos]);

        
        // LOCA KEYWORD :
        //   expected syntax : 
        //      - LOCA {PAIR} [ID=id] {N1,N2, ... ,Nn} : group specification by paired element & surface numbers
        //      - LOCA {LIST} [ID=id] {N1, ... ,Nn}    : group specification by a list of elements
        //   the syntax corresponding to the use of input file is not implemented
        if ( isKeyWord(buf_ligne,"LOCA") )
        {
            MESSAGE_MED("Mot clé LOCA détecté");
            processLoca=true;
            // if currentGroup is not empty, a group has been precessed 
            //  -> we store it, clear currentGroup, and start the precessing of a new group
            if ( !currentGroup.groupes.empty() )
                medi.groupes.push_back(currentGroup);
            currentGroup.groupes.clear();
            currentGroup.nom.erase();
            if ( buf_ligne.find("PAIR",0) != string::npos )
            {
                isGroupAList.push_back(false);
            }
            else if ( buf_ligne.find("LIST",0) != string::npos )
            {
                isGroupAList.push_back(true);
            }
            else
                throw MEDEXCEPTION("PORFLOW_MESH_RDONLY_DRIVER::read() : Error, after keyword LOCA, LIST or PAIR is expected\n");
                
            pos=buf_ligne.find("ID=",0);
            if ( pos != string::npos )
            {
                istringstream buf(buf_ligne.c_str()+pos+3);
                buf >> currentGroup.nom;

                // avoid problem in the following processing of elements if the group's name contains a digit
                // -> we replace group's name with blanks
                while( pos<buf_ligne.size() && !isspace( buf_ligne[pos] ) )
                    buf_ligne[pos++]=' ';
            }
        }


        // GRID KEYWORD :
        //   expected syntax : GRID {UNST} [THREE] {N1} [NODE] [N2]
        //   the syntax corresponding to structured grids is not implemented
        else if ( isKeyWord(buf_ligne,"GRID") )
        {
            MESSAGE_MED("Mot clé GRID détecté");
            processLoca=false;
            pos=buf_ligne.find("UNST",0);
            if ( pos != string::npos ) // unstructured grid
            {
                // we find the number of grid elements
                string::size_type len=buf_ligne.size();
                while ( pos<len && !isdigit(buf_ligne[pos]) )
                        ++pos;
                if ( pos==len )
                    throw MEDEXCEPTION("PORFLOW_MESH_RDONLY_DRIVER::read() : Error, number of grid elements unspecified!\n");
                istringstream buf(buf_ligne.c_str()+pos);
                buf >> numberOfGridElement;

                pos=buf_ligne.find("THREE",0);
                if ( pos != string::npos ) // if we find THREE, the problem is 3D
                    space_dimension=3;
            }
            else
            {
                throw MEDEXCEPTION("PORFLOW_MESH_RDONLY_DRIVER::read() : Error, structured grids are not yet supported!\n");
            }
        }
        

        // CONN KEYWORD :
        //   expected syntax : 
        //      - CONN {HYBR} {filename} : vertex connectivity for mixed hybrid elements
        //      - CONN {VERT} {filename} : vertex connectivity for quad or hex elements
        //   the default option is HYBR
        else if ( isKeyWord(buf_ligne,"CONN") )
        {
            MESSAGE_MED("Mot clé CONN détecté");
            processLoca=false;
            string fileCONN=getPorflowFileName(buf_ligne,"CONN");
            
            if ( buf_ligne.find("HYBR",0) != string::npos )
                hybride=true;


            if(fileCONN.rfind('/') == string::npos)
                // no directory was specified -> we add dirName, which may be empty

                connFileName=dirName+fileCONN;
            else
                connFileName=fileCONN;
        }

        
        // COOR KEYWORD :
        //   expected syntax : COOR {VERT} {filename}
        else if ( isKeyWord(buf_ligne,"COOR") )
        {
            MESSAGE_MED("Mot clé COOR");
            processLoca=false;
            string fileCOOR=getPorflowFileName(buf_ligne,"COOR");

            pos=buf_ligne.find("VERT",0);
            if ( pos == string::npos ) 
                throw MEDEXCEPTION("PORFLOW_MESH_RDONLY_DRIVER::read() : Error, after keyword COOR, we require VERT\n");

            pos=buf_ligne.find("CYLI",0);
            if ( pos != string::npos )
                coordinateSystem="CYLINDRICAL";

            if(fileCOOR.rfind('/') == string::npos)
                // no directory was specified -> we add dirName, which may be empty
                coorFileName=dirName+fileCOOR;
            else
                coorFileName=fileCOOR;
        }


        if (processLoca) 
        {
            // read group specification


            // replace non digit caracters with blanks
            string::iterator last_non_blank=buf_ligne.end();
            for (string::iterator it=buf_ligne.begin(); it!=buf_ligne.end(); ++it)
                if ( !isdigit(*it) )
                    *it=' ';
                else
                    last_non_blank=it;

            if (last_non_blank != buf_ligne.end() ) // if the line is not blank
            {
                buf_ligne=string(buf_ligne.begin(),++last_non_blank); // suppress trailing blanks

                istringstream buf(buf_ligne.c_str());
                int i1;
                buf >> i1; // we know the line is not blank
                do
                {
                    currentGroup.groupes.push_back(i1);
                    buf >> i1;
                }
                while ( buf );

            }
        }
        
    }
    if ( !currentGroup.groupes.empty() ) // if necessary, add the last group to medi.groupes
        medi.groupes.push_back(currentGroup);

    readPorflowCoordinateFile(coorFileName,medi,space_dimension);
    readPorflowConnectivityFile(hybride,connFileName,medi,p_ma_table,space_dimension);


    // Group processing :
    //   In the scan loop, we have store in medi.groupes.groupes either :
    //     - paired list of element and surface number (keyword PAIR)
    //     - an explicit list of element numbers (keyword LIST)
    //   What we stored (pairs or list) is remembered through vector isGroupAList.
    //   For both cases, we must convert these numbers to iterators to cells inside the set medi.maillage
    //     - in the LIST case, these cells already exist.
    //     - in the PAIR case, we have to create the corresponding 2D/1D faces
    // scan groups
    //for( std::vector<_groupe>::iterator i=medi.groupes.begin(); i!=medi.groupes.end(); ++i)
    for( unsigned i=0; i!=medi.groupes.size(); ++i)
    {
        if ( isGroupAList[i] ) 
        {
            // medi.groupes[i].groupes is a vector of element numbers; it points to it
            medi.groupes[i].mailles.resize( medi.groupes[i].groupes.size() );
            std::vector<int>::const_iterator it=medi.groupes[i].groupes.begin();
            for(int j = 0 ; it!=medi.groupes[i].groupes.end(); ++it, ++j)
            {
                // insert the iterator to the corresponding cell we stored in p_ma_table
                medi.groupes[i].mailles[j] = p_ma_table[*it];
            }
            
        }
        else
        {
            int nelem=0;
            int nface=0;
            int ngeom=0;
            int ielem=0;
            std::set<_maille>::iterator p_ma;
            _maille maille2D;
            
            // medi.groupes[i].groupes is a vector of paired element and surface numbers
            // *it points to the element number,  *(it+1) points to the surface number
            std::vector<int>::const_iterator it=medi.groupes[i].groupes.begin();
            medi.groupes[i].mailles.resize( medi.groupes[i].groupes.size() / 2 );
            for(  ; it!=medi.groupes[i].groupes.end(); ++it)
            {
                nelem=*it;
                nface=*(++it);
                p_ma=p_ma_table[nelem]; // points to the cell
                ngeom=geomMEDtoPorflow(p_ma->geometricType);

                // create the face corresponding to nface
                int l=0;
                while( l<nb_nodes2_max && connectivityPORFLOW[ngeom][nface-1][l] )
                {
                    maille2D.sommets.push_back(p_ma->sommets[ numPORFLOWtoMED[ngeom][ connectivityPORFLOW[ngeom][nface-1][l++]-1 ]-1 ]);
                }

                // the corresponding 2D MED geometric type depends upon the number of nodes
                maille2D.sommets.resize(l);
                maille2D.geometricType = get2DMedGeomType(l);
                p_ma = medi.insert(maille2D); // we insert the face in our mesh
                medi.groupes[i].mailles[ielem++]=p_ma; // and insert an iterator on it in our group
                maille2D.sommets.clear();
            }

        }
        medi.groupes[i].groupes.clear(); // we don't need element numbers anymore
        
    }

    p_ma_table.clear(); // we don't need it anymore
    MESSAGE_MED(LOC << "PORFLOW_MESH_RDONLY_DRIVER::read : RESULTATS STRUCTURE INTERMEDIAIRES : ");
    MESSAGE_MED(LOC <<  medi );
            // TRANSFORMATION EN STRUCTURES MED
    if ( ! _ptrMesh->isEmpty() )
    {
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Mesh object not empty : can't fill it!"));
    }
    else if ( /*medi.maillage.size()==0 || */medi.groupes.size()==0 || medi.points.size()==0)
    {
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << " Error while reading file " << _fileName 
                    << " The data read are not completed " ) ) ;
    }
    else
    {
        _ptrMesh->_name = _meshName;
        _ptrMesh->_spaceDimension = medi.points.begin()->second.coord.size();
        _ptrMesh->_numberOfNodes = medi.points.size();
        _ptrMesh->_coordinate = medi.getCoordinate();

        //Construction des groupes
        vector<GROUP *> groupCell, groupFace, groupEdge, groupNode;
        medi.getGroups(groupCell, groupFace, groupEdge, groupNode, _ptrMesh);
        _ptrMesh->_groupCell = groupCell;
        _ptrMesh->_groupFace = groupFace;
        _ptrMesh->_groupEdge = groupEdge;
        _ptrMesh->_groupNode = groupNode;

        // appele en dernier car cette fonction detruit le maillage intermediaire!
        _ptrMesh->_connectivity = medi.getConnectivity(); 
        MESSAGE_MED(LOC << "PORFLOW_MESH_RDONLY_DRIVER::read : FIN ");

        // calcul de la connectivite d-1 complete, avec renumerotation des groupes
        // if (_ptrMesh->_spaceDimension==3)
        //     _ptrMesh->_connectivity->updateGroup(_ptrMesh->_groupFace) ;
        // else if (_ptrMesh->_spaceDimension==2)
        //     _ptrMesh->_connectivity->updateGroup(_ptrMesh->_groupEdge) ;
        
        // Creation des familles ?artir des groupes
        // NC : Cet appel pourra ?e diff鲩 quand la gestion de la coh鲥nce famille/groupes sera assur饊   _ptrMesh->createFamilies();
    }


  END_OF_MED(LOC);
}

void PORFLOW_MESH_RDONLY_DRIVER::write( void ) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("PORFLOW_MESH_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}


/*--------------------- WRONLY PART -------------------------------*/

PORFLOW_MESH_WRONLY_DRIVER::PORFLOW_MESH_WRONLY_DRIVER():PORFLOW_MESH_DRIVER()
{
}
  
PORFLOW_MESH_WRONLY_DRIVER::PORFLOW_MESH_WRONLY_DRIVER(const string & fileName,
                                                       GMESH *        ptrMesh):
  PORFLOW_MESH_DRIVER(fileName,ptrMesh,WRONLY)
{
  MESSAGE_MED("PORFLOW_MESH_WRONLY_DRIVER::PORFLOW_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

PORFLOW_MESH_WRONLY_DRIVER::PORFLOW_MESH_WRONLY_DRIVER(const PORFLOW_MESH_WRONLY_DRIVER & driver): 
  PORFLOW_MESH_DRIVER(driver)
{
}

PORFLOW_MESH_WRONLY_DRIVER::~PORFLOW_MESH_WRONLY_DRIVER()
{
  //MESSAGE_MED("PORFLOW_MESH_WRONLY_DRIVER::PORFLOW_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
}

GENDRIVER * PORFLOW_MESH_WRONLY_DRIVER::copy(void) const
{
  return new PORFLOW_MESH_WRONLY_DRIVER(*this);
}

void PORFLOW_MESH_WRONLY_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("PORFLOW_MESH_WRONLY_DRIVER::read : Can't read with a WRONLY driver !");
}

void PORFLOW_MESH_WRONLY_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{ 
  const char * LOC = "void PORFLOW_MESH_WRONLY_DRIVER::write(void) const : ";
  BEGIN_OF_MED(LOC);

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<< "Write Driver isn\'t implemented"));

  END_OF_MED(LOC);
} 



/*--------------------- RDWR PART -------------------------------*/

PORFLOW_MESH_RDWR_DRIVER::PORFLOW_MESH_RDWR_DRIVER():
  PORFLOW_MESH_DRIVER(),
  PORFLOW_MESH_RDONLY_DRIVER(),
  PORFLOW_MESH_WRONLY_DRIVER()
{
}

PORFLOW_MESH_RDWR_DRIVER::PORFLOW_MESH_RDWR_DRIVER(const string & fileName,MESH * ptrMesh):
  PORFLOW_MESH_DRIVER(fileName,ptrMesh,RDWR),
  PORFLOW_MESH_RDONLY_DRIVER(fileName,ptrMesh),
  PORFLOW_MESH_WRONLY_DRIVER(fileName,ptrMesh)
{
  MESSAGE_MED("PORFLOW_MESH_RDWR_DRIVER::PORFLOW_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

PORFLOW_MESH_RDWR_DRIVER::PORFLOW_MESH_RDWR_DRIVER(const PORFLOW_MESH_RDWR_DRIVER & driver): 
  PORFLOW_MESH_DRIVER(driver),
  PORFLOW_MESH_RDONLY_DRIVER(driver),
  PORFLOW_MESH_WRONLY_DRIVER(driver)
{
}

PORFLOW_MESH_RDWR_DRIVER::~PORFLOW_MESH_RDWR_DRIVER() {
  //MESSAGE_MED("PORFLOW_MESH_RDWR_DRIVER::PORFLOW_MESH_RDWR_DRIVER(const string & fileName, MESH * ptrMesh) has been destroyed");
} 
  
GENDRIVER * PORFLOW_MESH_RDWR_DRIVER::copy(void) const
{
  return new PORFLOW_MESH_RDWR_DRIVER(*this);
}

void PORFLOW_MESH_RDWR_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{
  PORFLOW_MESH_WRONLY_DRIVER::write();
}
void PORFLOW_MESH_RDWR_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  PORFLOW_MESH_RDONLY_DRIVER::read();
}
