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

// File      : MEDMEM_EnsightUtils.hxx
// Created   : Tue May 27 12:24:11 2008
// Author    : Edward AGAPOV (eap)
//
#ifndef MEDMEM_EnsightUtils_HeaderFile
#define MEDMEM_EnsightUtils_HeaderFile

#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_DriverTools.hxx"
#include "MEDMEM_Mesh.hxx"
//#include "MEDMEM_Field.hxx"

#include <set>
#include <cfloat>
#include <cmath>
#include <unistd.h>

#ifdef WNT
#define isnan _isnan
#endif

namespace MEDMEM {

  // ==============================================================================
  /*!
   * \brief Functions to set writing format.
   * Default format is EnSight6 ASCII
   */
  // ==============================================================================

  enum EnSightFormat { ENSIGHT_6, ENSIGHT_GOLD };

  void MEDMEM_EXPORT setEnSightFormatForWriting (EnSightFormat format, bool isBinary);

  EnSightFormat getEnSightFormatForWriting();
  bool          isBinaryEnSightFormatForWriting();

  // ==============================================================================
  /*!
   * \brief To raise or not if MEDMEM-EnSight incompatibility encounters or suspected
   *
   * Default is to raise.
   * To ignore incompatibility is useful for debug of EnSigt reading and
   * for suppressing exceptions in some suspicious cases, for example writing
   * several meshes and fields.
   * Incompatibility exception includes "EnSight-MEDMEM compatibility problem" string.
   * Not all incompatibility exceptions can be suppressed.
   */
  // ==============================================================================

  void MEDMEM_EXPORT setIgnoreIncompatibility(bool toIgnore=true);
}

namespace MEDMEM {
  class ENSIGHT_MESH_RDONLY_DRIVER;
  class ENSIGHT_FIELD_RDONLY_DRIVER;
  class ENSIGHT_MESH_WRONLY_DRIVER;
  class ENSIGHT_FIELD_WRONLY_DRIVER;
}

// ==============================================================================
namespace MEDMEM_ENSIGHT { // INTERNAL MATTERS
// ==============================================================================

  class _CaseFileDriver;
  class _CaseFileDriver_User;
  struct _InterMed;
  struct _SubPart;
  struct _SubPartDesc;
  struct _Support;
  typedef std::set< _SubPartDesc > _SupportDesc;

  using namespace MED_EN;
  using namespace MEDMEM;
  using namespace std;

  // ---------------------------------------------------------------
  /*!
   * \brief Prepend "EnSight-MEDMEM compatibility problem" to the text
   *        of an exception
   */
  STRING compatibilityPb(const string& exceptionText);

  // ---------------------------------------------------------------
  /*!
   * \brief To ignore incompatibility or not
   */
  bool   toIgnoreIncompatibility();

  // ---------------------------------------------------------------
  //!< search substring in a string
  bool contains( const char* what, const char* inString );

  // ---------------------------------------------------------------
  /*!
   * \brief EnSight element type name and an array to convert
   *        med connectivity to EnSight one
   */
  struct TEnSightElemType
  {
    string             _name;
    vector<int>        _medIndex;
    medGeometryElement _medType;
  };
  // ---------------------------------------------------------------
  /*!
   * \brief Return EnSight type corresponding to med one
   */
  const TEnSightElemType& getEnSightType(medGeometryElement medType);

  // ---------------------------------------------------------------
  /*!
   * \brief Return EnSight type having a given name
   */
  const TEnSightElemType& getEnSightType(const string& typeName);

  // ---------------------------------------------------------------
  /*!
   * \brief Return true if typeName begins with "g_"
   */
  inline bool isGhostType(const string& typeName)
  { return ( typeName[0] == 'g' && typeName[1] == '_'); }

  // ---------------------------------------------------------------
  /*!
   * \brief Maximal EnSight line length
   */
  const int MAX_LINE_LENGTH = 80;

  // ---------------------------------------------------------------
  /*!
   * \brief Maximal length of field name in EnSight 
   */
  const int MAX_FIELD_NAME_LENGTH = 19;

  // ---------------------------------------------------------------
  /*!
   * \brief Maximal length of field name in EnSight 
   */
  const string ILLEGAL_FIELD_NAME_CHARACTERS = " !@#$^()[]*/+-"; // '.,' ????

  // ---------------------------------------------------------------
  /*!
   * \brief Width of fields in ASCII file
   */
  const int INT_WIDTH_6 = 8;
  const int INT_WIDTH_GOLD = 10;
  const int FLT_WIDTH = 12;

  // ---------------------------------------------------------------
  /*!
   * \brief EnSight space dimension
   */
  const int SPACE_DIM = 3;

  // ---------------------------------------------------------------
  /*!
   * \brief Time step data boundaries in single-file mode
   */
  static const char* TIME_STEP_BEG = "BEGIN TIME STEP";
  static const char* TIME_STEP_END = "END TIME STEP";
  const size_t TIME_STEP_BEG_LEN = 15;
  const size_t TIME_STEP_END_LEN = 13;
  

  // ---------------------------------------------------------------
  /*!
   * \brief Cast value to float and protect from overflow
   */
  static inline float _toFloat (const double & value) {
    if ( value >  FLT_MAX ) return  FLT_MAX;
    if ( value < -FLT_MAX ) return -FLT_MAX;
    if ( std::isnan( value ))    throw MEDEXCEPTION(compatibilityPb("NaN value not allowed"));
    return float( value );
  }
  static inline float _toFloat (const int & value) { return float( value ); }
  static inline float _toFloat (const long & value) { return float( value ); }

// ==============================================================================
/*!
 * \brief Reader/writer of EnSight Case file
 *
 * Apart from it's major job, it
 * o assures cooperation of MED and Mesh/Field drivers so that the Case file created
 *   by MED driver is not overwritten by Mesh driver called by MED driver.
 */
// ==============================================================================

class _CaseFileDriver
{
public:
  _CaseFileDriver(const string& fileName, const _CaseFileDriver_User* creator);
  ~_CaseFileDriver();

  void read() throw (MEDEXCEPTION);

  // ---------------
  // reading meshes
  // ---------------

  int  getNbMeshes() const;

  //!< sets all data necessary for meshDriver::read()
  void setDataFileName(const int meshIndex, ENSIGHT_MESH_RDONLY_DRIVER* meshDriver);

  // ---------------
  // reading fields
  // ---------------

  int  getNbVariables() const;

  int  getNbVarSteps(const int variableIndex);

  //!< return variable index by variable name, return 0 if none found
  int  getVariableIndex(const string & varName) const;

  //!< sets all data necessary for fieldDriver::read(), returns meshIndex
  int setDataFileName(const int                    varIndex,
                      const int                    stepIndex,
                      ENSIGHT_FIELD_RDONLY_DRIVER* fieldDriver);

  // --------
  // writing
  // --------

  //!< add a mesh to the Case file
  void addMesh(const ENSIGHT_MESH_WRONLY_DRIVER* meshDriver);

  //!< add a field to the Case file
  void addField(const ENSIGHT_FIELD_WRONLY_DRIVER * fieldDriver);

  //!< write and set necessary data to added drivers
  void write() throw (MEDEXCEPTION);

private:

  //!< throw if Case file has not been read else return false
  bool checkWasRead() const throw (MEDEXCEPTION);

  //!< replace '*' in file name if any and return index in a file and time
  int  fixWildCardName(const int           timeStep,
                       const std::string & ts,
                       const std::string & fs,
                       std::string &       fileName,
                       std::string &       time);

  // --------------------------------------------------------------------------------
  // GEOMETRY section
  // model: [ts] [fs] filename [change_coords_only]
  //        ts = time set number as specified in TIME section. This is optional.
  //        fs = corresponding file set number as specified in FILE section below.
  //            (Note, if you specify fs, then ts is no longer optional and must also be
  //            specified.)
  // filename = The filename of the appropriate file.
  //            -> Model or measured filenames for a static geometry case, as well as match,
  //                   boundary, and rigid_body filenames will not contain "*" wildcards.
  //            -> Model or measured filenames for a changing geometry case will
  //                  contain "*" wildcards.
  // change_coords_only =         The option to indicate that the changing geometry (as
  //                              indicated by wildcards in the filename) is coords only.
  //                              Otherwise, changing geometry connectivity will be
  //                              assumed.
  struct _Model {
    string _timeSetNumber, _fileSetNumber; //!< ts, fs
    string _fileName;                      //!< filename
    string _change_coords_only;
  };
  // --------------------------------------------------------------------------------
  // VARIABLE section
  // ts             = The corresponding time set number (or index) as specified in TIME
  //                  section below. This is only required for transient constants and
  //                  variables.
  // fs             = The corresponding file set number (or index) as specified in FILE
  //                  section below.
  //                   (Note, if you specify fs, then ts is no longer optional and must
  //                    also be specified.)
  // description    = The variable (GUI) name (ex. Pressure, Velocity, etc.)
  // const_value(s) = The constant value. If constants change over time, then ns (see
  //                  TIME section below) constant values of ts.
  // cvfilename     = The filename containing the constant values, one value per time step.
  // filename       = The filename of the variable file. Note: only transient filenames
  //                     contain "*" wildcards.
  // Re_fn          = The filename for the file containing the real values of the complex
  //                  variable.
  // Im_fn          = The filename for the file containing the imaginary values of the
  //                  complex variable.
  // freq           = The corresponding harmonic frequency of the complex variable.
  //                  For complex variables where harmonic frequency is undefined,
  //                  simply use the text string: UNDEFINED.
  struct _Variable {
    string _type;                          //!< constant|scalar|etc.
    string _name;                          //!< description
    string _timeSetNumber, _fileSetNumber; //!< [ts], [fs]
    string _fileNameOrData;                //!< [cv]filename|Re_fn Im_fn freq|const_value(s)
  };
  // --------------------------------------------------------------------------------
  // FILE section
  // fs = file set number. This is the number referenced in the GEOMETRY
  //      and VARIABLE sections above.
  // ns = number of transient steps
  // fi = file index number in the file name (replaces "*" in the filenames)
  struct _FileSet {
    int                    _number;        //!< fs
    std::list<int>         _nbStepsInFile; //!< ns
    std::list<std::string> _fileIndex;     //!< fi
  };
  // --------------------------------------------------------------------------------
  // TIME section
  struct _TimeSet {
    int                      _number;    //!< ts
    std::vector<std::string> _fileIndex; //!< fn
    std::vector<std::string> _times;     //!< times

    bool operator==(const _TimeSet& ts) const
    { return ( _fileIndex == ts._fileIndex && _times == ts._times ); }
  };

private:

  std::string               _fileName;
  std::string               _directory;
  EnSightFormat             _format;
  _Model                    _model;
  std::map< int, _Variable> _variables; //!< map order number to variable data
  std::map< int, _TimeSet > _timeSets;  //!< map ts to time set data
  std::map< int, _FileSet > _fileSets;  //!< map fs to file set data

  const _CaseFileDriver_User* _user; //!< mesh/field driver

  std::list<ENSIGHT_MESH_WRONLY_DRIVER*> _meshDrivers; //!< added meshes

  typedef std::map<std::string, std::list< ENSIGHT_FIELD_WRONLY_DRIVER* > > TFieldDriversByName;
  TFieldDriversByName _fieldDrivers; //!< added field drivers groupped by name

  //!< to block all calls from a mesh/field driver governed by med driver
  bool                      _blocked;
};

  // ==============================================================================
/*!
 * \brief Base of all Ensight drivers.
 * It stores data passed from case file
 */
// ==============================================================================

class MEDMEM_EXPORT _CaseFileDriver_User: public GENDRIVER
{
protected:

  _CaseFileDriver_User(const std::string&     caseFileName="",
                       MED_EN::med_mode_acces mode=MED_EN::RDWR);

  const std::string& getCaseFileName() const { return GENDRIVER::_fileName; }

  const std::string& getDataFileName() const { return _dataFileName; }

  bool isGoldFormat() const { return _isGoldFormat; }

  //!< returns true if there are several meshes/fields in a data file
  bool isSingleFileMode() const { return _singleFileMode; }

  //!< index of meshes/fields in a data file, zero for static mesh/feild
  int  getIndexInDataFile() const { return _indexInDataFile; }

  //!< true if there are time steps, i.e. getTime() has sense
  bool isTransientMode() const { return _transientMode; }

  //!< time of a step
  double getTime() const { return atof(_time.c_str()); }

  // -------------------------------
  // pass mesh data to field driver
  // -------------------------------

  //!< for mesh driver to store data
  void setInterData(_InterMed* imed);

  //!< for field driver to get mesh data
  _InterMed* getInterData();

  _SubPart* getSubPart(const _SubPartDesc & descriptor) throw (MEDEXCEPTION);

  _Support* getSupport(const _SupportDesc & descriptor,
                       const medEntityMesh  entity)  throw (MEDEXCEPTION);


public:
  //!< return part number to write support with, zero in failure case
  int getPartNumber(const SUPPORT* support) const;

  static bool canOpenFile(const string& fileName, MED_EN::med_mode_acces mode);

  static void getSupportNodes(const SUPPORT* sup, map<int, int> & nodeIds);

  //!< analyse if data file is binary
  static bool isBinaryDataFile(const string& dataFileName);

  static bool isTimeStepBeginning(const string& line)
  { return ( line == TIME_STEP_BEG ); }

  static bool isTimeStepEnd(const char* line)
  { return ( strncmp( line, TIME_STEP_END, TIME_STEP_END_LEN ) == 0 ); }

  static bool isToWriteEntity(const medEntityMesh entity, const GMESH* mesh);

  ~_CaseFileDriver_User();

  void merge( const GENDRIVER& driver);

private:

  friend class _CaseFileDriver;

  // members set by _CaseFileDriver::setDataFileName(...) and _CaseFileDriver::write()
  std::string _dataFileName;
  bool        _isGoldFormat;
  bool        _transientMode;   //!< true if there are time steps
  bool        _singleFileMode;  //!< only one or several meshes/fields in a data file 
  int         _indexInDataFile; //!< which meshes/fields in a data file
  std::string _time;            //!< time of a step

  _InterMed* _imed;       //!< to be used by field driver
  string     _imedMapKey; //!< key in the map storing mesh data for usage by field drv

};

// ==============================================================================
/*!
 * \brief Descriptor of the sub-part: part number and elem type or "block" etc.
 */
// ==============================================================================

struct _SubPartDesc: public std::pair<int, std::string >
{
  _SubPartDesc(int                partNumber=-1,
               const std::string& typeName="")
    : std::pair<int, std::string > ( partNumber, typeName ) {}

  const int&    partNumber() const { return this->first; }
  const string& typeName()   const { return this->second; }

  static _SubPartDesc globalCoordDesc() { return _SubPartDesc(-1,"coordinates"); }
};

std::ostream& operator << (std::ostream& os, const _SubPartDesc& desc);

// ==============================================================================
/*!
 * \brief A type within EnSight part. It stores data needed by field driver to
 * know nb of values of a geometric type and what place they get in MED group
 */
// ==============================================================================

struct _SubPart: public _SubPartDesc
{
  // _SubPart describes both nodes and elements since "block" describes the both.
  // Mesh driver sets
  //   for cells: myNbCells and myCellGroupIndex
  //   for nodes: myNbNodes and myFirstNode
  // GROUP of cells is created always,
  // GROUP of nodes, only if nodal field support is required (see getSupport())

  int                  myNbCells;        //!< nb of cells
  int                  myCellGroupIndex; //!< cell group id in _InterMed
  _groupe::TMailleIter myFirstCell;      //!< pointer to the first cell

  int                  myNbNodes;        //!< nb of nodes
  mutable int          myNodeGroupIndex; //!< node group id in _InterMed
  _maille::TNoeud      myFirstNode;      //!< pointer to the first node

  _SubPart(int                partNumber=-1,
           const std::string& typeName="")
    : _SubPartDesc(partNumber,typeName),
      myNbCells(0), myCellGroupIndex(-1),
      myNbNodes(0), myNodeGroupIndex(-1)
  {}

  _SubPartDesc getDescriptor() const { return _SubPartDesc( partNumber(), typeName() ); }
};

// ==============================================================================
/*!
 * \brief EnSight variable support composed of _SubPart's 
 */
// ==============================================================================

struct _Support
{
  _groupe * myCellGroup; //!< cell group in _InterMed
  _groupe * myNodeGroup; //!< node group in _InterMed

  _Support(): myCellGroup(0), myNodeGroup(0) {}

  void     setGroup( _groupe* g );
  SUPPORT* medSupport( medEntityMesh entity );

  int getIndex( const pair<const int,_noeud>& node);
  int getIndex( const _groupe::TMaille&       cell);

//   medGeometryElement getType( const pair<const int,_noeud>& node);
//   medGeometryElement getType( const _groupe::TMaille&       cell);
};

// ==============================================================================
/*!
 * \brief Structure to temporarily store data read from EnSight geom file
 */
// ==============================================================================

struct _InterMed : public _intermediateMED
{
  MESH* _medMesh;
  bool  _isOwnMedMesh; //!< whether to delete _medMesh
  int   _nbUsers;      //!< to know when to delete _medMesh

  bool  _needSubParts; //!< true if there are fields needing _SubPart data

  map< _SubPartDesc, _SubPart > _subPartDescribed;

  map< _SupportDesc, _Support > _supportDescribed;

  void addSubPart(const _SubPart& subPart);

  ~_InterMed();
};

// ==============================================================================
/*!
 * \brief Simple owner of C array
 */
// ==============================================================================

template <typename T> struct _ValueOwner {
  T * myValues;
  _ValueOwner(T* values):myValues(values) {}
  ~_ValueOwner() { if ( myValues ) delete [] myValues; }
  operator T*() { return myValues; }
private:
  _ValueOwner(const _ValueOwner& other) {} // forbidden
};
// instantiations
typedef _ValueOwner<char>   TStrOwner;
typedef _ValueOwner<int>    TIntOwner;
typedef _ValueOwner<double> TDblOwner;
typedef _ValueOwner<float>  TFltOwner;

// ==============================================================================
/*!
 * \brief Iterator on values of a component
 */
// ==============================================================================

template <typename T> class _ValueIterator
{
protected:
  const T* myPtr;
  int      myDelta;
public:
  _ValueIterator() // by default next() returns zero
    : myPtr(zeroPtr()), myDelta( 0 ) {}

  _ValueIterator(const T* values, int delta): myPtr(values-delta), myDelta(delta) {}

  const T & next() { myPtr += myDelta; return *myPtr; }

  static const T* zeroPtr() { static T a0 = 0; return &a0; }
};

// ==============================================================================
/*!
 * \brief Reader of ASCII files
 */
// ==============================================================================

class _ASCIIFileReader
{
public:
  _ASCIIFileReader(const string& fileName) throw (MEDEXCEPTION);

  ~_ASCIIFileReader();

  bool eof();

  string getWord(); //!< never throws

  int    getInt()  throw (MEDEXCEPTION) {
    if ( eof() ) throw MEDEXCEPTION("Unexpected EOF");
    return strtol(_ptr, &_ptr, 10);
  }
  float  getReal() throw (MEDEXCEPTION) {
    if ( eof() ) throw MEDEXCEPTION("Unexpected EOF");
#ifdef WNT
#else
    return strtof(_ptr, &_ptr);
#endif
  }
  //!< needed after getWord(), getInt() or getReal() to get a next line
  void toNextLine() {
    while (isspace(*_ptr)) if ((++_ptr)[-1]=='\n') break;
  }
  char*  getLine() throw (MEDEXCEPTION);

  const char* getCurrentPtr() const { return _ptr; }

  bool lookAt( const char* text );

  bool isTimeStepBeginning();

  bool isTimeStepEnd();

  //!< read out given data
  void skip(int nbVals, int nbPerLine, int valWidth);

  //!< read out width chars and nbLines line-ends
  void skip(int width, int nbLines);

  template <class T>
  char* convertReals( const int          nbValues,
                      const char*        undefValue = 0,
                      set<int>*          undefIndices = 0,
                      const vector<int>* partialIndices = 0,
                      const int          nbPartialComponents = 0)
    throw (MEDEXCEPTION)
  {
    T* result = new T[ nbValues ];
    T* ptrT = result;
    if ( undefValue ) // fill undefIndices
    {
      undefIndices->clear();
      float undef = atof( undefValue );
      for ( int i = 0; i < nbValues; ++i, ++ptrT ) {
        float value = getReal();
        (*ptrT) = (T) value;
        if ( value == undef )
          undefIndices->insert( undefIndices->end(), i+1 );
      }
    }
    else if ( partialIndices )
    {
      // partial variables are available in GOLD format only where
      // values are in no-interlace
      int shift = 1;
      for ( int j = 1; j <= nbPartialComponents; ++j ) {
        vector<int>::const_iterator i = partialIndices->begin(), iEnd = partialIndices->end();
        while ( i != iEnd )
          result[ *i++ - shift ] = (T) getReal();
        shift += nbValues;
      }
    }
    else
    {
      for ( int i = 0; i < nbValues; ++i, ++ptrT )
        (*ptrT) = (T) getReal();
    }
    return (char*) result;
  }

  //static string strip(char* & str);

  //!< divide a string into two parts
  static int  split(const string& str,
                    string &      part1,
                    string &      part2,
                    const char    separator=' ',
                    const bool    fromBack=false);

  //!< divide a string into parts, return nb of parts
  static int  split(const string&       str,
                    std::list<string> & parts,
                    const char          separator=' ',
                    const bool          fromBack=false);

  //!< check if string contains only digits
  static bool isDigit(const string& str, const bool real=false);

private:

  int   _file;
  char* _start; // buffer start
  char* _ptr;   // beginning of not read portion
  char* _eptr;  // end of buffer contents

  bool  _isWin;

};// class _ASCIIFileReader


// ==============================================================================
/*!
 * \brief Reader of binary files
 */
// ==============================================================================

class _BinaryFileReader
{
public:
  _BinaryFileReader(const string& fileName) throw (MEDEXCEPTION);

  ~_BinaryFileReader();

  void rewind(); //!< rewind the file backward

  void swapBytes() //!< turn on swapping bytes
  { _mySwapBytes = true; }

  int moreValuesAvailable() const; //!< size of not read file portion in sizeof(int)

  bool eof();

  void skip(int size) throw (MEDEXCEPTION);

  void skipTimeStepBeginning() throw (MEDEXCEPTION);

  char*   getLine()      throw (MEDEXCEPTION)
  { return get<char>(80); }

  int*    getInt(int nb) throw (MEDEXCEPTION)
  { return get<int>(nb,_mySwapBytes); }

  float*  getFlt(int nb) throw (MEDEXCEPTION)
  { return get<float>(nb,_mySwapBytes); }

  ssize_t getPosition() const { return _pos; }

  template <class T>
  char* convertReals( const int          nbValues,
                      const char*        undefValue = 0,
                      set<int>*          undefIndices = 0,
                      const vector<int>* partialIndices = 0,
                      const int          nbPartialComponents = 0 )
    throw (MEDEXCEPTION)
  {
    T* result = new T[ nbValues ];
    T* ptrT = result, *endT = result + nbValues;
    int nb = partialIndices ? partialIndices->size() * nbPartialComponents : nbValues;
    TFltOwner fltData( getFlt( nb ));
    float* ptrFlt = fltData;
    if ( undefValue ) // fill undefIndices
    {
      undefIndices->clear();
      float undef = atof( undefValue );
      while ( ptrT < endT ) {
        float value = *ptrFlt++;
        *ptrT++ = (T) value;
        if ( std::abs( value - undef ) <= FLT_MIN )
          undefIndices->insert( undefIndices->end(), ptrT - result );
      }
    }
    else if ( partialIndices )
    {
      // partial variables are available in GOLD format only where
      // values are in no-interlace
      int shift = 1;
      for ( int j = 1; j <= nbPartialComponents; ++j ) {
        vector<int>::const_iterator i = partialIndices->begin(), iEnd = partialIndices->end();
        while ( i != iEnd )
          result[ *i++ - shift ] = (T) *ptrFlt++;
        shift += nbValues;
      }
    }
    else
    {
      while ( ptrT < endT )
        *ptrT++ = (T) *ptrFlt++;
    }
    return (char*) result;
  }
private:

  int          _file;         //!< descriptor
  MEDEXCEPTION _exception;    //!< ready to raise exception
  ssize_t      _pos, _maxPos; //!< current position and end position
  bool         _mySwapBytes;  //!< to swap bytes

  //!< read any data from file
  template <typename T> T* get(int nb, bool inverseBytes=false)
  {
    size_t bufSize = nb * sizeof( T );
    if ( int(bufSize) > _maxPos - _pos )
      throw _exception;
    T* buf = new T[ nb ];
#ifdef WNT
#else
    ssize_t nBytesRead = ::read (_file, buf, bufSize );
    _pos += nBytesRead;
    if ( int(nBytesRead) < int(bufSize) ) {
      delete buf;
      throw _exception;
    }
    if ( inverseBytes ) { // swap bytes
      int* intBuf = ((int*) buf) - 1;
      int* bufEnd = (int*)((char*) buf + nBytesRead);
      while ( ++intBuf < bufEnd )
        *intBuf = MEDMEM::swapBytes( *intBuf );
    }
#endif
    return buf;
  }
};

// ==============================================================================
/*!
 * \brief Writer of binary files
 */
// ==============================================================================

class _BinaryFileWriter
{
public:
  _BinaryFileWriter(const string& fileName)  throw (MEDEXCEPTION);

  ~_BinaryFileWriter();

  //!< write a string
  void addString(const char* str)            throw (MEDEXCEPTION);

  //!< write a string
  void addString(const string& str)          throw (MEDEXCEPTION)
  { addString( str.c_str() ); }

  //!< write an integer value
  void addInt(const int value)               throw (MEDEXCEPTION)
  { add( &value, 1 ); }
  
  //!< write integer values
  void addInt(const int* data, int nbValues) throw (MEDEXCEPTION)
  { add( data, nbValues ); }

  //!< write integer values
  void addInt(const vector< int >& data)     throw (MEDEXCEPTION)
  { add( &data[0], data.size() ); }
  
  //!< write any data as floats
  template <typename T>
  void addReal(const T* data, int nbValues)  throw (MEDEXCEPTION)
  {
    _RealData realData( data, nbValues );
    add( realData.values(), nbValues );
  }

  //!< write any data as floats
  template <class TValueIterator>
  void addReal(vector< TValueIterator >& componentIt,
               const int                 nbValues,
               const medModeSwitch       interlace)  throw (MEDEXCEPTION)
  {
    _RealData realData( componentIt, nbValues, interlace );
    add( realData.values(), nbValues * componentIt.size() );
  }

private:

  int          _file; //!< descriptor
  MEDEXCEPTION _exception; //!< ready to raise exception
    
  //!< write any data to file
  template <typename T>
  void add(const T* data, int nbValues) throw (MEDEXCEPTION)
  {
#ifdef WNT
#else
    ssize_t nbWritten = ::write( _file, (const void *) data, nbValues * sizeof(T));
    if ( nbWritten < 0 ) throw _exception;
#endif
  }
  // ------------------------------------------------------------------------
  /*!
   * \brief Container of temporary data converting any data to floats
   */
  // ------------------------------------------------------------------------
  class _RealData {
    PointerOf<float> _floatData;
  public:
    //!< return pointer to float array
    const float* values() { return _floatData; }

    //!< convert nbValues to floats
    template <typename T>
    _RealData(const T* data, int nbValues)
    {
      if ( sizeof( T ) == sizeof( float ))
        _floatData.set((const float*) data);
      else {
        _floatData.set(nbValues);
        float* floatPtr = _floatData;
        const T *tPtr = data, *tEnd = data + nbValues;
        while ( tPtr < tEnd )
          *floatPtr++ = _toFloat( *tPtr++ );
      }
    }

    //!< convert nbValues to floats in given interlace
    template <class TValueIterator>
    _RealData(vector< TValueIterator >& componentIt,
              const int                 nbValues,
              const medModeSwitch       interlace)
    {
      int nbComponents = componentIt.size();
      _floatData.set(nbValues * nbComponents);
      float* floatPtr = _floatData;
      if ( interlace == MED_EN::MED_FULL_INTERLACE && nbComponents > 1 ) {
        for ( int i = 0; i < nbValues; ++i )
          for ( int j = 0; j < nbComponents; ++j )
            *floatPtr++ = _toFloat( componentIt[ j ].next() );
      }
      else {
        for ( int j = 0; j < nbComponents; ++j ) {
          TValueIterator & values = componentIt[ j ];
          for ( int i = 0; i < nbValues; ++i )
            *floatPtr++ = _toFloat( values.next() );
        }
      } 
    }
  }; // class _RealData

}; // class _BinaryFileWriter

}// namespace MEDMEM_ENSIGHT

#endif
