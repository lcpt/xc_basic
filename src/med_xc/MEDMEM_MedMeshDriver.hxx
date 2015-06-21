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

#ifndef MED_MESH_DRIVER_HXX
#define MED_MESH_DRIVER_HXX

#include "MEDMEM.hxx"

#include <string>
#include <vector>
#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"

#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Utilities.hxx"

/*!

  Driver Med for MESH.

  Generic part : implement open and close methods.

*/

namespace MEDMEM {
class GMESH;
class FAMILY;
class GROUP;
class CONNECTIVITY;
class MEDMEM_EXPORT MED_MESH_DRIVER : public GENDRIVER
{
protected:

  GMESH *             _ptrMesh;
  mutable std::string _meshName;
  med_idt    _medIdt ;
  


public :

  // all MED cell type
  static const med_geometry_type all_cell_type[MED_N_CELL_GEO_FIXED_CON];
  
  static const char * const all_cell_type_tab [MED_N_CELL_GEO_FIXED_CON];

  /*!
    Constructor.
  */
  MED_MESH_DRIVER() ;
  /*!
    Constructor.
  */
  MED_MESH_DRIVER(const std::string &    fileName,
                  GMESH *                ptrMesh,
                  MED_EN::med_mode_acces accessMode) ;
  /*!
    Copy constructor.
  */
  MED_MESH_DRIVER(const MED_MESH_DRIVER & driver) ;

  /*!
    Destructor.
  */
  virtual ~MED_MESH_DRIVER() ;

  virtual void open();
  virtual void close();

  virtual void write( void ) const = 0 ;
  virtual void read ( void ) = 0 ;

  /*!
    Set the name of the MESH asked in file.

    It could be different than the name of the MESH object.
  */
  virtual void   setMeshName(const std::string & meshName) ;
  /*!
    Get the name of the MESH asked in file.
  */
  virtual std::string getMeshName() const ;

public:
  virtual GENDRIVER * copy ( void ) const = 0 ;

};

/*!

  Driver Med for MESH : Read only.

  Implement read method.

*/

class MEDMEM_EXPORT MED_MESH_RDONLY_DRIVER : public virtual MED_MESH_DRIVER
{
 
public :
  
  /*!
    Constructor.
  */
  MED_MESH_RDONLY_DRIVER() ;
  /*!
    Constructor.
  */
  MED_MESH_RDONLY_DRIVER(const std::string & fileName, GMESH * ptrMesh) ;
  /*!
    Copy constructor.
  */
  MED_MESH_RDONLY_DRIVER(const MED_MESH_RDONLY_DRIVER & driver) ;
  /*!
    Destructor.
  */
  virtual ~MED_MESH_RDONLY_DRIVER() ;

   /*!
     Return a MEDEXCEPTION : it is the read-only driver.
   */
  void write( void ) const;
  /*!
    Read MESH in the specified file.
  */
  void read ( void );
  /*!
   *  This method activate global faces computation from SCRATCH if a family on FACE exists in the MED file.
   *  This implies a complete renumbering of FACES. This is the default behaviour of this driver.
   */
  void activateFacesComputation() { _computeFaces=true; }
  /*!
   *  This method desactivate global face computation.
   *  That is to say that FACES described in file are supposed to
   *  be correct and complete. The consequence is that reading is much faster. Use with care !
   */
  void desactivateFacesComputation() { _computeFaces=false; }

protected:
  int getDescendingConnectivity(CONNECTIVITY * Connectivity);
  void updateFamily() ;
  void buildAllGroups(std::vector<GROUP*> & Groups, std::vector<FAMILY*> & Families) ;

private:
  int getCOORDINATE();
  int getCONNECTIVITY();
  int getFAMILY();
  int getNodalConnectivity(CONNECTIVITY * Connectivity) ;
  int getNodesFamiliesNumber(int * MEDArrayNodeFamily) ;
  int getCellsFamiliesNumber(int** Arrays, /*CONNECTIVITY* Connectivity, */MED_EN::medEntityMesh entity) ;
  void getGRID ();

  GENDRIVER * copy ( void ) const ;
  virtual void merge ( const GENDRIVER& driver );

private:
  bool _computeFaces;
};

/*!

  Driver Med for MESH : Write only.

  Implement write method.

*/

class MEDMEM_EXPORT MED_MESH_WRONLY_DRIVER : public virtual MED_MESH_DRIVER
  {
  public :
  
  /*!
    Constructor.
  */
  MED_MESH_WRONLY_DRIVER();
  /*!
    Constructor.
  */
  MED_MESH_WRONLY_DRIVER(const std::string &    fileName,
                         GMESH *                ptrMesh,
                         MED_EN::med_mode_acces access=MED_EN::WRONLY);
  /*!
    Copy constructor.
  */
  MED_MESH_WRONLY_DRIVER(const MED_MESH_WRONLY_DRIVER & driver);

  /*!
    Destructor.
  */
  virtual ~MED_MESH_WRONLY_DRIVER();

  /*!
    Return a MEDEXCEPTION : it is the write-only driver.
  */
  void read ( void );

  /*!
    Write MESH in the specified file.
  */
  void write( void ) const;

private:
  int writeCoordinates    ()                           const;
  int writeConnectivities (MED_EN::medEntityMesh entity)       const;
  void groupFamilyConverter(const std::vector<GROUP*>& groups,
                            std::vector<FAMILY*>&      families) const;
  int writeFamilyNumbers  ()                           const;
  int writeFamilies       (std::vector<FAMILY*> & families) const;
  int writeGRID() const;

  GENDRIVER * copy ( void ) const;
};


/*!

  Driver Med for MESH : Read write.
  - Use read method from MED_MESH_RDONLY_DRIVER
  - Use write method from MED_MESH_WRONLY_DRIVER

*/

class MEDMEM_EXPORT MED_MESH_RDWR_DRIVER: public virtual MED_MESH_RDONLY_DRIVER, public virtual MED_MESH_WRONLY_DRIVER 
  {
  private:
    GENDRIVER * copy(void) const;

  public :

  /*!
    Constructor.
  */
  MED_MESH_RDWR_DRIVER();
  /*!
    Constructor.
  */
  MED_MESH_RDWR_DRIVER(const std::string & fileName, GMESH * ptrMesh);
  /*!
    Copy constructor.
  */
  MED_MESH_RDWR_DRIVER(const MED_MESH_RDWR_DRIVER & driver);

  /*!
    Destructor.
  */
  ~MED_MESH_RDWR_DRIVER();

  /*!
    Write MESH in the specified file.
  */
  void write(void) const;
  /*!
    Read MESH in the specified file.
  */
  void read (void);


};

}

#endif /* MED_MESH_DRIVER_HXX */
