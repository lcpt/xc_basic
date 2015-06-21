// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File      : MEDMEM_EnsightUtils.cxx
// Created   : Fri Jun  6 16:08:32 2008
// Author    : Edward AGAPOV (eap)
//
#include "MEDMEM_Field.hxx" // important to include it before MEDMEM_EnsightUtils.hxx,
// in order not to redefine DBL_MIN

#include "MEDMEM_EnsightUtils.hxx"

#include "MEDMEM_EnsightMeshDriver.hxx"
#include "MEDMEM_EnsightFieldDriver.hxx"
#include "MEDMEM_DriverTools.hxx"

#ifdef WNT
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
//#include <stdio.h>

using namespace std;
using namespace MEDMEM;

  // TODO:
// -2) in transient model, get time values for meshes form fields on them
// -1) why after reading /data/eap/ENSIGHT/SAMPLES_SRC/EnSight/crash.case only
//     geo EnSIght files are written
// -0) test selecting imaginary part to read
// 0) appending fields to write with same names in Case file
// 1) write grid
// 2) if mesh description is longer that maxLen, distribute it among two lines
// 3) care of mesh description at reading
// 4) at writing, cut off " t=..." from mesh name and set to meshDriver->_meshName
// 5) MEDMEM_EXPORT
// 4.5) do something with zzzz121b.med pb
// 6) stripe in getLine()
// 7) care of elem numbers that were read
// 8) read real and imaginary of complex field by MED driver
// 9) read "mesured" and care of variable on it
// 10) clear theInterMedMap and delete _InterMed's there
// 11) not to fill MESH made in getMeshData()
// 12) care of changing nb of '*' in names
// 14) analize _TimeSet to write in shortest form
// 15) add LOCALIZED where no method name specified
// 17) add checks of input mesh and field like in gibi
// 18) check writing interlace in all modes
// 19) define creation by MEDMEM by case file and not by geo
// 21) there maybe '\n' in binary file after string end
// 22) optimize _ASCIIFileReader::eof() at file end
// 23) What if to create CELL entity with 1-3 dim and a CELL group?
// 24) not to write CELL connectivity if a group on all cells is present
// 25) compact imed (sortedNodeIDs etc.)
// 26) no need in converting EnSight variables to eventual types in convertReals
//     since a result array is not used directly but occupies more memory
// 27) try to exclude merged nodes in DTree and compare time on ChampsDarcy.med


namespace MEDMEM
{
  // ==============================================================================
  /*!
    Writing format.
   */
  // ==============================================================================

  EnSightFormat theEnSightFormatForWriting = ENSIGHT_6;
  bool          theBinaryFormatForWriting  = false;
  bool          theIgnoreIncompatibility   = false;

  void          setEnSightFormatForWriting (EnSightFormat format, bool isBinary)
  {
    theEnSightFormatForWriting = format;
    theBinaryFormatForWriting  = isBinary;
  }
  EnSightFormat getEnSightFormatForWriting()
  {
    return theEnSightFormatForWriting;
  }
  bool          isBinaryEnSightFormatForWriting()
  {
    return theBinaryFormatForWriting;
  }
  // ==============================================================================
  /*!
    To raise or not if MEDMEM-EnSight incompatibility encounters or suspected
  */
  // ==============================================================================

  void          setIgnoreIncompatibility(bool toIgnore)
  {
    theIgnoreIncompatibility = toIgnore;
  }
}

#ifdef WNT
#define FILE_SEPARATOR '\\'
#else
#define FILE_SEPARATOR '/'
#endif

#define _ATOI( str ) atoi((str).c_str())
#define _ATOF( str ) atof((str).c_str())

#define BUFFER_SIZE 16184  // for non-stream input

namespace MEDMEM_ENSIGHT
{
  //--------------------------------------------------------------------------------
  /*!
   * \brief Registry of drivers creating _CaseFileDriver
   *
   * The problem is that if the mesh/field driver is created by the user, it is
   * to write Case file, but if it is created by MED driver, it should not, in
   * order not to overwrite the Case file written by MED driver.
   * To assure this feature, we register all mesh/field drivers added to a case 
   * file by a MED driver and then ignore thier calls to _CaseFileDriver.
   */
  set< const _CaseFileDriver_User* > theCaseUsers;

  //--------------------------------------------------------------------------------
  /*!
   * \brief Storage of data read by mesh driver to be used by field drivers
   *
   * The field driver needs info read by the mesh driver (nb of elements by type in
   * each part and what place the element gets in MEDMED support).
   * So we store needed data unless it is no more needed (i.e. no driver is alive).
   * The map key is "<case_file>:<mesh index>".
   */
  map< string, _InterMed* > theInterMedMap;

  //--------------------------------------------------------------------------------
  /*!
   * \brief Add a driver to the registry and return true if was already in
   */
  bool isToIgnore(const _CaseFileDriver_User* driver);
  bool isToIgnore(const _CaseFileDriver_User* driver)
  {
    return ! theCaseUsers.insert( driver ).second;
  }

  //--------------------------------------------------------------------------------
  /*!
   * \brief Remove a driver from the registry
   */
  void unregister(const _CaseFileDriver_User* driver);
  void unregister(const _CaseFileDriver_User* driver)
  {
    theCaseUsers.erase( driver );
    if ( theCaseUsers.empty() )
    {
      for ( map< string, _InterMed* >::iterator str_imed = theInterMedMap.begin();
            str_imed != theInterMedMap.end();
            str_imed++ )
        delete str_imed->second;
      theInterMedMap.clear();
    }
  }

  //--------------------------------------------------------------------------------
  /*!
   * \brief Return mesh data needed by field driver
   */
  _InterMed* getMeshData( const string& key );
  _InterMed* getMeshData( const string& key )
  {
    // find existing data
    map< string, _InterMed* >::iterator kimed = theInterMedMap.find( key );
    if ( kimed != theInterMedMap.end() )
      return kimed->second;

    // create a new data
    MESH * mesh = new MESH;
    string caseFile, meshIndex;
    _ASCIIFileReader::split( key, caseFile, meshIndex, ':' );
    ENSIGHT_MESH_RDONLY_DRIVER meshDrv(caseFile, mesh, _ATOI( meshIndex ));
    meshDrv.read();
    kimed = theInterMedMap.find( key );
    if ( kimed == theInterMedMap.end() )
      return 0;
    _InterMed* imed     = kimed->second;
    imed->_medMesh      = mesh;
    imed->_isOwnMedMesh = true;
    return imed;
  }
  // ---------------------------------------------------------------
  /*!
   * \brief Prepend "EnSight-MEDMEM compatibility problem" to the text
   *        of an exception
   */
  STRING compatibilityPb(const string& exceptionText)
  {
    return STRING("EnSight-MEDMEM compatibility problem:\n") << exceptionText;
  }
  // ---------------------------------------------------------------
  /*!
   * \brief To ignore incompatibility or not
   */
  bool   toIgnoreIncompatibility()
  {
    return theIgnoreIncompatibility;
  }

  //--------------------------------------------------------------------------------
  /*!
   * \brief Return true if set index is empty or corresponds to an existing set
   */
  template <class TSet> bool isValidIndex(const string& index, const map<int,TSet>& aMap)
  {
    if ( index.empty() ) return true;
    return ( aMap.find( _ATOI( index ) ) != aMap.end() );
  }

  //--------------------------------------------------------------------------------
  /*!
   * \brief Return EnSight type corresponding to med one
   */
  const TEnSightElemType& getEnSightType(medGeometryElement medType)
  {
    static TEnSightElemType theEnSightType;

    int nbNodes = medType % 100;
    theEnSightType._medType = medType;

    switch ( medType ) {
    case MED_EN::MED_NONE    :
    case MED_EN::MED_POINT1  : {
      theEnSightType._name = "point";
      theEnSightType._medIndex.resize(1,0);
      break ;
    }
    case MED_EN::MED_SEG2    : {
      theEnSightType._name = "bar2";
      int conn [2] = {0,1};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_SEG3    : {  
      theEnSightType._name = "bar3";
      int conn [3] = {0,2,1};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_TRIA3   : {
      theEnSightType._name = "tria3";
      int conn [3] = {0,2,1};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_QUAD4   : {
      theEnSightType._name = "quad4";
      int conn [4] = {0,3,2,1};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_TRIA6   : {
      theEnSightType._name = "tria6";
      int conn [6] = {0,2,1,5,4,3};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_QUAD8   : {
      theEnSightType._name = "quad8";
      int conn [8] = {0,3,2,1,7,6,5,4};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_TETRA4  : {
      theEnSightType._name = "tetra4";
      int conn [4] = {0,1,3,2};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_PYRA5   : {
      theEnSightType._name = "pyramid5";
      int conn [5] = {0,3,2,1,4};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_PENTA6  : {
      theEnSightType._name = "penta6";
      int conn [6] = {0,2,1,3,5,4};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_HEXA8   : {
      theEnSightType._name = "hexa8";
      int conn [8] = {0,3,2,1,4,7,6,5};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_TETRA10 : {
      theEnSightType._name = "tetra10";
      int conn [10] = {0,2,1,3,6,5,4,7,9,8};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_PYRA13  : {
      theEnSightType._name = "pyramid13";
      int conn [13] = {0,3,2,1,4,8,7,6,5,9,12,11,10};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_PENTA15 : {
      theEnSightType._name = "penta15";
      int conn [15] = {0,2,1,3,5,4,8,7,6,11,10,12,14,13};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_HEXA20  : {
      theEnSightType._name = "hexa20";
      int conn [20] = {0,3,2,1,4,7,6,5,11,10,9,8,15,14,13,12,16,19,18,17};
      theEnSightType._medIndex.assign( conn, conn+nbNodes );
      break ;
    }
    case MED_EN::MED_POLYGON : {
      theEnSightType._name = "nsided";
      theEnSightType._medIndex.clear();
      break ;
    }
    case MED_EN::MED_POLYHEDRA : {
      theEnSightType._name = "nfaced";
      theEnSightType._medIndex.clear();
      break ;
    }
    default: {
      theEnSightType._name = "";
      theEnSightType._medIndex.clear();
    }
    }
    return theEnSightType;
  }

  //--------------------------------------------------------------------------------
  /*!
   * \brief Return EnSight type having a given name
   */
  const TEnSightElemType& getEnSightType(const string& theTypeName)
  {
    string typeName = theTypeName;
    if ( isGhostType( typeName ))
      typeName = string( &typeName[2] ); // ghost type

    static map<string, TEnSightElemType> name2Type;

    map<string, TEnSightElemType>::iterator nameType = name2Type.find( typeName );
    if ( nameType != name2Type.end() )
      return nameType->second;

    const list<medGeometryElement> & allMedTypes = MED_EN::meshEntities[MED_EN::MED_CELL];
    list< medGeometryElement >::const_iterator medType = allMedTypes.begin();
    for ( ; medType != allMedTypes.end(); ++medType )
    {
      const TEnSightElemType& enSightType = getEnSightType( *medType );
      if ( enSightType._name == typeName )
        return name2Type[ typeName ] = enSightType;
    }
    return getEnSightType(MED_ALL_ELEMENTS);
  }

// =============================================================== _CaseFileDriver
/*!
 * \brief Case file driver constructor
 */
//================================================================================

_CaseFileDriver::_CaseFileDriver(const string &              fileName,
                                 const _CaseFileDriver_User* creator)
  : _fileName( fileName ), _directory("."), _user( creator )
{
  // Find out if the driver is blocked
  _blocked = isToIgnore( creator );
  if ( creator->getAccessMode() == MED_EN::RDONLY )
    _blocked = false;
  if ( !_blocked ) {
    // set directory
    string::size_type sepPos = _fileName.rfind( FILE_SEPARATOR );
    if ( sepPos != _fileName.npos ) {
      _directory = _fileName.substr( 0, sepPos );
    }
    _format = getEnSightFormatForWriting();
  }
}

// =============================================================== _CaseFileDriver
/*!
 * \brief Case file driver destructor
 */
//================================================================================

_CaseFileDriver::~_CaseFileDriver()
{
  if ( !_blocked )
  {
    // to write case file again by same DRIVER
    unregister( _user );
    ((_CaseFileDriver_User*)_user)->_imed = 0;
  }
}

#define READ_NEXT_LINE continue
#define RAISE_EXCEPTION break

//================================================================================
/*!
 * \brief Read Case file
 */
//================================================================================

void _CaseFileDriver::read() throw (MEDEXCEPTION)
{
  if ( _blocked )
    return;

  _ASCIIFileReader reader( _fileName );

  STRING badFile("Invalid Case file ");
  badFile << _fileName << "\n";

  list<_FileSet> fileSets;
  list<_TimeSet> timeSets;

  set<string> varNames; // to detect equal variable names

  string section = "_";
  int lineNb = 0;
  while ( !reader.eof() )
  {
    string line = reader.getLine();
    ++lineNb;

    // cut off comments
    string comment;
    reader.split( line, line, comment, '#');
    if ( line.empty() )
      READ_NEXT_LINE;

    string key, value; // parts of a line splited by column
    reader.split( line, key, value, ':');

    STRING errorMsg;

    // analyse not empty lines
    switch ( section[0] ) {
    case 'V': {
      // --------------------------------------------------------------------------------
      // VARIABLE

      string type, s, ts, fs;
      reader.split(key,type,s);
      int newVarIndex = _variables.empty() ? 1 : _variables.rbegin()->first + 1;

      if ( type == "scalar" || type == "vector" || type == "tensor" )
      {
        // scalar per node:        [ts] [fs] description filename
        // vector per node:        [ts] [fs] description filename
        // tensor symm per node:       [ts]  [fs]   description filename
        // tensor asym per node:       [ts]  [fs]   description filename
        // scalar per element:         [ts]  [fs]   description filename
        // vector per element:         [ts]  [fs]   description filename
        // tensor symm per element:    [ts]  [fs]   description filename
        // tensor asym per element:    [ts]  [fs]   description filename
        // scalar per measured node:   [ts]  [fs]   description filename
        // vector per measured node:   [ts]  [fs]   description filename
        list<string> parts;
        int nbParts = reader.split( value, parts );
        if ( nbParts < 2 ) {
          errorMsg << "invalid variable format:\n" << line;
          RAISE_EXCEPTION;
        }
        if ( contains( "per measured node", s.c_str() )) {
          //std::cerr << "Skip not supported data type: " << key << endl;
          READ_NEXT_LINE;
        }
        list<string>::reverse_iterator p = parts.rbegin();
        _Variable& var      = _variables[newVarIndex];
        var._type           = key;
        var._fileNameOrData = *p++;
        var._name           = *p++;
        if ( nbParts == 3 ) {
          var._timeSetNumber = *p;
        }
        else if ( nbParts == 4 ) {
          var._fileSetNumber = *p++;
          var._timeSetNumber = *p;
        }
        varNames.insert( var._name );
        READ_NEXT_LINE;
      }
      else if ( type == "constant" )
      {
        // constant per case:      [ts]      description const_value(s)
        // constant per case file: [ts]      description cvfilename
        reader.split(value,s,value);
        if ( reader.isDigit( s )) {
          ts = s;
          reader.split(value,s,value);
        }
        _Variable& var      = _variables[newVarIndex];
        var._type           = key;
        var._name           = s;
        var._fileNameOrData = value;
        var._timeSetNumber  = ts;
        if ( var._name.empty() || var._fileNameOrData.empty() ) {
          errorMsg << "invalid variable format:\n" << line;
          RAISE_EXCEPTION;
        }
        varNames.insert( var._name );
        READ_NEXT_LINE;
      }
      else if ( type == "complex" )
      {
        // complex scalar per node:    [ts]  [fs]   description Re_fn   Im_fn freq
        // complex vector per node:    [ts]  [fs]   description Re_fn   Im_fn freq
        // complex scalar per element:    [ts] [fs] description Re_fn   Im_fn freq
        // complex vector per element:    [ts] [fs] description Re_fn   Im_fn freq
        reader.split(value,s,value);
        if ( reader.isDigit( s )) {
          ts = s;
          reader.split(value,s,value);
          if ( reader.isDigit( s )) {
            fs = s;
            reader.split(value,s,value);
          }
        }
        list<string> parts;
        int nbParts = reader.split( value, parts );
        if ( nbParts < 3 ) {
          errorMsg << "invalid variable format:\n" << line;
          RAISE_EXCEPTION;
        }
        // a variable contains two fields. We leave one slot in _variables empty
        // in order to have last key in _variables equal to number of fields.
        // Variable index equal to missing slot key corresponds to real part (Re_fn)
        _Variable& var     = _variables[++newVarIndex];
        var._type          = key;
        var._name          = s;
        var._timeSetNumber = ts;
        var._fileSetNumber = fs;
        var._fileNameOrData = value;
        varNames.insert( var._name );
        READ_NEXT_LINE;
      }
      break;
    }
    case 'T': {
      // --------------------------------------------------------------------------------
      // TIME
      //    time set:              ts [description]
      //    number of steps:       ns
      //    filename start number: fs
      //    filename increment:    fi
      //    time values:           time_1 time_2 .... time_ns
      // or
      //    time set:              ts [description]
      //    number of steps:       ns
      //    filename numbers:      fn
      //    time values:           time_1 time_2 .... time_ns
      // or
      //    time set:              ts [description]
      //    number of steps:       ns
      //    filename numbers file: fnfilename
      //    time values file:      tvfilename
      _TimeSet* timeSet = timeSets.empty() ? 0 : & timeSets.back();
      // ---------------------------------------------------------
      if ( key == "time set" )
      {
        int nb = _ATOI( value );
        if ( nb < 1 ) {
          errorMsg << "Invalid time set number: " << value;
          RAISE_EXCEPTION;
        }
        timeSets.push_back( _TimeSet() );
        timeSets.back()._number = nb;
        READ_NEXT_LINE;
      }
      // ---------------------------------------------------------
      if ( key == "number of steps" )
      {
        if ( !timeSet || !timeSet->_times.empty() ) {
          errorMsg << "Unexpected command: " << key;
          RAISE_EXCEPTION;
        }
        int nbSteps = _ATOI( value );
        if ( nbSteps < 1 ) {
          errorMsg << "invalid number of steps: " << value;
          RAISE_EXCEPTION;
        }
        timeSet->_times.resize( nbSteps );
        timeSet->_fileIndex.resize( nbSteps );
        READ_NEXT_LINE;
      }
      // ---------------------------------------------------------
      if ( key == "filename start number" )
      {
        if ( !timeSet || timeSet->_fileIndex.empty() ) {
          errorMsg << "Unexpected command: " << key;
          RAISE_EXCEPTION;
        }
        if ( !reader.isDigit( value )) {
          errorMsg << "invalid " << line;
          RAISE_EXCEPTION;
        }
        timeSet->_fileIndex[0] = value;
        READ_NEXT_LINE;
      }
      // ---------------------------------------------------------
      if ( key == "filename increment" ) {
        int incr = _ATOI( value );
        if ( incr == 0 ) {
          errorMsg << "invalid " << line;
          RAISE_EXCEPTION;
        }
        if ( !timeSet ||
             timeSet->_fileIndex.empty() ||
             timeSet->_fileIndex[0].empty() ) {
          errorMsg << "Unexpected command: " << key;
          RAISE_EXCEPTION;
        }
        int index = incr + _ATOI( timeSet->_fileIndex[0] );
        int nbSteps = timeSet->_fileIndex.size();
        for ( int i = 1; i < nbSteps; ++i, index += incr )
          timeSet->_fileIndex[i] = STRING( index );
        READ_NEXT_LINE;
      }
      // ---------------------------------------------------------
      if ( key == "time values" )
      {
        if ( !timeSet || timeSet->_times.empty() ) {
          errorMsg << "Unexpected command: " << key;
          RAISE_EXCEPTION;
        }
        list<string> times;
        int i, nbTimes = reader.split( value, times );
        list<string>::iterator t = times.begin();
        for ( i = 0; i < nbTimes; ++i, ++t )
          timeSet->_times[i] = *t;
        while ( nbTimes != (int)timeSet->_times.size() ) {
          value = reader.getLine();
          ++lineNb;
          nbTimes += reader.split( value, times );
          for (t = times.begin(); i < nbTimes; ++i, ++t ) {
            if ( ! reader.isDigit( *t, /*real=*/true ))
              break;
            timeSet->_times[i] = *t;
          }
        }
        if ( nbTimes != (int)timeSet->_times.size() ) {
          errorMsg << "incorrect number of times in time set " << timeSet->_number;
          RAISE_EXCEPTION;
        }
        for ( i = 1; i < nbTimes; ++i )
          if ( _ATOF( timeSet->_times[ i ]) <= _ATOF(timeSet->_times[ i-1 ] ))
            break;
        if ( i < nbTimes ) { // break from the previous loop occured
          errorMsg << "time values are not in ascending order in time set " << timeSet->_number;
          RAISE_EXCEPTION;
        }
        READ_NEXT_LINE;
      }
      // ---------------------------------------------------------
      if ( key == "filename numbers" )
      {
        if ( !timeSet || timeSet->_fileIndex.empty()) {
          errorMsg << "Unexpected command: " << key;
          RAISE_EXCEPTION;
        }
        list<string> numbers;
        int i, nb = reader.split( value, numbers );
        int nbFiles = timeSet->_fileIndex.size();
        timeSet->_fileIndex.insert(timeSet->_fileIndex.begin(), numbers.begin(), numbers.end() );
        while ( nb != nbFiles ) {
          value = reader.getLine();
          ++lineNb;
          i = nb;
          nb += reader.split( value, numbers );
          list<string>::iterator n = numbers.begin();
          for ( ; i < nb; ++i, ++n ) {
            if ( ! reader.isDigit( *n ))
              break;
            timeSet->_fileIndex[i] = *n;
          }
        }
        if ( nb != nbFiles ) {
          errorMsg << "incorrect number of " << key << " in time set " << timeSet->_number;
          RAISE_EXCEPTION;
        }
        READ_NEXT_LINE;
      }
      // ---------------------------------------------------------
      if ( key == "filename numbers file" ||
           key == "time values file" )
      {
        if ( !timeSet || timeSet->_fileIndex.empty()) {
          errorMsg << "Unexpected command: " << key;
          RAISE_EXCEPTION;
        }
        string fileName = _directory + FILE_SEPARATOR + value;
        if ( !_user->canOpenFile( fileName, RDONLY )) {
          errorMsg << "Can not open file " << fileName;
          RAISE_EXCEPTION;
        }
        _ASCIIFileReader file( fileName );
        list<string> numbers;
        while ( !file.eof() )
          numbers.push_back( file.getWord() );
        int nb = numbers.size();
        if ( nb != (int)timeSet->_times.size() ) {
          errorMsg << "incorrect number of values in file " << value;
          RAISE_EXCEPTION;
        }
        if ( key[0] == 'f' )
          timeSet->_fileIndex.assign( numbers.begin(), numbers.end() );
        else
          timeSet->_times.assign( numbers.begin(), numbers.end() );
        READ_NEXT_LINE;
      }
      break;
    }
    case 'F': {
      if ( section[1] == 'I' ) {
        // --------------------------------------------------------------------------------
        // FILE
        // file set:               fs
        // filename index:         fi # Note: only used when data continues in other files
        // number of steps:        ns
        if ( key == "file set" ) {
          int nb = _ATOI( value );
          if ( nb < 1 ) {
            errorMsg << "Invalid file set number: " << value;
          }
          else {
            fileSets.push_back( _FileSet() );
            fileSets.back()._number = nb;
            READ_NEXT_LINE;
          }
        }
        else if ( key == "filename index" ) {
          if ( fileSets.empty() ) {
            errorMsg << "'filename index' before 'file set'";
          }
          else if ( !value.empty() ) {
            fileSets.back()._fileIndex.push_back( value );
            READ_NEXT_LINE;
          }
        }
        else if ( key == "number of steps" ) {
          if ( fileSets.empty() ) {
            errorMsg << "'number of steps' before 'file set'";
          }
          else if ( value.empty() ) {
            errorMsg << "number of steps omitted: " << line;
          }
          else if ( !reader.isDigit( value )) {
            errorMsg << "invalid number of steps: " << value;
          }
          else {
            int n = _ATOI( value );
            if ( n < 1 ) {
              errorMsg << "invalid number of steps: " << value;
              RAISE_EXCEPTION;
            }
            fileSets.back()._nbStepsInFile.push_back( n );
            READ_NEXT_LINE;
          }
        }
      }
      else {
        // --------------------------------------------------------------------------------
        // FORMAT
        // type:  ensight
        if ( key != "type" ) {
          errorMsg << "unexpected info in section " << section << ":\n" << line;
          RAISE_EXCEPTION;
        }
        else {
          if ( value == "ensight gold" ) {
            _format = ENSIGHT_GOLD;
          }
          else if ( value == "ensight" ) {
            _format = ENSIGHT_6;
          }
          else {
            errorMsg << "Unsupported EnSight format: " << value;
            RAISE_EXCEPTION;
          }
          section = "_";
          READ_NEXT_LINE;
        }
      }
      break;
    }
    case 'G': {
      // --------------------------------------------------------------------------------
      // GEOMETRY
      // model:    [ts] [fs] filename [change_coords_only]
      // measured: [ts] [fs] filename [change_coords_only]
      // match:              filename
      // boundary:           filename
      // rigid_body:         filename
      if ( key == "measured" || key == "match" || key == "boundary" || key == "rigid_body") {
        //errorMsg << key << " geometry not supported";
	std::cerr << "Warning: " << key << " geomerty not supported" << endl;
        READ_NEXT_LINE;
      }
      else if ( key == "model" ) {
        list<string> parts;
        reader.split( value, parts );
        list<string>::reverse_iterator s = parts.rbegin();
        for ( ; s != parts.rend(); ++s )
        {
          if ( *s == "change_coords_only" )
            _model._change_coords_only = *s;
          else if ( _model._fileName.empty() )
            _model._fileName = *s;
          else if ( _model._fileSetNumber.empty() )
            _model._fileSetNumber = *s;
          else
            _model._timeSetNumber = *s;
        }
        if ( _model._timeSetNumber.empty() && !_model._fileSetNumber.empty() )
          swap( _model._timeSetNumber, _model._fileSetNumber );
        if ( _model._fileName.empty() ) {
          errorMsg << "invalid model: " << value;
          RAISE_EXCEPTION;
        }
        READ_NEXT_LINE;
      }
      break;
    }
    case 'M': { // MATERIAL
      string keyWord1;
      reader.split(key,keyWord1,key);
      if ( keyWord1 == "material" || keyWord1 == "species" )
        READ_NEXT_LINE;
      break;
    }
    } // end switch (section[0])

    if ( !errorMsg.empty() ) {
      throw MEDEXCEPTION(STRING("Invalid Case file ") << _fileName
                         << ":" << lineNb << "\n" << errorMsg );
    }

    // we are here if a line was not recognized to belong to a current section
    if ( line == "FORMAT"   ||
         line == "GEOMETRY" ||
         line == "VARIABLE" ||
         line == "TIME"     ||
         line == "FILE"     ||
         line == "MATERIAL")
      section = line;
    else
      throw MEDEXCEPTION(STRING() << "Invalid format of Case file " << _fileName
                         << "\nwrong line: " << line );
  }

  if ( _model._fileName.empty() )
    throw MEDEXCEPTION(badFile << "no supported geometry information found" );

  // store time sets and file sets
  list<_FileSet>::iterator fs = fileSets.begin();
  for ( ; fs != fileSets.end(); ++fs ) {
    if ( fs->_nbStepsInFile.size() > 1 && fs->_fileIndex.empty() )
      throw MEDEXCEPTION(badFile << "missing file indices in a file set " << fs->_number );
    _fileSets[ fs->_number ] = *fs;
  }
  list<_TimeSet>::iterator ts = timeSets.begin();
  for ( ; ts != timeSets.end(); ++ts )
    _timeSets[ ts->_number ] = *ts;

  // check validity of ts and fs

  if ( !isValidIndex( _model._timeSetNumber, _timeSets ))
    throw MEDEXCEPTION(badFile << "bad time set index:" << _model._timeSetNumber );
  if ( !isValidIndex( _model._fileSetNumber, _fileSets ))
    throw MEDEXCEPTION(badFile << "bad file set index:" << _model._timeSetNumber );

  map< int, _Variable>::iterator ivars = _variables.begin();
  for ( ; ivars != _variables.end(); ++ivars ) {
    if ( !isValidIndex( ivars->second._timeSetNumber, _timeSets ))
      throw MEDEXCEPTION(badFile << "bad time set index:" << ivars->second._timeSetNumber );
    if ( !isValidIndex( ivars->second._fileSetNumber, _fileSets ))
      throw MEDEXCEPTION(badFile << "bad file set index:" << ivars->second._fileSetNumber );
  }

  // check uniqueness of variable names
  if ( varNames.size() != _variables.size() )
    std::cerr << badFile <<
      "Warning: there are different fields with equal names, you may have problems!" << endl;
//     throw MEDEXCEPTION(badFile );

  // As variable does not refer to time set if there is one step (issue 0020113),
  // we try to restore the reference

  for ( ivars = _variables.begin(); ivars != _variables.end(); ++ivars ) {
    _Variable & var = ivars->second;
    if ( var._timeSetNumber.empty() )
    {
      // try to find time set with id equal to variable order number
      map< int, _TimeSet >::iterator iTs = _timeSets.find( ivars->first );
      if ( iTs != _timeSets.end() && iTs->second._times.size() == 1 )
        var._timeSetNumber = STRING( iTs->second._number );
      else {
        // find any time set with 1 time value
        for ( iTs = _timeSets.begin(); iTs != _timeSets.end(); ++iTs )
          if ( iTs->second._times.size() == 1 )
            var._timeSetNumber = STRING( iTs->second._number ); 
      }
    }
  }
}

//================================================================================
/*!
 * \brief return number of time steps of a model
 */
//================================================================================

int _CaseFileDriver::getNbMeshes() const
{
  if ( _blocked || checkWasRead())
    return 0 ;
  if ( _model._timeSetNumber.empty() )
    return 1;
  int ts = _ATOI( _model._timeSetNumber );
  map< int, _TimeSet >::const_iterator its = _timeSets.find( ts );
  if ( its == _timeSets.end() )
    throw MEDEXCEPTION(STRING() << "Invalid format of Case file " << _fileName
                       << "\n Inexistent time set number of a model" );
  return its->second._times.size();
}

//================================================================================
/*!
 * \brief  Sets all data necessary for meshDriver::read()
 *  \param meshIndex - time step index
 *  \param meshDriver - driver
 */
//================================================================================

void _CaseFileDriver::setDataFileName(const int                   meshIndex,
                                      ENSIGHT_MESH_RDONLY_DRIVER* meshDriver)
{
  if ( _blocked || checkWasRead())
    return;
  isToIgnore( meshDriver ); // remeber

  meshDriver->_dataFileName    = _directory + FILE_SEPARATOR + _model._fileName;
  meshDriver->_indexInDataFile = fixWildCardName( meshIndex,
                                                  _model._timeSetNumber,
                                                  _model._fileSetNumber,
                                                  meshDriver->_dataFileName,
                                                  meshDriver->_time);
  meshDriver->_isGoldFormat    = ( _format == ENSIGHT_GOLD );
  meshDriver->_transientMode   = ( !_model._timeSetNumber.empty() );
  meshDriver->_singleFileMode  = ( !_fileSets.empty() );
  meshDriver->_imedMapKey      = STRING(_fileName)<<":"<<meshIndex;

  GMESH* ptrMesh = meshDriver->getMesh();
  ptrMesh->setName(STRING("EnSight mesh ") << meshIndex);
}

//================================================================================
/*!
 * \brief Return nb of ensight variables
 */
//================================================================================

int  _CaseFileDriver::getNbVariables() const
{
  return _variables.empty() ? 0 : _variables.rbegin()->first;
}

//================================================================================
/*!
 * \brief return number of time steps of a variable
 */
//================================================================================

int _CaseFileDriver::getNbVarSteps(const int variableIndex)
{
  if ( _blocked || checkWasRead() )
    return 0;

  map< int, _Variable>::const_iterator ivar = _variables.find( variableIndex );
  if ( ivar == _variables.end() ) {
    // it can be index of real part of complex variable
    ivar = _variables.find( variableIndex+1 );
    if ( ivar == _variables.end() || !contains( "complex", ivar->second._type.c_str() )) {
      throw MEDEXCEPTION(STRING( "_CaseFileDriver::getNbVarSteps(): invalid variable index: ")
                         << variableIndex);
    }
  }
  const _Variable & var = ivar->second;
  if ( var._timeSetNumber.empty() )
    return 1;

  const _TimeSet & ts = _timeSets[ _ATOI( var._timeSetNumber)];
  return ts._times.size();
}

//================================================================================
/*!
 * \brief return variable index by variable name, return 0 if none found
 */
//================================================================================

int _CaseFileDriver::getVariableIndex(const string & varName) const
{
  if ( _blocked || checkWasRead() )
    return 0;

  map< int, _Variable>::const_iterator ivar = _variables.begin();
  for ( ; ivar != _variables.end(); ++ivar )
  {
    if ( ivar->second._name == varName ) {
      if ( contains( "complex", ivar->second._type.c_str() ))
        return ivar->first - 1; // real part of complex variable
      return ivar->first;
    }
  }
  // maybe varName is "<true_varName>_Im"
  size_t _ImBeg = varName.size() - 3;
  if ( varName[ _ImBeg + 0 ] == '_' &&
       varName[ _ImBeg + 1 ] == 'I' &&
       varName[ _ImBeg + 2 ] == 'm' )
  {
    int i = getVariableIndex( varName.substr( 0, _ImBeg ));
    return ( i ? i + 1 : i ); // getVariableIndex() returns index for a real part
  }
  return 0;
}

//================================================================================
/*!
 * \brief sets all data necessary for fieldDriver::read()
 */
//================================================================================

int _CaseFileDriver::setDataFileName(const int                    varIndex,
                                     const int                    stepIndex,
                                     ENSIGHT_FIELD_RDONLY_DRIVER* fieldDriver)
{
  const char* LOC = "_CaseFileDriver::setDataFileName(): ";
  if ( _blocked || checkWasRead() )
    return 0;
  isToIgnore( fieldDriver ); // remeber

  map< int, _Variable>::iterator ivar = _variables.find( varIndex );
  if ( ivar == _variables.end() ) {
    // it can be index of real part of complex variable
    ivar = _variables.find( varIndex+1 );
    if ( ivar == _variables.end() || !contains( "complex", ivar->second._type.c_str() ))
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "invalid variable index: " << varIndex));
  }
  _Variable & var = ivar->second;

  const bool isComplex = contains( "complex", var._type.c_str() );
  const bool isImaginary = ( isComplex && varIndex == ivar->first );

  // complex scalar per element:    [ts] [fs] description Re_fn   Im_fn freq
  // complex scalar per node:       [ts] [fs] description Re_fn   Im_fn freq
  // complex vector per element:    [ts] [fs] description Re_fn   Im_fn freq
  // complex vector per node:       [ts] [fs] description Re_fn   Im_fn freq
  // constant per case file:        [ts]      description cvfilename
  // constant per case:             [ts]      description const_value(s)
  // scalar per element:            [ts] [fs] description filename
  // scalar per measured node:      [ts] [fs] description filename
  // scalar per node:               [ts] [fs] description filename
  // tensor asym per element:       [ts] [fs] description filename
  // tensor asym per node:          [ts] [fs] description filename
  // tensor symm per element:       [ts] [fs] description filename
  // tensor symm per node:          [ts] [fs] description filename
  // vector per element:            [ts] [fs] description filename
  // vector per measured node:      [ts] [fs] description filename
  // vector per node:               [ts] [fs] description filename

  FIELD_* field = fieldDriver->getField();

  // field name
  if ( field->getName().empty() ) {
    if ( isImaginary )
      field->setName( var._name + "_Im" );
    else
      field->setName( var._name );
  }
  // nb of components
  list<string> type_parts;
  _ASCIIFileReader::split( var._type, type_parts );
  string type1 = type_parts.front(); type_parts.pop_front();
  string type2 = type_parts.front(); type_parts.pop_front();
  int nbComponents = 1;
  if ( type1 == "vector" || type2 == "vector" )
    nbComponents = 3;
  else if ( type1 == "tensor" )
    nbComponents = ( type2 == "symm" ) ? 6 : 9;
  field->setNumberOfComponents( nbComponents );

  // component names
  vector<string> compNames( nbComponents );
  switch ( nbComponents ) {
  case 1:
    compNames[0] = type1;
    break;
  case 3: {
    const char* xyz[3] = { "X comp","Y comp","Z comp" };
    compNames.assign(xyz, xyz+3);
    break;
  }
  case 6: {
    const char* xyz[6] = { "11 comp", "22 comp", "33 comp","12 comp", "13 comp", "23 comp" };
    compNames.assign(xyz, xyz+6);
    break;
  }
  case 9: {
    const char* xyz[9] = { "11 comp", "12 comp", "13 comp", "21 comp", "22 comp", "23 comp",
                           "31 comp", "32 comp", "33 comp" };
    compNames.assign(xyz, xyz+9);
    break;
  }}
  field->setComponentsNames( & compNames[0] );

  // componet units
  vector<UNIT> units( nbComponents );
  vector<string> unitS( nbComponents ), descriptions( nbComponents );
  field->setComponentsUnits(&units[0]);
  field->setMEDComponentsUnits(&unitS[0]);
  field->setComponentsDescriptions(&descriptions[0]);

  // support type
  SUPPORT* sup = const_cast<SUPPORT*>( field->getSupport());
  if ( !sup ) {
    field->setSupport( sup = new SUPPORT );
    sup->removeReference(); // sup belongs to field
  }
  medEntityMesh entity = ( type_parts.back() == "node" ) ? MED_EN::MED_NODE : MED_EN::MED_CELL;
  sup->setEntity( entity );

  // data file name etc.
  list<string> fileData_parts;
  _ASCIIFileReader::split( var._fileNameOrData, fileData_parts );
  if ( isImaginary )
    fieldDriver->_dataFileName  = _directory + FILE_SEPARATOR + *(++fileData_parts.begin());
  else
    fieldDriver->_dataFileName  = _directory + FILE_SEPARATOR + fileData_parts.front();
  fieldDriver->_indexInDataFile = fixWildCardName( stepIndex,
                                                   var._timeSetNumber,
                                                   var._fileSetNumber,
                                                   fieldDriver->_dataFileName,
                                                   fieldDriver->_time);
  fieldDriver->_isGoldFormat    = ( _format == ENSIGHT_GOLD );
  fieldDriver->_transientMode   = ( !var._timeSetNumber.empty() );
  fieldDriver->_singleFileMode  = ( !_fileSets.empty() );

  if ( type1 == "constant" ) {
    if ( type_parts.back() == "file" ) {
      // append constant values from cvfilename to fileData_parts
      string cvfilename = _directory + FILE_SEPARATOR + fileData_parts.back();
      _ASCIIFileReader cvfile( cvfilename );
      fileData_parts.pop_back();
      while ( !cvfile.eof() )
        fileData_parts.push_back( cvfile.getWord() );
    }
    if ( (int)fileData_parts.size() < stepIndex )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "can't find value for step " << stepIndex
                                   << " of " << var._type << " " << var._name));
    list<string>::iterator value = fileData_parts.begin();
    advance( value, stepIndex-1 );
    fieldDriver->setConstantValue( *value );
  }

  // mesh data
  int meshIndex = 1;
  if ( _model._fileName.find('*') != _model._fileName.npos ) {
    _TimeSet& ts = _timeSets[ _ATOI( _model._timeSetNumber )];
    vector<string>::iterator t = find( ts._times.begin(), ts._times.end(), fieldDriver->_time );
    if ( t != ts._times.end() )
      meshIndex += distance( ts._times.begin(), t );
  }
  fieldDriver->_imedMapKey = STRING(_fileName) << ":" << meshIndex;

  // time
  if ( fieldDriver->_transientMode ) {
    field->setTime( _ATOF( fieldDriver->_time ));
    //field->setOrderNumber( stepIndex );
    field->setIterationNumber( stepIndex );
  }
  return meshIndex;
}

//================================================================================
/*!
 * \brief Throws if case file has not been read else return false
 */
//================================================================================

bool _CaseFileDriver::checkWasRead() const throw (MEDEXCEPTION)
{
  if ( _model._fileName.empty() )
    throw MEDEXCEPTION(STRING("Case file ") << _fileName << " has not been successfully read");
  return false;
}

//================================================================================
/*!
 * \brief replace '*' in file name if any and return index in file
 */
//================================================================================

int  _CaseFileDriver::fixWildCardName(const int      timeStep,
                                      const string & ts,
                                      const string & fs,
                                      string &       fileName,
                                      string &       time)
{
  int indexInFile = 0;
  string fileIndex;

  STRING badFile("Invalid Case file ");
  badFile << _fileName << "\n";

  if ( !fs.empty() ) { // single file mode
    const _FileSet & fileSet = _fileSets[ _ATOI( fs ) ];
    if ( fileSet._fileIndex.empty() ) { // no file continuation
      indexInFile = timeStep;
    }
    else {
      list<int>::const_iterator   nbStepsIt = fileSet._nbStepsInFile.begin();
      list<string>::const_iterator fIndexIt = fileSet._fileIndex.begin();
      int nbSteps = 0;
      for ( ; nbStepsIt != fileSet._nbStepsInFile.end(); ++nbStepsIt ) {
        if ( nbSteps + *nbStepsIt <= timeStep )
          break;
        nbSteps += *nbStepsIt;
      }
      if ( nbStepsIt == fileSet._nbStepsInFile.end() )
        throw MEDEXCEPTION(LOCALIZED(badFile << "Cant'f find file index for time step " <<
                                     timeStep << " in file set " << fs ));
      indexInFile = timeStep - nbSteps;
      fileIndex   = *fIndexIt;
    }
  }

  string head, queue;
  _ASCIIFileReader::split( fileName, head, queue, '*' );
  int indexWidth = fileName.size() - head.size() - queue.size();

  if ( indexWidth > 0 || !ts.empty() || timeStep > 1 ) {
    int tsId = ts.empty() ? 1 : _ATOI( ts );
    const _TimeSet& timeSet = _timeSets[ tsId ];
    if ( timeStep > (int)timeSet._times.size() )
      throw MEDEXCEPTION(LOCALIZED(badFile << "Cant'f find time for time step " <<
                                   timeStep << " in time set " << ts ));
    time = timeSet._times[ timeStep-1 ];
    if ( timeStep-1 < (int)timeSet._fileIndex.size()  )
      fileIndex = timeSet._fileIndex[ timeStep-1 ];
    if ( !indexInFile )
      indexInFile = 1;
  }
      
  if ( indexWidth > 0 )
  {
    if ( fileIndex.empty() ) {
      throw MEDEXCEPTION(LOCALIZED(badFile << "Can't find file index for time step " <<
                                   timeStep << " in time set <" << ts <<
                                   "> and file set <" << fs << ">"));
    }
    if ( indexWidth ==  (int)fileIndex.size() ) {
      fileName = head + fileIndex + queue;
    }
    else {
      fileName = (STRING(head) << setw(indexWidth) << setfill('0') << fileIndex << queue);
    }
  }
  return indexInFile;
}

//================================================================================
/*!
 * \brief add a mesh to the Case file
 */
//================================================================================

void _CaseFileDriver::addMesh(const ENSIGHT_MESH_WRONLY_DRIVER* meshDriver)
{
  if ( _blocked )
    return;

  _meshDrivers.push_back( const_cast<ENSIGHT_MESH_WRONLY_DRIVER*>( meshDriver ));

  if ( _format == ENSIGHT_6 )
  {
    const GMESH* mesh = _meshDrivers.back()->getMesh();
    if ( mesh->getNumberOfElements(MED_EN::MED_CELL, MED_POLYGON) > 0 ||
         mesh->getNumberOfElements(MED_EN::MED_FACE, MED_POLYGON) > 0 ||
         mesh->getNumberOfElements(MED_EN::MED_CELL, MED_POLYHEDRA) > 0 )
      throw MEDEXCEPTION
        ( compatibilityPb(STRING("Can't write mesh <") << mesh->getName() <<
                          "> since Ensight6 format does not support poly elements,"
                          " use Ensight Gold format instead: call "
                          "setEnSightFormatForWriting( ENSIGHT_GOLD )"));
  }

  isToIgnore( meshDriver ); // remeber
}

//================================================================================
/*!
 * \brief add a field to the Case file
 */
//================================================================================

void _CaseFileDriver::addField(const ENSIGHT_FIELD_WRONLY_DRIVER * theFieldDriver)
{
  if ( _blocked )
    return;

  ENSIGHT_FIELD_WRONLY_DRIVER * fieldDriver =
    const_cast<ENSIGHT_FIELD_WRONLY_DRIVER*>( theFieldDriver );

  FIELD_* field = fieldDriver->getField();

  STRING problem;
  if ( field->getNumberOfValues() == 0 )
    problem << "getNumberOfValues() == 0";
  else if ( field->getGaussPresence() )
    problem << compatibilityPb("Gauss points are not supported by EnSight v8");
  else if ( !field->getSupport() )
    problem << "it has NULL support";

  switch ( field->getNumberOfComponents() ) {
  case 1:
  case 3:
  case 6:
  case 9: break; // ok, supported
  case 2:
    if ( const GMESH* mesh = field->getSupport()->getMesh() )
      if ( mesh->getSpaceDimension() == 2 )
        break; // we add one component to both mesh and field
  default:
    problem <<
      compatibilityPb(STRING("it has ") << field->getNumberOfComponents()
                      << " components but only 1,3,6 and 9 components are supported by EnSight");
  }
  if ( !problem.empty() )
    throw MEDEXCEPTION(STRING("Can't write field <") << field->getName() <<
                       "> to EnSight: " << problem);

  string fieldName = fieldDriver->getFieldName();
  if ( fieldName.empty() )
    fieldName = field->getName();
  if ( fieldName.empty() )
    fieldName = STRING("med_field_")<<_fieldDrivers.size();
  else { // replace illegal characters
    string::size_type pos = fieldName.find_first_of( ILLEGAL_FIELD_NAME_CHARACTERS );
    while ( pos != fieldName.npos ) {
      fieldName[ pos ] = '_';
      pos = fieldName.find_first_of( ILLEGAL_FIELD_NAME_CHARACTERS );
    }
  }
    
  _fieldDrivers[ fieldName ].push_back( fieldDriver );

  isToIgnore( fieldDriver ); // remeber
}

//================================================================================
/*!
 * \brief writing Case file
 */
//================================================================================

void _CaseFileDriver::write() throw (MEDEXCEPTION)
{
  if ( _blocked )
    return;

  // Make case file data from added drivers

  bool dataIsRead = !_model._fileName.empty();
  if ( !dataIsRead && _meshDrivers.empty() )
    throw MEDEXCEPTION("no mesh to write into Case file");

  const int defaultNbDigits = 3; // in file index

  string path, name, ext;
  _ASCIIFileReader::split( _fileName, path, name, FILE_SEPARATOR, /*fromBack=*/true);
  _ASCIIFileReader::split( name, name, ext, '.', /*fromBack=*/true);
  if ( name.empty())
    name = ext; // _fileName: "path/.name"

  list<ENSIGHT_MESH_WRONLY_DRIVER*>::iterator mDrv = _meshDrivers.begin();
  TFieldDriversByName::iterator               fDrv = _fieldDrivers.begin();

  map< string, string > fileToRenameTo;

  int i, nbOldMeshes = 0, nbNewMeshes = _meshDrivers.size();

  if ( nbNewMeshes > 0 )
  {
    if ( dataIsRead )
    {
      // A mesh is going to be added into an existing case file.
      // Check that number of parts is same in the existing model
      // and in the added ones
      string geoFileName     = _directory + FILE_SEPARATOR + _model._fileName;
      string realGeoFileName = geoFileName, time;
      fixWildCardName( 1,
                       _model._timeSetNumber,
                       _model._fileSetNumber,
                       realGeoFileName,
                       time);
      int nbParts = ENSIGHT_MESH_RDONLY_DRIVER::countParts(realGeoFileName,
                                                           !_model._fileSetNumber.empty());
      for ( ; mDrv != _meshDrivers.end(); ++mDrv )
      {
        int nbNewParts = (*mDrv)->nbPartsToWrite();
        if ( nbParts != nbNewParts )
          throw MEDEXCEPTION(compatibilityPb("Can't add a mesh ") << (*mDrv)->getMesh()->getName()
                             << "to " << _fileName << " as number of parts in the file "
                             "differs from number of parts going to be written");
      }
      if ( !_model._timeSetNumber.empty() && !_variables.empty() ) {
        // Make time set of the model independent of that of variables as
        // the number of time steps is apparently becoming different
        map< int, _Variable>::iterator ivar = _variables.begin();
        for ( ; ivar != _variables.end(); ++ivar ) {
          if ( ivar->second._timeSetNumber == _model._timeSetNumber ) {
            int tsNum    = _timeSets.rbegin()->first + 1;
            _TimeSet& ts = _timeSets[ tsNum ];
            ts._number   = tsNum;
            ts           = _timeSets[ _ATOI( _model._timeSetNumber )];
            _model._timeSetNumber = STRING(tsNum);
            break;
          }
        }
      }
      string::size_type digitPos = _model._fileName.find('*');
      bool isSeveralFiles = ( digitPos != _model._fileName.npos );
      int nbDigits = defaultNbDigits;
      if ( isSeveralFiles ) {
        nbDigits = 1;
        while ( _model._fileName[ ++digitPos ] == '*' )
          nbDigits++;
      }
      // Update time set and file set of the model 
      int nbMeshes;
      if ( _model._timeSetNumber.empty() ) {
        // old model is static, create a time set
        nbMeshes = nbNewMeshes + 1;
        int tsNum    = _timeSets.empty() ? 1 : _timeSets.rbegin()->first + 1;
        _TimeSet& ts = _timeSets[ tsNum ];
        ts._number   = tsNum;
        _model._timeSetNumber = STRING(tsNum);
        // fill the new time set
        ts._fileIndex.resize( nbMeshes );
        ts._times.resize    ( nbMeshes );
        for ( i = 0; i < nbMeshes; ++i ) {
          ts._fileIndex[ i ] = (STRING() << setw(nbDigits) << setfill('0') << i);
          ts._times    [ i ] = STRING(i);
        }
        // not to create equal time sets
        map< int, _TimeSet >::iterator its, tsEnd = _timeSets.end();
        for ( --tsEnd, its = _timeSets.begin(); its != tsEnd; ++its ) {
          if ( ts == its->second ) {
            _model._timeSetNumber = STRING( its->first );
            _timeSets.erase( tsEnd );
            break;
          }
        }
      }
      else {
        // old model is transient, add times and file indices for new meshes
        _TimeSet& ts = _timeSets[ _ATOI( _model._timeSetNumber )];
        nbOldMeshes = ts._times.size();
        nbMeshes = nbNewMeshes + nbOldMeshes;
        ts._times.resize( nbMeshes );
        double time = 1. + _ATOF( ts._times[ nbOldMeshes-1 ] );
        for ( i = nbOldMeshes; i < nbMeshes; ++i, time+=1. )
          ts._times[ i ] = STRING( time );
        if ( _model._fileSetNumber.empty() ) { // multi-file mode
          ts._fileIndex.resize( nbMeshes, string(nbDigits,'0'));
          i = nbOldMeshes;
          int index = 1 + _ATOI( ts._fileIndex[ i-1 ] );
          for ( ; i < nbMeshes; ++i, ++index )
            ts._fileIndex[ i ] = (STRING() << setw(nbDigits) << setfill('0') << index);
        }
        else { // single-file mode
          _FileSet & fs = _fileSets[ _ATOI( _model._fileSetNumber )];
          for ( i = 0; i < nbNewMeshes; ++i )
            fs._nbStepsInFile.push_back( 1 );
          int index = 1;
          if ( fs._fileIndex.empty() )
            fs._fileIndex.push_back(string(nbDigits,'0'));
          else
            index += _ATOI( fs._fileIndex.back() );
          for ( i = fs._fileIndex.size(); i < nbMeshes; ++i, ++index )
            ts._fileIndex[ i ] = (STRING() << setw(3) << setfill('0') << index);
        }
      }
      // file of the old model is to be renamed
      if ( !isSeveralFiles ) {
        _model._fileName += string(nbDigits,'*');
        fileToRenameTo[ geoFileName ] = geoFileName + string(nbDigits,'0');
      }
    }
    else if ( nbNewMeshes > 1 )
    {
      // Store meshes into a new case file: create a time set
      int tsNum    = _timeSets.empty() ? 1 : _timeSets.rbegin()->first + 1;
      _TimeSet& ts = _timeSets[ tsNum ];
      ts._number   = tsNum;
      _model._timeSetNumber = STRING(tsNum);
      _model._fileName      = name + ".geo" + string(defaultNbDigits, '*');
      // fill the new time set
      ts._fileIndex.resize( nbNewMeshes );
      ts._times.resize    ( nbNewMeshes );
      for ( i = 0; i < nbNewMeshes; ++i ) {
        ts._fileIndex[ i ] = (STRING() << setw(defaultNbDigits) << setfill('0') << i);
        ts._times    [ i ] = STRING(i);
      }
    }
    else {
      // One mesh in a new file
      _model._fileName = name + ".geo";
    }

    // Set data to mesh drivers
    i = nbOldMeshes + 1;
    for ( mDrv = _meshDrivers.begin(); mDrv != _meshDrivers.end(); ++mDrv, ++i )
    {
      _CaseFileDriver_User* meshDriver = (*mDrv);
      meshDriver->_dataFileName    = _directory + FILE_SEPARATOR + _model._fileName;
      meshDriver->_indexInDataFile = fixWildCardName( i,
                                                      _model._timeSetNumber,
                                                      _model._fileSetNumber,
                                                      meshDriver->_dataFileName,
                                                      meshDriver->_time);
      meshDriver->_isGoldFormat    = ( _format == ENSIGHT_GOLD );
      meshDriver->_transientMode   = ( meshDriver->_indexInDataFile > 0 );
      meshDriver->_singleFileMode  = ( !_fileSets.empty() );
      meshDriver->_imedMapKey      = STRING(_fileName)<<":"<<i;
    }
  }

  typedef map< double, ENSIGHT_FIELD_WRONLY_DRIVER* > FDriverByDouble;

  //bool isVarsRead = ( !_variables.empty() );
  for ( ; fDrv != _fieldDrivers.end(); ++fDrv )
  {
    const string &                       fieldName = fDrv->first;
    list< ENSIGHT_FIELD_WRONLY_DRIVER* > & drivers = fDrv->second;

    int nbNewSteps = drivers.size();

    // find out by which parameter fields differ and sort them by the parameter
    FDriverByDouble timeMap, iterMap, orderMap;
    list< ENSIGHT_FIELD_WRONLY_DRIVER* >::iterator drv;
    for ( drv = drivers.begin(); drv != drivers.end(); ++drv ) {
      FIELD_* field = (*drv)->getField();
      double time = field->getTime();
      int    iter = field->getIterationNumber();
      int    ordr = field->getOrderNumber();
      timeMap.insert  ( make_pair( time, *drv ));
      iterMap.insert  ( make_pair( iter, *drv ));
      orderMap.insert ( make_pair( ordr, *drv ));
    }
    FDriverByDouble * sortedDrivers;
    FDriverByDouble::iterator tDrv;
    if ( (int)timeMap.size() == nbNewSteps )
      sortedDrivers = & timeMap;
    else if ( (int)iterMap.size() == nbNewSteps )
      sortedDrivers = & iterMap;
    else if ( (int)orderMap.size() == nbNewSteps )
      sortedDrivers = & orderMap;
    else {
      timeMap.clear();
      sortedDrivers = & timeMap;
      for ( drv = drivers.begin(); drv != drivers.end(); ++drv ) {
        double time = (*drv)->getField()->getTime();
        if ( ! timeMap.insert( make_pair( time, *drv )).second )
          timeMap.insert( make_pair( timeMap.rbegin()->first + 1., *drv ));
      }
    }

//     if ( isVarsRead ) {
//       int iVar = getVariableIndex( fieldName );
//       if ( iVar > 0 ) {
//         // A variable with fieldName already exists,
//         // add more time steps to it
//         _Variable& var = _variables[ iVar ];
//         _TimeSet& ts = _timeSets[ _ATOI( var._timeSetNumber )];
//         int nbOldSteps = ts._times.size();
//       }
//     }
    FIELD_* field = drivers.front()->getField();
    int varNum = _variables.size() + 1;
    _Variable& var = _variables[ varNum ];
    var._name = fieldName;
    switch ( field->getNumberOfComponents() ) {
    case 1: var._type = "scalar "; break;
    case 2: var._type = "vector "; break;// we add one component to a vector in 2d space
    case 3: var._type = "vector "; break;
    case 6: var._type = "tensor symm "; break;
    case 9: var._type = "tensor asym "; break;
    }
    if ( field->getSupport()->getEntity() == MED_EN::MED_NODE )
      var._type += "per node";
    else
      var._type += "per element";
    var._fileNameOrData = name + "." + fieldName;

    // always create Time set to store time
    int nbDigits = defaultNbDigits;
    int tsNum = _timeSets.empty() ? 1 : _timeSets.rbegin()->first + 1;
    var._timeSetNumber = STRING( tsNum );
    _TimeSet & ts = _timeSets[ tsNum ];
    ts._number    = tsNum;
    ts._fileIndex.resize( nbNewSteps );
    ts._times.resize    ( nbNewSteps );
    tDrv = sortedDrivers->begin();
    for ( i = 0; tDrv != sortedDrivers->end(); ++tDrv, ++i ) {
      ts._times    [ i ] = (STRING( tDrv->first ));
      ts._fileIndex[ i ] = (STRING() << setw(nbDigits) << setfill('0') << i);
    }
    if ( nbNewSteps > 1 )
      var._fileNameOrData += string( nbDigits, '*' );
    else
      ts._fileIndex.clear();
    // not to create equal time sets
    map< int, _TimeSet >::iterator its, tsEnd = _timeSets.end();
    for ( --tsEnd, its = _timeSets.begin(); its != tsEnd; ++its ) {
      if ( ts == its->second ) {
        tsNum = its->first;
        var._timeSetNumber = STRING( tsNum );
        _timeSets.erase( tsEnd );
        break;
      }
    }
    tDrv = sortedDrivers->begin();
    for ( i = 1; tDrv != sortedDrivers->end(); ++tDrv, ++i ) {
      _CaseFileDriver_User* fieldDriver = tDrv->second;
      fieldDriver->_dataFileName    = _directory + FILE_SEPARATOR + var._fileNameOrData;
      fieldDriver->_indexInDataFile = fixWildCardName( i,
                                                       var._timeSetNumber,
                                                       var._fileSetNumber,
                                                       fieldDriver->_dataFileName,
                                                       fieldDriver->_time);
      fieldDriver->_isGoldFormat    = ( _format == ENSIGHT_GOLD );
      fieldDriver->_transientMode   = ( fieldDriver->_indexInDataFile > 0 );
      fieldDriver->_singleFileMode  = ( !_fileSets.empty() );
    }

    // do not refer to time set if there is one step (issue 0020113)
    if ( nbNewSteps == 1 )
      var._timeSetNumber = "";

  } // loop on _fieldDrivers

  if ( nbOldMeshes + nbNewMeshes > 1 && !_variables.empty() )
  {
    // if there are variables on a transient model, model should not change much,
    // nb of entities in all parts with values must be same
    if ( nbOldMeshes == 0 ) {
      // TODO: check consistency
    }
    if( toIgnoreIncompatibility() )
      std::cerr << "Warning: EnSight file will be probably invalid " << _fileName << endl;
    else
      throw MEDEXCEPTION
        (compatibilityPb(STRING("EnSight file will be invalid if fields refer to "
                                "the second mesh, which differ from the first one too much.\n"
                                "If you are sure in data correctness, you can suppress "
                                "this exception by calling setIgnoreIncompatibility(1)\n")
                         << _fileName));
  }

  // Write a file

  ofstream caseFile( _fileName.c_str(), ios::out );
  if ( !caseFile )
    throw MEDEXCEPTION(STRING("Can't open file for writing ") << _fileName);

  caseFile << "# generated by MEDMEM-to-EnSight driver" << endl << endl
           << "FORMAT" << endl
           << "type: " << (_format==ENSIGHT_GOLD ? "ensight gold" : "ensight") << endl
           << endl
           << "GEOMETRY"
           << endl
           << "model:\t"
           << _model._timeSetNumber << " "
           << _model._fileSetNumber << " "
           << _model._fileName << " \t"
           << _model._change_coords_only
           << endl << endl;

  // write VARIABLE section
  if ( !_variables.empty() )
  {
    caseFile << "VARIABLE" << endl;

    map< int, _Variable>::iterator ivar = _variables.begin();
    for ( ; ivar != _variables.end(); ++ivar )
    {
      _Variable& var = ivar->second;
      caseFile << var._type << ": \t"
               << var._timeSetNumber << " "
               << var._fileSetNumber << " \t"
               << var._name << " \t"
               << var._fileNameOrData << endl;
    }
    caseFile << endl;
  }
  // write TIME section
  if ( !_timeSets.empty() )
  {
    caseFile << "TIME" << endl;

    map< int, _TimeSet>::iterator its = _timeSets.begin();
    for ( ; its != _timeSets.end(); ++its )
    {
      _TimeSet & ts = its->second;
      caseFile << "time set:\t"        << ts._number << endl
               << "number of steps:\t" << ts._times.size() << endl;
      if ( !ts._fileIndex.empty() ) {
        STRING numbers( "filename numbers:" );
        for ( unsigned i = 0; i < ts._fileIndex.size(); ++i ) {
          if (int( numbers.size() + ts._fileIndex[i].size() + 2) > MAX_LINE_LENGTH ) {
            caseFile << numbers << endl;
            numbers = STRING();
          }
          numbers << " " << ts._fileIndex[i];
        }
        caseFile << numbers << endl;
      }
      STRING times( "time values:" );
      for ( unsigned i = 0; i < ts._times.size(); ++i ) {
        if (int( times.size() + ts._times[i].size() + 2 ) > MAX_LINE_LENGTH ) {
          caseFile << times << endl;
          times = STRING();
        }
        times << " " << ts._times[i];
      }
      caseFile << times << endl;
    }
  }
  // write FILE section
  if ( !_fileSets.empty() )
  {
    caseFile << "FILE" << endl;

    map< int, _FileSet >::iterator ifs = _fileSets.begin();
    for ( ; ifs != _fileSets.end(); ++ifs )
    {
      _FileSet & fs = ifs->second;
      caseFile << "file set: " << fs._number << endl;

      list<int>::iterator nbSteps = fs._nbStepsInFile.begin();
      list<string>::iterator fileIndex = fs._fileIndex.begin();
      for ( ; nbSteps != fs._nbStepsInFile.end(); ++nbSteps )
      {
        if ( fileIndex != fs._fileIndex.end() )
          caseFile << "filename index: " << *fileIndex++;
        caseFile << "number of steps: " << *nbSteps << endl;
      }
    }
  }

  caseFile.close();

} // _CaseFileDriver::write()


//================================================================================
/*!
 * \brief _CaseFileDriver_User constructor
 */
//================================================================================

_CaseFileDriver_User::_CaseFileDriver_User(const string & caseFileName,
                                           MED_EN::med_mode_acces mode)
  : GENDRIVER( caseFileName, mode, ENSIGHT_DRIVER ), _imed(0)
{
}

//================================================================================
/*!
 * \brief take data from other driver
 */
//================================================================================

void _CaseFileDriver_User::merge( const GENDRIVER& driver)
{
  const _CaseFileDriver_User* other = dynamic_cast< const _CaseFileDriver_User* >( &driver );
  if ( other ) {
    _dataFileName    = other->_dataFileName   ;
    _isGoldFormat    = other->_isGoldFormat   ;
    _transientMode   = other->_transientMode  ;  
    _singleFileMode  = other->_singleFileMode ; 
    _indexInDataFile = other->_indexInDataFile;
    _time            = other->_time           ;           
    _imed            = other->_imed           ;            
    _imedMapKey      = other->_imedMapKey     ;      
  }
}

//================================================================================
/*!
 * \brief analyse if data file is binary
 */
//================================================================================

bool _CaseFileDriver_User::isBinaryDataFile(const string& dataFileName)
{
#ifdef WNT
  int _file = ::_open (dataFileName.c_str(), _O_RDONLY|_O_BINARY);
#else
  int _file = ::open (dataFileName.c_str(), O_RDONLY);
#endif
  char buf[81];
  int nBytesRead = ::read (_file, buf, 80);

  bool isBinary = true;

  const char cBin[] = "C Binary";
  const char fBin[] = "Fortran Binary";
  if ( strncmp( buf, cBin, sizeof(cBin)-1) != 0 &&
       strncmp( buf, fBin, sizeof(fBin)-1) != 0 )
  {
    for ( int i = nBytesRead-1; i >= 0 && isBinary; --i )
      isBinary = ( buf[ i ] != '\n' );
  }

  ::close (_file);

  return isBinary;
}

//================================================================================
/*!
 * \brief return part number to write support with, zero in failure case
 */
//================================================================================

int _CaseFileDriver_User::getPartNumber(const SUPPORT* support) const
{
  bool isGroup = ( dynamic_cast<const GROUP*>( support ));
  bool isForField = ( dynamic_cast<const ENSIGHT_FIELD_DRIVER*>( this ));
  medEntityMesh entity = support->getEntity();
  const GMESH* mesh = support->getMesh();

  // for supports on all entities, reserve numbers corresponding to entity
  bool isOnAll = support->isOnAllElements();
  if (!isOnAll && mesh ) {
    int nbMeshElem = mesh->getNumberOfElements(entity, MED_ALL_ELEMENTS);
    int nbSuppElem = support->getNumberOfElements(MED_ALL_ELEMENTS);
    isOnAll = (  nbSuppElem == nbMeshElem );
  }
  if ( !isGroup ) {
    if ( !isOnAll )
      return 0;
    else if ( entity == MED_EN::MED_NODE )
      return 1 + MED_EN::MED_CELL; // all nodes are described with all CELLs
    else
      return 1 + entity;
  }
  if ( isForField && isOnAll ) {
    if ( entity == MED_EN::MED_NODE )
      return 1 + MED_EN::MED_CELL; // all nodes are described with all CELLs
    else
      return 1 + entity;
  }
  if ( !mesh )
    return 0;

  int partNum = MED_ALL_ENTITIES + 1;
  for ( int ent = MED_EN::MED_CELL; ent < MED_ALL_ENTITIES; ++ent ) {
    entity = (medEntityMesh) ent;
    int nbGroups = mesh->getNumberOfGroups(entity);
    if ( entity != support->getEntity() ) {
      partNum += nbGroups;
    }
    else {
      for ( int i=1; i<=nbGroups; ++i, ++partNum)
        if ( support == mesh->getGroup( entity, i ))
          return partNum;
    }
  }
  throw MED_EXCEPTION
    ( LOCALIZED( STRING("Can't find GROUP ") << support->getName() << " in its MESH"));

  return 0;
}

//================================================================================
/*!
 * \brief Return true if we strore an enetuty to EnSight
 */
//================================================================================

bool _CaseFileDriver_User::isToWriteEntity(const medEntityMesh entity,
                                           const GMESH*        mesh)
{
  if ( entity == MED_EN::MED_NODE )
    return mesh->getNumberOfNodes() > 0;

  if ( mesh->getNumberOfElements( entity, MED_ALL_ELEMENTS ) < 1 )
    return false;
  if ( entity == MED_EN::MED_CELL )
    return true;
  int meshDim = mesh->getTypes(MED_EN::MED_CELL)[0] / 100;
  if ( entity == MED_EN::MED_FACE )
    return ( meshDim == 3 || meshDim == 5 );
  if ( entity == MED_EN::MED_EDGE )
    return ( meshDim == 2 || meshDim == 4 );

  return false;
}

//================================================================================
/*!
 * \brief Return nodes of non-nodal support, which is not on all entities
 */
//================================================================================

void _CaseFileDriver_User::getSupportNodes(const SUPPORT* support, map<int, int> & nodeIds)
{
  medEntityMesh entity = support->getEntity();

  const medConnectivity conType     = MED_EN::MED_NODAL;
  const medGeometryElement allGeoms = MED_ALL_ELEMENTS;
  const int * connectivity          = 0;
  const int * elemConnectivity      = 0;
  const int * index                 = 0;
  const int * number                = 0;
  int j;

  if ( support->isOnAllElements() )
  {
    if ( entity == MED_EN::MED_NODE ) { // all NODES
      int numberOfCell = support->getNumberOfElements(allGeoms);
      while ( numberOfCell ) {
        nodeIds.insert( nodeIds.begin(), make_pair( numberOfCell, numberOfCell ));
        --numberOfCell;
      }
    }
    else {
      const MESH* mesh = support->getMesh()->convertInMESH();
      int conLength = 0;
      connectivity = mesh->getConnectivity      (conType, entity, allGeoms);
      conLength    = mesh->getConnectivityLength(conType, entity, allGeoms);
      while ( conLength-- ) nodeIds[ *connectivity++ ];
      mesh->removeReference();
    }
    return;
  }

  if ( entity == MED_EN::MED_NODE )
  {
    number           = support->getNumber(MED_ALL_ELEMENTS);
    int numberOfCell = support->getNumberOfElements(MED_ALL_ELEMENTS);
    for (j=0; j < numberOfCell; j++)
      nodeIds.insert(nodeIds.end(), make_pair( number[j], j ));
    return;
  }

  number           = support->getNumber(MED_ALL_ELEMENTS);
  int numberOfCell = support->getNumberOfElements(MED_ALL_ELEMENTS);
  const MESH* mesh = support->getMesh()->convertInMESH();
  index        = mesh->getConnectivityIndex( MED_EN::MED_NODAL, entity);
  connectivity = mesh->getConnectivity( MED_EN::MED_NODAL, entity, MED_ALL_ELEMENTS);
  for ( j = 0; j < numberOfCell; ++j )
  {
    int elem = number[j];
    elemConnectivity   = connectivity + index[elem-1]-1;
    const int* connEnd = connectivity + index[elem]-1;
    while ( elemConnectivity < connEnd )
      nodeIds[ *elemConnectivity++ ];
  }
  mesh->removeReference();
}

//================================================================================
/*!
 * \brief method called by mesh driver at reading to store data to be used by
 *        field driver
 */
//================================================================================

void _CaseFileDriver_User::setInterData(_InterMed* imed )
{
  theInterMedMap[ _imedMapKey ] = imed;
  if ( ENSIGHT_MESH_DRIVER* mDrv = dynamic_cast<ENSIGHT_MESH_DRIVER*>( this )) {
    imed->_medMesh = dynamic_cast<MESH*>( mDrv->getMesh() );
    imed->_isOwnMedMesh = false;
  }
  else
    imed->_medMesh = 0;
}

//================================================================================
/*!
 * \brief for field driver to get mesh data
 */
//================================================================================

_InterMed* _CaseFileDriver_User::getInterData()
{
  return _imed = getMeshData( _imedMapKey );
}

//================================================================================
/*!
 * \brief return _SubPart by its description
 */
//================================================================================

_SubPart* _CaseFileDriver_User::getSubPart(const _SubPartDesc & descriptor)
  throw (MEDEXCEPTION)
{
  if ( !_imed )
    _imed = getMeshData( _imedMapKey );

  map< _SubPartDesc, _SubPart >::iterator descPart = 
    _imed->_subPartDescribed.find( descriptor );

  if ( descPart != _imed->_subPartDescribed.end() )
    return & descPart->second;

  if ( descriptor == _SubPartDesc::globalCoordDesc() )
    return 0; // if the mesh is structured, there are no global coordinates (EnSight 6)

  throw MEDEXCEPTION(LOCALIZED(STRING("No mesh info for part ") << descriptor.partNumber()
                               << " " << descriptor.typeName()));
}

//================================================================================
/*!
 * \brief return _Support by its description
 */
//================================================================================

_Support* _CaseFileDriver_User::getSupport(const _SupportDesc & descriptor,
                                           const medEntityMesh  entity)
  throw (MEDEXCEPTION)
{
  const char* LOC = "_CaseFileDriver_User::getSupport(): ";
  if ( !_imed )
    _imed = getMeshData( _imedMapKey );

  _imed->treatGroupes();

  if ( _imed->_supportDescribed.empty() && !_imed->_subPartDescribed.empty() )
  {
    // fill _supportDescribed with _Support's corresponding to EnSight parts
    for (unsigned int i=0; i < _imed->groupes.size(); ++i)
    {
      _groupe& grp = _imed->groupes[i];
      if ( !grp.medGroup ) continue;
      
      vector<int> grpIndices(1,i);
      if ( !grp.groupes.empty() )
        grpIndices.assign( grp.groupes.begin(), grp.groupes.end());

      _SupportDesc supDescriptor;
      // look for a subpart for each _groupe
      vector<int>::iterator grIndex = grpIndices.begin(), grIndexEnd = grpIndices.end();
      for ( ; grIndex != grIndexEnd; ++grIndex )
      {
        map< _SubPartDesc, _SubPart >::iterator descSub;
        for ( descSub  = _imed->_subPartDescribed.begin();
              descSub != _imed->_subPartDescribed.end();
              ++descSub)
        {
          if ( descSub->second.myCellGroupIndex == *grIndex ) {
            supDescriptor.insert( descSub->first );
            break;
          }
        }
      }
      if ( supDescriptor.size() == grpIndices.size() ) {
        _Support & sup = _imed->_supportDescribed[ supDescriptor ];
        sup.setGroup( & grp );
      }
    }
  }

  // find the support by its description
  map< _SupportDesc, _Support >::iterator descSup =
    _imed->_supportDescribed.find( descriptor );

  // Create a new support
  if ( descSup == _imed->_supportDescribed.end() || !descSup->second.medSupport( entity ))
  {
    // create a _groupe composed of groups corresponding to subparts
    _imed->groupes.push_back(_groupe());
    _groupe& grp = _imed->groupes.back();
    grp.groupes.reserve( descriptor.size() );

    // to detect dimension change within a support group
    set<int> dimensions;

    // fill grp with sub-group indices
    _SupportDesc::const_iterator subPartDesc = descriptor.begin();
    for ( ; subPartDesc != descriptor.end(); ++subPartDesc )
    {
      const _SubPart* subPart = getSubPart(*subPartDesc);
      if ( !subPart )
        throw MEDEXCEPTION(LOCALIZED(STRING("No mesh info for ") << *subPartDesc));

      int groupIndex =
        (entity == MED_EN::MED_NODE) ? subPart->myNodeGroupIndex : subPart->myCellGroupIndex;
      if ( groupIndex < 1 ) {
        if ( entity == MED_EN::MED_NODE )
        {
          // make a _groupe of nodes
          _imed->groupes.push_back(_groupe());
          groupIndex = subPart->myNodeGroupIndex = _imed->groupes.size();
          _groupe & groupe = _imed->groupes.back();
          groupe.mailles.resize( subPart->myNbNodes );

          _maille ma( MED_EN::MED_POINT1, 1 );
          ma.sommets.resize( 1 );
          map< int, _noeud >::iterator n = subPart->myFirstNode;
          for (int i = 0; i < subPart->myNbNodes; ++i, ++n ) {
            ma.sommets[0] = n;
            ma.setOrdre( n->second.number );
            groupe.mailles[i] = _imed->insert(ma);
          }
        }
        else
        {
          throw MEDEXCEPTION(LOCALIZED(STRING("No cell info for ") << *subPartDesc));
        }
      }
      grp.groupes.push_back( groupIndex );

      // find out subpart dimension
      if ( entity != MED_EN::MED_NODE )
        dimensions.insert( _imed->groupes[ groupIndex-1 ].maille(0).dimensionWithPoly() );
    }
    // check if MEDMEM allows creating such a support
    if ( dimensions.size() > 1 )
      throw MEDEXCEPTION
        (compatibilityPb(LOC) << "can't create a SUPPORT for the field from "
         << _dataFileName << ", since it includes different mesh entities");

    ENSIGHT_MESH_RDONLY_DRIVER::makeGroup( grp, *_imed );

    // add _Support
    descSup = _imed->_supportDescribed.insert( make_pair( descriptor, _Support() )).first;
    _Support & sup = descSup->second;
    sup.setGroup( & grp );
      
  } // new SUPPORT creation

  _Support* sup = & descSup->second;

  // remove temporary mesh from med SUPPORT
  if ( _imed->_isOwnMedMesh )
  {
    if ( sup->medSupport( entity )->getMesh() == _imed->_medMesh )
      _imed->_medMesh->addReference(); // don't want _medMesh to die.
    sup->medSupport( entity )->setMesh( 0 );
    sup->medSupport( entity )->setMeshName( _imed->_medMesh->getName() );
  }

  return sup;
}

//================================================================================
/*!
 * \brief check possibility to open a file with a given mode
 */
//================================================================================

bool _CaseFileDriver_User::canOpenFile(const string&  fileName,
                                       MED_EN::med_mode_acces mode)
{
  bool ok = false;
  if ( mode == WRONLY ) {
    fstream file( fileName.c_str(),
                  ios::app | ios_base::out ); // not to overwrite it, just to check
    ok = bool(file);
  }
  else {
    fstream file( fileName.c_str(), ios::in );
    ok = bool(file);
  }
  return ok;
}

//================================================================================
/*!
 * \brief _CaseFileDriver_User destructor
 */
//================================================================================

_CaseFileDriver_User::~_CaseFileDriver_User()
{
  if ( _imed )
    _imed->_nbUsers--;
  unregister( this );
}

//================================================================================
/*!
 * \brief Search substring in a string
 */
//================================================================================

bool contains( const char* what, const char* inString )
{
  size_t whatLen = strlen( what );
  size_t inLen   = strlen( inString );
  return 
    ( search( inString, inString+inLen, what, what + whatLen) != inString+inLen );
}

//================================================================================
/*!
 * \brief store subPart data
 */
//================================================================================

void _InterMed::addSubPart(const _SubPart& theSubPart)
{
  if ( _needSubParts ) {
    _SubPart & subPart = _subPartDescribed[ theSubPart.getDescriptor() ];
    subPart = theSubPart;
    if ( subPart.myCellGroupIndex > 0 ) {
      _groupe & groupe = this->groupes[ subPart.myCellGroupIndex-1 ];
      subPart.myFirstCell = groupe.mailles.begin();
    }
  }
}
//================================================================================
/*!
 * \brief delete intermediate med data
 */
//================================================================================

_InterMed::~_InterMed()
{
  if ( _isOwnMedMesh )
  {
    // remove MEDMEM groups not belonging to _medMesh
    for (unsigned int i=0; i < _intermediateMED::groupes.size(); ++i)
    {
      _groupe& grp = _intermediateMED::groupes[i];
      if ( !grp.medGroup ) continue;
      vector<GROUP*> groups = _medMesh->getGroups( grp.medGroup->getEntity() );
      if ( find( groups.begin(), groups.end(), grp.medGroup ) == groups.end() )
        grp.medGroup->removeReference();
    }
    if(_medMesh) _medMesh->removeReference();
    _medMesh=0;
  }
}
//================================================================================
/*!
 * \brief For a node, find its index in the supporting GROUP 
 */
//================================================================================

int _Support::getIndex( const pair<const int,_noeud>& inode)
{
  if ( myNodeGroup->relocMap.empty() ) // on all and not self intersecting support
    return abs( inode.second.number );

  map<unsigned,int>::iterator ordreIndex = myNodeGroup->relocMap.find( abs( inode.second.number ));
  if ( ordreIndex == myNodeGroup->relocMap.end() )
    throw MEDEXCEPTION(LOCALIZED(STRING("No index found for ") << inode.second));
//   map < int, int >::iterator numIndex = myNodeRelocMap.find( node->number );
//   if ( numIndex == myNodeRelocMap.end() )
//     throw MEDEXCEPTION(STRING("No index found for node ") << node->number);

  return ordreIndex->second;
}

//================================================================================
/*!
 * \brief For a cell, return its index in the supporting GROUP 
 */
//================================================================================

int _Support::getIndex( const _groupe::TMaille& cell)
{
  if ( myCellGroup->relocMap.empty() ) // on all and not self intersecting support
    return cell->ordre();

  map<unsigned,int>::iterator ordreIndex = myCellGroup->relocMap.find( cell->ordre() );
  if ( ordreIndex == myCellGroup->relocMap.end() )
    throw MEDEXCEPTION(LOCALIZED(STRING("No index found for ") << *cell));

  return ordreIndex->second;
}

//================================================================================
/*!
 * \brief Return med geom type for a subPart element
 */
//================================================================================

// medGeometryElement _Support::getType( const pair<const int,_noeud>& node)
// {
//   return myNodeGroup->medGroup.getTypes[0];
// }

// //================================================================================
// /*!
//  * \brief Return med geom type for a subPart element
//  */
// //================================================================================

// medGeometryElement _Support::getType( const _groupe::TMaille& cell)
// {
//   return cell->geometricType;
// }

//================================================================================
/*!
 * \brief Set med support
 */
//================================================================================

void _Support::setGroup( _groupe* group )
{
  if ( group->medGroup ) {
    if ( group->medGroup->getEntity() == MED_EN::MED_NODE )
      myNodeGroup = group;
    else
      myCellGroup = group;
  }
  else {
    throw MEDEXCEPTION(LOCALIZED("_Support::setGroup(): med GROUP is NULL"));
  }
}

//================================================================================
/*!
 * \brief Return med group correspoding to entity
 */
//================================================================================

SUPPORT* _Support::medSupport( medEntityMesh entity )
{
  if ( entity == MED_EN::MED_NODE )
    return myNodeGroup ? myNodeGroup->medGroup : 0;
  else
    return myCellGroup ? myCellGroup->medGroup : 0;
}

//================================================================================
/*!
 * \brief print _SubPartDesc
 */
//================================================================================

std::ostream& operator << (std::ostream& os, const _SubPartDesc& desc)
{
  if ( desc == _SubPartDesc::globalCoordDesc() )
    os << "'global coordinates'";
  else
    os << "<'part " << desc.partNumber() << "', '" << desc.typeName() << "'>";
  return os;
}

//================================================================================
/*!
 * \brief Constructor of ASCIIFileReader
 */
//================================================================================

_ASCIIFileReader::_ASCIIFileReader(const string& fileName) throw (MEDEXCEPTION)
{
#ifdef WNT
  _file = ::_open (fileName.c_str(), _O_RDONLY|_O_BINARY);
#else
  _file = ::open (fileName.c_str(), O_RDONLY);
#endif
  if (_file >= 0)
  {
    _start = new char [BUFFER_SIZE];
    _ptr   = _start;
    _eptr  = _start;
  }
  else
  {
    throw MEDEXCEPTION(STRING("Can't read from ")<<fileName);
  }
  if ( eof() )
    throw MEDEXCEPTION(STRING("Empty file ")<<fileName);

  // there must be end-of-line in ASCII file
  char* ptr = _start + MAX_LINE_LENGTH;
  bool isASCII = false;
  while ( !isASCII && ptr >= _start )
    isASCII = (*ptr-- == '\n');
  _isWin = ( *ptr == '\r');
  if ( !isASCII )
    throw MEDEXCEPTION(STRING("Not ASCII file ")<<fileName);
}

//================================================================================
/*!
 * \brief Return true if the whole file has been read
 */
//================================================================================

bool _ASCIIFileReader::eof()
{
  // Check the state of the buffer;
  // if there is too little left, read the next portion of data
  int nBytesRest = _eptr - _ptr;
  if (nBytesRest < 2 * MAX_LINE_LENGTH)
  {
    if (nBytesRest > 0) {
      char* tmpBuf = new char [nBytesRest];
      memcpy (tmpBuf, _ptr, nBytesRest);
      memcpy (_start, tmpBuf, nBytesRest);
      delete [] tmpBuf;
    } else {
      nBytesRest = 0;
    }
    _ptr = _start;
    const int nBytesRead = ::read (_file,
                                   &_start [nBytesRest],
                                   BUFFER_SIZE - nBytesRest);
    nBytesRest += nBytesRead;
    _eptr = &_start [nBytesRest];

    // skip spaces at file end
    if ( nBytesRest < MAX_LINE_LENGTH ) {
      while ( isspace( *_ptr )) _ptr++;
      nBytesRest = _eptr - _ptr;
    }
  }
  return nBytesRest < 1;
}

//================================================================================
/*!
 * \brief read out given data
 */
//================================================================================

void _ASCIIFileReader::skip(int nbVals, int nbPerLine, int valWidth)
{
  int nbLines = (nbVals + nbPerLine - 1) / nbPerLine;
  int width = nbVals * valWidth;
  skip( width, nbLines );
}

//================================================================================
/*!
 * \brief read out width chars and nbLines line-ends
 */
//================================================================================

void _ASCIIFileReader::skip(int width, int nbLines)
{
  width += nbLines; // to skip '\n'
  if ( _isWin )
    width += nbLines; // to skip '\r'

  _ptr += width;
  int nBytesRest = _eptr - _ptr;
  while ( nBytesRest < 0 ) {
    width = -nBytesRest;
    if ( eof() ) return;
    _ptr += width;
    nBytesRest = _eptr - _ptr;
  }
}

//================================================================================
/*!
 * \brief Read a next line from the file
 */
//================================================================================

char* _ASCIIFileReader::getLine() throw (MEDEXCEPTION)
{
  if ( eof() )
    throw MEDEXCEPTION("Unexpected EOF");

  // Check the buffer for the end-of-line
  char * ptr = _ptr;
  while (true)
  {
    // Check for end-of-the-buffer, the ultimate criterion for termination
    if (ptr >= _eptr)
    {
      //_eptr[-1] = '\0';
      _eptr[0] = '\0';
      break;
    }
    // seek the line-feed character
    if (ptr[0] == '\n')
    {
      if ( ptr > _start && // avoid "Invalid read" error by valgrind
           ptr[-1] == '\r')
        ptr[-1] = '\0';
      ptr[0] = '\0';
      ++ptr;
      break;
    }
    ++ptr;
  }
  // Output the result
  char * line = _ptr;
  _ptr = ptr;

  return line;
}

//================================================================================
/*!
 * \brief 
 */
//================================================================================

bool _ASCIIFileReader::lookAt( const char* text )
{
  while ( isspace(*_ptr)) ++_ptr;
  return ( strncmp( _ptr, text, strlen( text )) == 0 );
}

//================================================================================
/*!
 * \brief Read a next word from the file
 */
//================================================================================

string _ASCIIFileReader::getWord()
{
  if ( eof() )
    return "";

  // skip spaces
  while ( isspace(*_ptr)) ++_ptr;
  if ( _ptr >= _eptr )
    return "";

  // skip not spaces
  char* word = _ptr++;
  while ( !isspace(*_ptr)) ++_ptr;

  return string( word, _ptr - word );
}

//================================================================================
/*!
 * \brief Return true if TIME_STEP_BEG follows and skips TIME_STEP_BEG line
 */
//================================================================================

bool _ASCIIFileReader::isTimeStepBeginning()
{
  if ( eof() ) throw MEDEXCEPTION(LOCALIZED("Unexpected EOF"));

  while ( isspace(*_ptr)) ++_ptr;

  if ( strncmp( _ptr, TIME_STEP_BEG, TIME_STEP_BEG_LEN ) != 0 )
    return false;

  _ptr += TIME_STEP_BEG_LEN;
  while ( isspace(*_ptr)) ++_ptr;
  return true;
}

//================================================================================
/*!
 * \brief Return true if TIME_STEP_END follows and skips TIME_STEP_END line
 */
//================================================================================

bool _ASCIIFileReader::isTimeStepEnd()
{
  if ( eof() ) return true;

  while ( isspace(*_ptr)) ++_ptr;

  if ( strncmp( _ptr, TIME_STEP_END, TIME_STEP_END_LEN ) != 0 )
    return false;

  _ptr += TIME_STEP_END_LEN;
  while ( isspace(*_ptr)) ++_ptr;
  return true;
}


//================================================================================
/*!
 * \brief divide a string into two parts
 */
//================================================================================

int _ASCIIFileReader::split(const string& str,
                            string &      part1,
                            string &      part2,
                            const char    separator,
                            const bool    fromBack)
{
  int nbParts = 0;
  string parts[2];
  const char* ptr1 = str.c_str();
  const char* back = ptr1 + str.size();
  for (nbParts = 0; nbParts < 2; ++nbParts ) {
    // skip spaces before the current part
    while ( isspace(*ptr1)) ++ptr1;
    if ( !*ptr1) break;
    // find end of the part and beginning of the next part
    const char* ptr2 = ptr1;
    const char* nextBeg = back;
    if ( nbParts > 0 ) {
      ptr2 = back;
    }
    else if ( fromBack ) {
      ptr2 = back;
      string::size_type pos = str.rfind( separator );
      if ( pos != str.npos ) {
        ptr2 = & str[ pos ];
        nextBeg = ptr2 + 1;
        if ( separator != ' ')
          while ( *nextBeg && *nextBeg == separator ) ++nextBeg;
      }
    }
    else if ( separator == ' ' ) {
      while ( *ptr2 && !isspace(*ptr2)) ++ptr2;
      if ( *ptr2 ) nextBeg = ptr2 + 1;
    }
    else {
      while ( *ptr2 && *ptr2 != separator ) ++ptr2;
      if ( *ptr2 ) {
        nextBeg = ptr2 + 1;
        while ( *nextBeg && *nextBeg == separator ) ++nextBeg;
      }
    }
    //if ( !*ptr2) --ptr2;
    // skip spaces after the current part
    while ( ptr2 > ptr1 && isspace(ptr2[-1])) --ptr2;
    parts[ nbParts ] = string( ptr1, ptr2-ptr1 );
    ptr1 = nextBeg;
  }
  part1 = parts[0];
  part2 = parts[1];
  return nbParts;
}

//================================================================================
/*!
 * \brief divide a string into parts, return nb of parts
 */
//================================================================================

int _ASCIIFileReader::split(const string&       str,
                            list<string> &      parts,
                            const char          separator,
                            const bool          fromBack)
{
  parts.clear();
  if ( str.empty() )
    return 0;
  int nbParts = 0;
  const char* ptr1 = str.c_str();
  const char* back = ptr1 + str.size();
  if ( fromBack ) {
    swap( ptr1, back );
    while (1) {
      // skip spaces after the current part
      while ( isspace(ptr1[-1])) --ptr1;
      if ( ptr1 <= back ) break;
      // find beginning of the part
      const char* ptr2 = ptr1 - 1;
      if ( separator == ' ' )
        while ( ptr2 > back && !isspace(ptr2[-1])) --ptr2;
      else
        while ( ptr2 > back && ptr2[-1] != separator ) --ptr2;
      //if ( !*ptr2) --ptr2;
      const char* sepPtr = ptr2;
      // skip spaces before the current part
      while ( isspace(*ptr2)) ++ptr2;
      parts.push_back( string( ptr2, ptr1-ptr2 ));
      ++nbParts;
      ptr1 = sepPtr - 1;
    }
  }
  else {
    while (1) {
      // skip spaces before the current part
      while ( isspace(*ptr1)) ++ptr1;
      if ( ptr1 >= back) break;
      // find end of the part
      const char* ptr2 = ptr1 + 1;
      if ( separator == ' ' )
        while ( *ptr2 && !isspace(*ptr2)) ++ptr2;
      else
        while ( *ptr2 && *ptr2 != separator ) ++ptr2;
      //if ( !*ptr2) --ptr2;
      const char* sepPtr = ptr2;
      // skip spaces after the current part
      while ( ptr2 > ptr1 && isspace(ptr2[-1])) --ptr2;
      parts.push_back( string( ptr1, ptr2-ptr1 ));
      ++nbParts;
      ptr1 = sepPtr + int( sepPtr < back );
    }
  }
  return nbParts;
}

//================================================================================
/*!
 * \brief check if a string contains only digits
 */
//================================================================================

bool _ASCIIFileReader::isDigit(const string& str, const bool real)
{
  const char* s = str.c_str();
  if ( real ) {
    while ( *s ) {
      char c = *s++;
      if ( !isdigit(c) && c!='-' && c!='+' && c!='.' && c!=',' && c!='E' && c!='e')
        return false;
    }
  }
  else {
    while ( *s ) {
      if ( !isdigit( *s++ ))
        return false;
    }
  }
  return true;
}

//================================================================================
/*!
 * \brief Destructor of ASCIIFileReader closes its file
 */
//================================================================================

_ASCIIFileReader::~_ASCIIFileReader()
{
  if (_file >= 0)
  {
    ::close (_file);
    delete [] _start;
  }
}

//================================================================================
/*!
 * \brief Constructor of _BinaryFileReader opens its file
 */
//================================================================================

_BinaryFileReader::_BinaryFileReader(const string& fileName) throw (MEDEXCEPTION)
  : _exception(STRING("Unexpected EOF ") << fileName), _mySwapBytes(false)
{
#ifdef WNT
  _file = ::_open (fileName.c_str(), _O_RDONLY|_O_BINARY);
#else
  _file = ::open (fileName.c_str(), O_RDONLY);
#endif

  if (_file < 0)
    throw MEDEXCEPTION(STRING("Can't read from ") << fileName);

  _maxPos = ::lseek( _file, 0, SEEK_END);
  _pos    = ::lseek( _file, 0, SEEK_SET);
}

//================================================================================
/*!
 * \brief Destructor of _BinaryFileWriter closes its file
 */
//================================================================================

_BinaryFileReader::~_BinaryFileReader()
{
  if (_file >= 0)
    ::close (_file);
}

//================================================================================
/*!
 * \brief rewind the file backward
 */
//================================================================================

void _BinaryFileReader::rewind()
{
  _pos = ::lseek( _file, 0, SEEK_SET);
}

//================================================================================
/*!
 * \brief size of not read file portion in sizeof(int)
 */
//================================================================================

int _BinaryFileReader::moreValuesAvailable() const
{
  return (_maxPos - _pos) / sizeof(int); // believe that sizeof(int) == sizeof(float)
}

//================================================================================
/*!
 * \brief Return true if the whole file has been read
 */
//================================================================================

bool _BinaryFileReader::eof()
{
  return _pos >= _maxPos;
}

//================================================================================
/*!
 * \brief Skips given nb of bytes
 */
//================================================================================

void _BinaryFileReader::skip(int size) throw (MEDEXCEPTION)
{
  if ( _pos + size > _maxPos )
    throw _exception;
  off_t newPos = ::lseek( _file, size, SEEK_CUR);
  if ( newPos < _pos + size )
    throw _exception;
  _pos = newPos;
}

//================================================================================
/*!
 * \brief Read lines until TIME_STEP_BEG encounters
 */
//================================================================================

void _BinaryFileReader::skipTimeStepBeginning() throw (MEDEXCEPTION)
{
  bool tsbReached = false;
  while ( !tsbReached ) {
    TStrOwner line( getLine() );
    tsbReached = ( strncmp( line, TIME_STEP_BEG, TIME_STEP_BEG_LEN ) == 0 );
  }
}

//================================================================================
/*!
 * \brief Constructor of _BinaryFileWriter opens its file
 */
//================================================================================

_BinaryFileWriter::_BinaryFileWriter(const string& fileName)  throw (MEDEXCEPTION)
  : _exception(STRING("Can't write into ") << fileName)
{
#ifdef WNT
  _file = ::_open (fileName.c_str(), _O_WRONLY|_O_BINARY|_O_TRUNC);
#else
  _file = ::open (fileName.c_str(), O_WRONLY|O_TRUNC); //length shall be truncated to 0
#endif

  if (_file < 0)
    throw _exception;
}

//================================================================================
/*!
 * \brief Destructor of _BinaryFileWriter closes its file
 */
//================================================================================

_BinaryFileWriter::~_BinaryFileWriter()
{
  if (_file >= 0)
    ::close (_file);
}

//================================================================================
/*!
 * \brief Write string
 */
//================================================================================

void _BinaryFileWriter::addString(const char* str) throw (MEDEXCEPTION)
{
  size_t len = strlen( str );
  if ((int) len > MAX_LINE_LENGTH )
    throw MEDEXCEPTION
      (LOCALIZED(STRING("_BinaryFileWriter::addString(), too long string (>80):\n") << str));

  string buffer( str, len );
  // avoid "Syscall param write(buf) points to uninitialised byte(s)" error by valgrind
  buffer += string(MAX_LINE_LENGTH, ' ');
  buffer[ len ] = '\0';
  buffer[ MAX_LINE_LENGTH-1 ] = '\0'; // to have line-end within

  add( buffer.c_str(), MAX_LINE_LENGTH );
}



} // end namespace MEDMEM_ENSIGHT
