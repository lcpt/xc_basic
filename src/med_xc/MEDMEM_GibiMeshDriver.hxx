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

#ifndef GIBI_MESH_DRIVER_HXX
#define GIBI_MESH_DRIVER_HXX

#include "MEDMEM.hxx"

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <list>

#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_FieldForward.hxx"

/////
#include <sstream>
#include <iomanip>
/////

#ifdef WNT
#else
#define HAS_XDR
#endif

/*!

  Driver GIBI for MESH.

  Generic part : implement the readopen and close methods.

*/

namespace MEDMEM {
class GMESH;
class MESH;
class FAMILY;
class GROUP;
class SUPPORT;
class CONNECTIVITY;
class FIELD_;
struct _intermediateMED;

// IMP 0020434: mapping GIBI names to MED names
struct nameGIBItoMED {
  // GIBI value
  int gibi_pile;    // PILE_SOUS_MAILLAGE or PILE_FIELD/PILE_NODES_FIELD, or PILE_STRINGS(for components)
  int gibi_id;
  string gibi_name; // used only for components
  // MED value
  // med_pile = 27; // PILE_STRINGS
  int med_id;       // used only on reading
  string med_name;  // used only on writing
};

class MEDMEM_EXPORT GIBI_MESH_DRIVER : public GENDRIVER
{
protected:

  GMESH *         _mesh;
  std::string    _meshName;

  // This enumeration is used to substitude static const
  // memers data causing link errors on VC7 compiler.
  enum
  {
    nb_geometrie_gibi = 47
  };
  // tableau de correspondance des types géométriques de CASTEM -> MED
  //static const size_t nb_geometrie_gibi=47;
  static const MED_EN::medGeometryElement geomGIBItoMED[nb_geometrie_gibi];
  /////

public :

  /*!
    Constructor.
  */
  GIBI_MESH_DRIVER() ;
  /*!
    Constructor.
  */
  GIBI_MESH_DRIVER(const string &         fileName,
                   GMESH *                ptrMesh,
                   MED_EN::med_mode_acces accessMode) ;
  /*!
    Copy constructor.
  */
  GIBI_MESH_DRIVER(const GIBI_MESH_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~GIBI_MESH_DRIVER() ;

  virtual void write( void ) const = 0 ;
  virtual void read ( void ) = 0 ;

  /*!
    Set the name of the MESH asked in file.

    It could be different than the name of the MESH object.
  */
  void   setMeshName(const string & meshName) ;
  /*!
    Get the name of the MESH asked in file.
  */
  string getMeshName() const ;

  static MED_EN::medGeometryElement gibi2medGeom( size_t gibiTypeNb );
  static int med2gibiGeom( MED_EN::medGeometryElement medGeomType );

 private:
  virtual GENDRIVER * copy ( void ) const = 0 ;

};


class MEDMEM_EXPORT GIBI_MESH_RDONLY_DRIVER : public virtual GIBI_MESH_DRIVER
{
public :

  /*!
    Constructor.
  */
  GIBI_MESH_RDONLY_DRIVER() ;
  /*!
    Constructor.
  */
  GIBI_MESH_RDONLY_DRIVER(const string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  GIBI_MESH_RDONLY_DRIVER(const GIBI_MESH_RDONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~GIBI_MESH_RDONLY_DRIVER() ;

  /*!
    Return a MEDEXCEPTION : it is the read-only driver.
  */
  void write( void ) const throw (MEDEXCEPTION);
  /*!
    Read MESH in the specified file.
  */
  void read ( void ) throw (MEDEXCEPTION);

  void open();
  void close();

protected:

  bool readFile(_intermediateMED*  medi, bool readFields );

  void fillMesh(_intermediateMED* medi);

  void updateSupports();

private:

  GENDRIVER * copy ( void ) const ;

  // fields and methods for file reading

  bool getLine(char* & line);
  bool getNextLine (char* & line, bool raiseIfNot = true ) throw (MEDEXCEPTION)
  {
    if ( getLine( line )) return true;
    if ( raiseIfNot ) throw MEDEXCEPTION(LOCALIZED(STRING("Unexpected EOF on ln ")<<_lineNb));
    return false;
  }
  void initNameReading(int nbValues, int width = 8);  // FORMAT(8(1X,A8))
  void initIntReading(int nbValues); //  FORMAT(10I8)
  void initDoubleReading(int nbValues); //  FORMAT(1P,3E22.14)
  void init( int nbToRead, int nbPosInLine, int width, int shift = 0 );
  bool more() const;
  void next();
  char* str() const { return _curPos; }
  int index() const { return _iRead; }
  int getInt() const;
  float getFloat() const;
  double getDouble() const;
  string getName() const;

  // line getting
  int   _File;
  char* _start;
  char* _ptr;
  char* _eptr;
  int   _lineNb;

  // string reading
  int _iPos, _nbPosInLine, _width, _shift;
  int _iRead, _nbToRead;
  char* _curPos;

  // xdr
#ifdef HAS_XDR
  bool _is_xdr;
  FILE* _xdrs_file;
  void* _xdrs;
  char* _xdr_cvals;
  int* _xdr_ivals;
  double* _xdr_dvals;
  int _xdr_kind;
  enum
    {
      _xdr_kind_null,
      _xdr_kind_char,
      _xdr_kind_int,
      _xdr_kind_double
    };
#endif
};

/*!

  Driver Med for MESH : Write only.

  Implement write method.

*/

class MEDMEM_EXPORT GIBI_MESH_WRONLY_DRIVER : public virtual GIBI_MESH_DRIVER {

public :

  /*!
    Constructor.
  */
  GIBI_MESH_WRONLY_DRIVER() ;
  /*!
    Constructor.
  */
  GIBI_MESH_WRONLY_DRIVER(const string & fileName, GMESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  GIBI_MESH_WRONLY_DRIVER(const GIBI_MESH_WRONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~GIBI_MESH_WRONLY_DRIVER() ;

  /*!
    Write MESH in the specified file.
  */
  void write( void ) const throw (MEDEXCEPTION);
  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void ) throw (MEDEXCEPTION);

  void open();
  void close();

 protected:

  fstream _gibi;

  /*!
    Add a support to write. Return true if a support will be written
  */
  bool addSupport( const SUPPORT * support );
  /*!
    return submesh id and size for a support. Call it after writeSupportsAndMesh()
  */
  int getSubMeshIdAndSize(const SUPPORT *                  support,
                          std::list<std::pair<int,int> > & idsAndSizes ) const;
  /*!
    Write MESH and _supports.
  */
  void writeSupportsAndMesh(list<nameGIBItoMED>& listGIBItoMED_mail);
  /*!
   * Store MED names of supports and fields.
   * All MED names are written in STRINGS PILE, and the correspondence
   * between GIBI and MED names is written as TABLE "noms_med"
  */
  void writeMEDNames(const std::list<nameGIBItoMED>& listGIBItoMED_mail,
                     const std::list<nameGIBItoMED>& listGIBItoMED_cham,
                     const std::list<nameGIBItoMED>& listGIBItoMED_comp);
  /*!
    Write the record closing file
  */
  void writeLastRecord();

  //static void addName( map<string,int>& nameMap, string& name, int index, string prefix );
  static void addName( std::map<std::string,int>& nameMap,
                       std::map<std::string,int>& namePrefixesMap,
                       const std::string&         name,
                       int                        index);

  void writeNames( std::map<std::string,int>& nameMap );

 private:

  struct typeData
  {
    int         _nbElems;
    const int * _ptrElemIDs; // elem ids or
    int         _elemID1;    // first elem id if isOnAllElements()
    typeData( int nbElems=0, const int * ptrElemIDs=NULL, int elemID1=0 )
      : _nbElems(nbElems),  _ptrElemIDs(ptrElemIDs), _elemID1(elemID1) {}
  };
  struct supportData
  {
    typedef map< MED_EN::medGeometryElement, list< typeData > >::iterator typeIterator;
    int                                         _id;
    string                                      _cleanName;
    map< MED_EN::medGeometryElement, list< typeData > > _types;
    supportData(): _id(0) {}
    int getNumberOfTypes() const { return _types.size(); }
    int getNumberObjects() const
    { return _types.size() < 2 ? _types.size() : _types.size() + !_cleanName.empty(); }
    void addTypeData(MED_EN::medGeometryElement type, int nbElems,
                     const int * ptrElemIDs,  int elemID1 )
    { _types[type].push_back( typeData( nbElems, ptrElemIDs, elemID1 )); }
  };

  void writeElements (MED_EN::medGeometryElement geomType,
                      list< typeData >&  typeDataList,
                      const int *        nodalConnect,
                      const int *        nodalConnectIndex);

  map<const SUPPORT*, supportData> _supports;

  GENDRIVER * copy ( void ) const ;
};


/*!

  Driver GIBI for MESH : Read write.
  - Use read method from GIBI_MESH_RDONLY_DRIVER
  - Use write method from GIBI_MESH_WRONLY_DRIVER

*/

class MEDMEM_EXPORT GIBI_MESH_RDWR_DRIVER : public GIBI_MESH_RDONLY_DRIVER, public GIBI_MESH_WRONLY_DRIVER {

public :

  /*!
    Constructor.
  */
  GIBI_MESH_RDWR_DRIVER() ;
  /*!
    Constructor.
  */
  GIBI_MESH_RDWR_DRIVER(const std::string & fileName, MESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  GIBI_MESH_RDWR_DRIVER(const GIBI_MESH_RDWR_DRIVER & driver) ;

  /*!
    Destructor.
  */
  ~GIBI_MESH_RDWR_DRIVER() ;

  /*!
    Write MESH in the specified file.
  */
  void write(void) const throw (MEDEXCEPTION);
  /*!
    Read MESH in the specified file.
  */
  void read (void) throw (MEDEXCEPTION);

  void open();
  void close();

 private:
  GENDRIVER * copy(void) const ;

};

class MEDMEM_EXPORT GIBI_MED_RDONLY_DRIVER : public GIBI_MESH_RDONLY_DRIVER {

  std::vector<FIELD_*> * _fields;

public:

  /*!
    Constructor.
  */
  GIBI_MED_RDONLY_DRIVER() ;
  /*!
    Constructor.
  */
  GIBI_MED_RDONLY_DRIVER(const std::string & fileName, std::vector<FIELD_*>& ptrFields) ;
  /*!
    Copy constructor.
  */
  GIBI_MED_RDONLY_DRIVER(const GIBI_MED_RDONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~GIBI_MED_RDONLY_DRIVER() ;

  /*!
    Read MESH and FIELDs in the specified file.
  */
  void read ( void ) throw (MEDEXCEPTION);

  MESH* getMesh() const;

private:

  GENDRIVER * copy ( void ) const ;

};

/*!

  Driver Med for MESH : Write only.

  Implement write method.

*/

class MEDMEM_EXPORT GIBI_MED_WRONLY_DRIVER : public GIBI_MESH_WRONLY_DRIVER {

  std::vector<const FIELD_*> _fields;

public :

  /*!
    Constructor.
  */
  GIBI_MED_WRONLY_DRIVER() ;
  /*!
    Constructor. To write a mesh and all fields on it
  */
  GIBI_MED_WRONLY_DRIVER(const std::string &               fileName,
                         const std::vector<const FIELD_*>& fields,
                         GMESH *                           ptrMesh);
  /*!
    Copy constructor.
  */
  GIBI_MED_WRONLY_DRIVER(const GIBI_MED_WRONLY_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~GIBI_MED_WRONLY_DRIVER() ;

  /*!
    Write MESH in the specified file.
  */
  void write( void ) const throw (MEDEXCEPTION);

  //int getSupports(const FIELD_* field, list<const SUPPORT*>& supList) const;

private:

  GENDRIVER * copy ( void ) const ;
};

}


#endif /* GIBI_MESH_DRIVER_HXX */
