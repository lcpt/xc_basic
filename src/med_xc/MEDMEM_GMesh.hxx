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
// File      : MEDMEM_GMesh.hxx
// Created   : Fri Jul 23 10:17:08 2010
// Author    : Edward AGAPOV (eap)
//

#ifndef __MEDMEM_GMesh_HXX__
#define __MEDMEM_GMesh_HXX__

#include "MEDMEM.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_RCBase.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_FieldForward.hxx"
#include "MEDMEM_Support.hxx"

#include <map>

namespace MEDMEM {

class FAMILY;
class GROUP;
class SUPPORT;
class GMESH;
class MESH;

MEDMEM_EXPORT std::ostream & operator<<(std::ostream &os, const GMESH &me);

/*!
 * \brief Base class of structured and unstructured meshes
 */
class MEDMEM_EXPORT GMESH: public RCBASE
  {
  protected:

    std::string _name;
    std::string _description;
    int _spaceDimension;

    std::vector<FAMILY*> _familyNode;
    std::vector<FAMILY*> _familyCell;
    std::vector<FAMILY*> _familyFace;
    std::vector<FAMILY*> _familyEdge;

    std::vector<GROUP*>  _groupNode;
    std::vector<GROUP*>  _groupCell;
    std::vector<GROUP*>  _groupFace;
    std::vector<GROUP*>  _groupEdge;

    std::vector<GENDRIVER *> _drivers; // Storage of the drivers currently in use

    std::map<MED_EN::medEntityMesh,SUPPORT*> _entitySupport;

    virtual bool isEmpty() const = 0;

    // Add your personnal driver line (step 2)
    friend class MED_MESH_RDONLY_DRIVER;
    friend class MED_MESH_WRONLY_DRIVER;

    friend class GIBI_MESH_RDONLY_DRIVER;
    friend class GIBI_MESH_WRONLY_DRIVER;
    friend class GIBI_MESH_RDWR_DRIVER;

    friend class PORFLOW_MESH_RDONLY_DRIVER;
    friend class PORFLOW_MESH_WRONLY_DRIVER;
    friend class PORFLOW_MESH_RDWR_DRIVER;

    friend class VTK_MESH_DRIVER;

    friend class ENSIGHT_MESH_RDONLY_DRIVER;

   public:

    GMESH();
    GMESH(GMESH &m);
    virtual ~GMESH();

    virtual void init();
    GMESH & operator=(const GMESH &m);
    virtual bool operator==(const GMESH& other) const;
    virtual bool deepCompare(const GMESH& other) const = 0;

    friend std::ostream & operator<<(std::ostream &os, const GMESH &me);
    virtual void printMySelf(std::ostream &os) const = 0;

    int addDriver(driverTypes driverType,
                   const std::string &    fileName ="Default File Name.med",
                   const std::string &    driverName="Default Mesh Name",
                   MED_EN::med_mode_acces access=MED_EN::RDWR);
    int addDriver(GENDRIVER & driver);
    void rmDriver(int index=0);

    virtual void read(int index=0);
    virtual void read(const GENDRIVER & genDriver);
    virtual void read(driverTypes driverType,
                      const std::string& filename,
                      const std::string& meshname);

    virtual void write(int index=0) const ;
    virtual void write(const GENDRIVER & genDriver,
                       MED_EN::med_mode_acces medMode=MED_EN::WRONLY) const;
    virtual void write(driverTypes driverType,
                       const std::string& filename,
                       const std::string& meshname="",
                       MED_EN::med_mode_acces medMode=MED_EN::WRONLY) const;

    void setName(std::string name);
    std::string getName() const;

    void setDescription(std::string description);
    std::string getDescription() const;

    int getSpaceDimension() const;
    virtual int getMeshDimension() const = 0;

    virtual bool getIsAGrid() const = 0;

    virtual std::string getCoordinatesSystem() const = 0;
    virtual const std::string * getCoordinatesNames() const = 0;
    virtual const std::string * getCoordinatesUnits() const = 0;

    virtual int getNumberOfNodes() const = 0;
    virtual int getNumberOfTypes(MED_EN::medEntityMesh Entity) const = 0;
    virtual int getNumberOfElements(MED_EN::medEntityMesh Entity,
                                                           MED_EN::medGeometryElement Type) const = 0;
    virtual const MED_EN::medGeometryElement * getTypes(MED_EN::medEntityMesh Entity) const = 0;
    virtual MED_EN::medGeometryElement getElementType(MED_EN::medEntityMesh Entity,
                                                      int Number) const = 0;


    virtual int getNumberOfFamilies(MED_EN::medEntityMesh Entity) const;
    virtual const std::vector<FAMILY*> getFamilies(MED_EN::medEntityMesh Entity) const;
    virtual const FAMILY* getFamily(MED_EN::medEntityMesh Entity,int i) const;

    virtual int getNumberOfGroups(MED_EN::medEntityMesh Entity) const;
    virtual const std::vector<GROUP*>  getGroups(MED_EN::medEntityMesh Entity) const;
    virtual const GROUP* getGroup(MED_EN::medEntityMesh Entity,int i) const;
    virtual const GROUP* getGroup(const std::string& name) const throw (MEDEXCEPTION);

    /*!
     * \brief Return unstructured mesh. Call removeReference() after having finished using it!!!
     */
    virtual const MESH *convertInMESH() const = 0;


    virtual SUPPORT *getBoundaryElements(MED_EN::medEntityMesh Entity) const
                                                  throw (MEDEXCEPTION) = 0;
    virtual SUPPORT *getSkin(const SUPPORT * Support3D)
                                                  throw (MEDEXCEPTION) = 0;

    virtual const SUPPORT *getSupportOnAll(MED_EN::medEntityMesh Entity) const
                                                  throw (MEDEXCEPTION);

    static SUPPORT *mergeSupports(const std::vector<SUPPORT *> Supports)
                                                  throw (MEDEXCEPTION) ;
    static SUPPORT *intersectSupports(const std::vector<SUPPORT *> Supports)
                                                  throw (MEDEXCEPTION) ;

    virtual SUPPORT *buildSupportOnNodeFromElementList(const std::list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION) = 0;
    virtual void fillSupportOnNodeFromElementList(const std::list<int>& listOfElt, SUPPORT *supportToFill) const throw (MEDEXCEPTION) = 0;
    virtual SUPPORT *buildSupportOnElementsFromElementList(const std::list<int>& listOfElt, MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);


    virtual FIELD<double>* getVolume (const SUPPORT * Support, bool isAbs = true) const
                                  throw (MEDEXCEPTION) = 0;
                                  // Support must be on 3D elements
    virtual FIELD<double>* getArea (const SUPPORT * Support) const
                                  throw (MEDEXCEPTION) = 0;
                                  // Support must be on 2D elements
    virtual FIELD<double>* getLength (const SUPPORT * Support) const
                                  throw (MEDEXCEPTION) = 0;
                                  // Support must be on 1D elements
    virtual FIELD<double>* getNormal (const SUPPORT * Support) const
                                  throw (MEDEXCEPTION) = 0;
                                  // Support must be on 2D elements
    virtual FIELD<double>* getBarycenter (const SUPPORT * Support) const
                                  throw (MEDEXCEPTION) = 0;
    
    virtual vector< vector<double> >   getBoundingBox() const = 0;

    template<class T> static FIELD<T>* mergeFields(const std::vector< FIELD<T> * > & others,
                                                   bool meshCompare=false);

    void createFamilies();
    void createGroups();
  };

// ================================================================================================
//Create a new FIELD that should be deallocated based on a SUPPORT that should be deallocated too.
// ================================================================================================
template<class T>
FIELD<T> * GMESH::mergeFields(const std::vector< FIELD<T> * > & others, bool meshCompare)
{
  const char * LOC = "MESH::mergeFields(const vector< FIELD<T>* >& others,bool meshCompare): ";
  BEGIN_OF_MED(LOC);
  int i,j;
  if(others.size()==0)
    return 0;
  std::vector<SUPPORT *> sup;
  typename std::vector< FIELD<T, FullInterlace>* >::const_iterator iter;
  iter = others.begin();
  MED_EN::med_type_champ valueType = (*iter)->getValueType();
  for(iter=others.begin();iter!=others.end();iter++)
    {
      MED_EN::med_type_champ valueTypeIter = (*iter)->getValueType();
      if (valueTypeIter != valueType)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Fields vector have not the same value type"));

      sup.push_back((SUPPORT *)(*iter)->getSupport());
    }
  iter=others.begin();
  SUPPORT *retSup=mergeSupports(sup);
  int retNumberOfComponents=(*iter)->getNumberOfComponents();
  FIELD<T, FullInterlace> *ret=new FIELD<T, FullInterlace>(retSup, retNumberOfComponents);
  T* valuesToSet=(T*)ret->getValue();
  int nbOfEltsRetSup=retSup->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
  T* tempValues=new T[retNumberOfComponents];
  if(retSup->isOnAllElements())
    {
      for(i=0;i<nbOfEltsRetSup;i++)
        {
          bool found=false;
          for(iter=others.begin();iter!=others.end() && !found;iter++)
            {
              found=(*iter)->getValueOnElement(i+1,tempValues);
              if(found)
                for(j=0;j<retNumberOfComponents;j++)
                  valuesToSet[i*retNumberOfComponents+j]=tempValues[j];
            }
        }
    }
  else
    {
      const int *eltsRetSup=retSup->getNumber(MED_EN::MED_ALL_ELEMENTS);
      for(i=0;i<nbOfEltsRetSup;i++)
        {
          bool found=false;
          for(iter=others.begin();iter!=others.end() && !found;iter++)
            {
              found=(*iter)->getValueOnElement(eltsRetSup[i],tempValues);
              if(found)
                for(j=0;j<retNumberOfComponents;j++)
                  valuesToSet[i*retNumberOfComponents+j]=tempValues[j];
            }
          if(!found)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Merging error due to an error in merging support"));
        }
    }
  if(retSup)
    retSup->removeReference();
  delete [] tempValues;
  END_OF_MED(LOC);
  return ret;
}
}
#endif
