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

#ifndef MED_FIELD_DRIVER_HXX
#define MED_FIELD_DRIVER_HXX

#include "MEDMEM_GenDriver.hxx"

namespace MEDMEM {
template <class T> class MED_FIELD_RDWR_DRIVER;
template <class T> class MED_FIELD_RDONLY_DRIVER;
template <class T> class MED_FIELD_WRONLY_DRIVER;

/*!

  Driver Med for FIELD.

  Generic part : implement open and close methods.

*/

template <class T> class MED_FIELD_DRIVER : public GENDRIVER
{
protected:
  // Developement plus propre :
  // - Il faudrait soit utiliser le type FIELD_ et ajouter à cette classe
  //   les accesseurs de FIELD<> utilisées dans les drivers
  // - Ou bien avoir des drivers à deux paramètres template (le top)
  // - Remarquez l'affreux cast dans le second constructeur :
  //      _ptrField( (FIELD<T> *) ptrField )
  //   Cela cast toujours le ptrField en FullInterlace
  //   Cela ne pose cependant pas de pb de fonctionement aux drivers
  FIELD<T> *       _ptrField;
  std::string      _fieldName;
  int              _fieldNum;
  med_idt _medIdt;

public :

  MED_FIELD_DRIVER();

  template <class INTERLACING_TAG>
  MED_FIELD_DRIVER(const std::string &         fileName,
                   FIELD<T, INTERLACING_TAG> * ptrField,
                   MED_EN::med_mode_acces      accessMode);

  MED_FIELD_DRIVER(const MED_FIELD_DRIVER & fieldDriver);

  virtual ~MED_FIELD_DRIVER();

  virtual void open() throw (MEDEXCEPTION);
  virtual void close();
  virtual void write( void ) const = 0 ;
  virtual void read ( void ) = 0 ;
  /*!
    Set the name of the FIELD asked in file.

    It could be different than the name of the FIELD object.
  */
  virtual void   setFieldName(const std::string & fieldName) { _fieldName = fieldName; }
  /*!
    Get the name of the FIELD asked in file.
  */
  virtual std::string getFieldName() const { return _fieldName; }

protected:
  virtual GENDRIVER * copy ( void ) const = 0 ;

  bool createFieldSupportPart1(med_idt        id,
                               const std::string &     fieldName,
                               med_int        ndt,
                               med_int        od,
                               SUPPORT &               support,
                               std::string &           meshName,
                               std::vector<int> &      numberOfElementsOfTypeC,
                               std::vector<int> &      numberOfGaussPoint,
                               std::vector<std::string>& gaussModelName,
                               std::vector<std::string>& profilName,
                               int &                   totalNumberOfElWg,
                               MED_EN::medEntityMesh & fieldMedFileEntity,
                               MED_EN::medEntityMesh   preferEntity=MED_EN::MED_ALL_ENTITIES
                               ) const throw (MEDEXCEPTION);

  void getMeshGeometricTypeFromFile(med_idt      id,
                                    std::string &              meshName,
                                    MED_EN::medEntityMesh entite,
                                    std::vector<MED_EN::medGeometryElement> & geoType,
                                    std::vector<int> &         nbOfElOfType,
                                    std::vector<int> &         nbOfElOfTypeC
                                    ) const throw(MEDEXCEPTION);

  void getMeshGeometricTypeFromMESH( const GMESH * meshPtr,
                                     MED_EN::medEntityMesh  entity,
                                     std::vector<MED_EN::medGeometryElement> & geoType,
                                     std::vector<int> &nbOfElOfType,
                                     std::vector<int> &nbOfElOfTypeC
                                     ) const throw(MEDEXCEPTION);

  int getMeshDimensionFromFile(med_idt id, const string & meshName) const;
        
  MED_EN::medEntityMesh getMEDMEMEntityFromMEDType(MED_EN::medGeometryElement type,
                                                   int                        mesh_dim) const;

};

/*!

  Driver Med for FIELD : Read only.

  Implement read method.

*/

template <class T> class MED_FIELD_RDONLY_DRIVER : public virtual MED_FIELD_DRIVER<T>
{
public :

  MED_FIELD_RDONLY_DRIVER();

  template <class INTERLACING_TAG>
  MED_FIELD_RDONLY_DRIVER(const string &              fileName,
                          FIELD<T, INTERLACING_TAG> * ptrField);

  MED_FIELD_RDONLY_DRIVER(const MED_FIELD_RDONLY_DRIVER & fieldDriver);

  virtual ~MED_FIELD_RDONLY_DRIVER() {};

  void write( void ) const throw (MEDEXCEPTION) ;
  void read ( void ) throw (MEDEXCEPTION) ;

private:
  GENDRIVER * copy( void ) const ;

};

/*!

  Driver Med for FIELD : Write only.

  Implement write method.

*/

template <class T> class MED_FIELD_WRONLY_DRIVER : public virtual MED_FIELD_DRIVER<T>
{
public :

  MED_FIELD_WRONLY_DRIVER();

  template <class INTERLACING_TAG>
  MED_FIELD_WRONLY_DRIVER(const string &              fileName,
                          FIELD<T, INTERLACING_TAG> * ptrField);

  MED_FIELD_WRONLY_DRIVER(const MED_FIELD_WRONLY_DRIVER & fieldDriver);

  virtual ~MED_FIELD_WRONLY_DRIVER();

  /*!
    Write FIELD in the specified file.
  */
  void write( void ) const throw (MEDEXCEPTION) ;
  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void ) throw (MEDEXCEPTION) ;

private:
  GENDRIVER * copy( void ) const ;

};


/*!

  Driver Med for FIELD : Read write.
  - Use read method from MED_FIELD_RDONLY_DRIVER
  - Use write method from MED_FIELD_WDONLY_DRIVER

*/

template <class T> class MED_FIELD_RDWR_DRIVER : public MED_FIELD_RDONLY_DRIVER<T>, public MED_FIELD_WRONLY_DRIVER<T>
{
public :

  MED_FIELD_RDWR_DRIVER();

  template <class INTERLACING_TAG>
  MED_FIELD_RDWR_DRIVER(const string &              fileName,
                        FIELD<T, INTERLACING_TAG> * ptrField);

  MED_FIELD_RDWR_DRIVER(const MED_FIELD_RDWR_DRIVER & fieldDriver);

  /*!
    Destructor.
  */
  ~MED_FIELD_RDWR_DRIVER();

  /*!
    Write FIELD in the specified file.
  */
  void write(void) const throw (MEDEXCEPTION) ;
  /*!
    Read FIELD in the specified file.
  */
  void read (void) throw (MEDEXCEPTION) ;

private:
  GENDRIVER * copy( void ) const ;

};

} //End namespace MEDMEM

// implementation
#include "MEDMEM_MedFieldDriver.txx"

#endif /* MED_FIELD_DRIVER_HXX */
