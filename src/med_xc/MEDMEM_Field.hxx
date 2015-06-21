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

/*
 File Field.hxx
*/

#ifndef FIELD_HXX
#define FIELD_HXX

#include "MEDMEM.hxx"

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_MedVersion.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_nArray.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_RCBase.hxx"
#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_SetInterlacingType.hxx"
#include "MEDMEM_FieldForward.hxx"
#include "MEDMEM_GaussLocalization.hxx"
#include "InterpKernelGaussCoords.hxx"
#include "PointLocator.hxx"

#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <math.h>
#include <cmath>
#include <float.h>

namespace MEDMEM {

  template<class T>
  struct MinMax {
  };

  template<>
  struct MinMax<double> {
    static double getMin() { return DBL_MIN; }
    static double getMax() { return DBL_MAX; }
  };

  template<>
  struct MinMax<int> {
    static int getMin() { return INT_MIN; }
    static int getMax() { return INT_MAX; }
  };  

  template < typename T > struct SET_VALUE_TYPE {
    static const MED_EN::med_type_champ _valueType = MED_EN::MED_UNDEFINED_TYPE;};
  template < > struct SET_VALUE_TYPE<double> {
    static const MED_EN::med_type_champ _valueType = MED_EN::MED_REEL64; };
  template < > struct SET_VALUE_TYPE<int> {
    static const MED_EN::med_type_champ _valueType = MED_EN::MED_INT32; };

        /*!\defgroup FIELD_io Reading and writing files

Fields can be read or written to/from MED files.

\par Reading fields

 For reading a field a typical use consists in :
- reading the mesh associated on which the field lies
- read the field, specifying its time step and order number

As an example :
\verbatim
//reading mesh from file
MESH mesh(MED_DRIVER, "file.med", "my_Mesh");
//reading the field from the file
FIELD<double> field(group,MED_DRIVER,"file.med","my_Field",1,1,&mesh);
\endverbatim

It is also possible to read a field without specifying its support. In this case, the field constructor 
creates a support with no link to the initial mesh:
\verbatim
FIELD<double> field(MED_DRIVER, "file.med", "myField",1,1);
SUPPORT* support= field->getSupport();
\endverbatim

See also \ref FIELD_constructors

\par Writing fields

When it comes to write fields, it is enough to call write() method.
A typical use will be :

\verbatim
mesh.write(MED_DRIVER, "myResultFile.med");
field.write(MED_DRIVER, "myResultFile.med");
\endverbatim

\defgroup FIELD_constructors

The different field constructors correspond to the two main 
ways a field is used :
- either it is read from a file to be consulted,
- or it can be created from scratch with a link to a support on which the values will be built.

\defgroup FIELD_algo Numerical operations on fields
This section groups together the different operators that enable the user to 
treat the FIELD objects as high-level numerical arrays, giving operators for 
numerical treatment (overloading of basic operators, algorithms, etc...)

\defgroup FIELD_getset Basic Get/Set operations

This sections groups together the basic operations
that describe access to all the elements constitutive of the description of the field :
- name (compulsory),
- time iteration number(compulsory),
- inner loop iteration number(compulsory),
- time(compulsory),
- description(optional), 
- number of components(compulsory),
- components names(optional),
- components description(optional).

Some of these items are compulsory because they are essential to the field in order to define
its structure or to be identified inside a MED file during the write process. The other ones 
are there for additional information and can be overlooked if not necessary.

When creating a field by reading a file, all the parameters are set according to the file 
data and can be consulted via the get methods. When creating a file from scratch, the
 name and number of components are set by the constructor, but the other items have to be
 set via the setXXX methods.

\defgroup FIELD_gauss Gauss points

In MED, it is possible to declare a Gauss model that 
describes the location of Gauss points in a reference cell.
This Gauss model definition is contained in the 
\a GAUSS_LOCALIZATION class. A \a GAUSS_LOCALIZATION object
is associated to a field and to a type.

It is not permitted to define a Gauss model in a polygonal 
or polyhedric element.

The Gauss model can be :
- loaded from a MED file,
- written to a MED file,
- used to define a FIELD with multiple localizations per element.

\section gauss_constructors Constructing a Gauss Model

A Gauss model can be constructed with the following constructor :
\param locName defines a name associated with the gauss model
\param typeGeo names the type to which the Gauss model is assocaited  
\param nGauss defines the number of Gauss points
\param cooRef defines an array giving the coordinates of the nodes of the reference element (dimension : spaceDimension * number of nodes for type \a typeGeo)
\param cooGauss defines an array giving the coordinates of the nodes of the Gauss points (dimension : spaceDimension * \a nGauss
)
\param wg weights associated with each Gauss point (dimension : \a nGauss)

Example : in 2D, a Gauss model definition for a triangle 
would be written as :

\code
string locname("gauss model");
double cooRef[6] ={0.0, 0.0, 1.0, 0.0, 0.0, 1.0};
double cooGauss[6]={0.2, 0.2, 0.8, 0.1, 0.1, 0.8};
double wg[3]={0.3334, 0.3334, 0.3334};
GAUSS_LOCALIZATION model(locname, 
                         MED_EN::MED_TRIA3,  
                         3,
                         cooRef,
                         cooGauss,
                         wg);
\endcode

*/
        

/*!

  This class contains all the informations related with a template class FIELD :
  - Components descriptions
  - Time step description
  - Location of the values (a SUPPORT class)

*/
  class MEDMEM_EXPORT FIELD_ : public RCBASE    // GENERIC POINTER TO a template <class T, class INTERLACING_TAG> class FIELD
{
protected:

  bool            _isRead ;
  bool            _isMinMax;

  /*!
    \if developper
    Field name.
    \endif
  */
  string          _name ;
  /*!
    \if developper
    Field description.
    \endif
  */
  string          _description ;
  /*!
    \if developper
    Pointer to the support the field deals with.
    \endif
  */
  const SUPPORT * _support ;

  /*!
    \if developper
    Number of field's components.
    \endif
  */
  int             _numberOfComponents ;
  /*!
    \if developper
    Number of field's values.
    doesn't take care of _numberOfComponents
    and number of Gauss points.
    \endif
  */
  int             _numberOfValues ;

  /*!
    \if developper
    Array of size _numberOfComponents. \n
    (constant, scalar, vector, tensor)\n
    We could use an array of integer to store
    numbers of values: \n
    - 1 for scalar,\n
    - space dimension for vector,\n
    - space dimension square for tensor.\n
    So numbers of values per entities would be
    sum of _componentsTypes array.

    Not implemented yet! All type are scalar !
    \endif
  */
  vector<int>     _componentsTypes ;
  /*!
    \if developper
    Array of size _numberOfComponents
    storing components names if any.
    \endif
  */
  vector<string>  _componentsNames;
  /*!
    \if developper
    Array of size _numberOfComponents
    storing components descriptions if any.
    \endif
  */
  vector<string>  _componentsDescriptions;
  /*!
    \if developper
    Array of size _numberOfComponents
    storing components units if any.
    \endif
  */
  vector<UNIT>    _componentsUnits;
  /*!
    \if developper
    Array of size _numberOfComponents
    storing components units if any.
    \endif
  */
  vector<string>  _MEDComponentsUnits;
  /*!
    \if developper
    Iteration number of the field.
    \endif
  */
  int             _iterationNumber ;
  /*!
    \if developper
    Time of the field.
    \endif
  */
  double          _time;
  /*!
    \if developper
    Order number of the field.
    \endif
  */
  int             _orderNumber ;
  /*!
    \if developper
    At the initialization step of the field using the constructors; this attribute,
    the value type (integer or real) , is set automatically. There is a get method
    but not a set method for this attribute.
    \endif
  */
  MED_EN::med_type_champ _valueType ;
  /*!
    \if developper
    At the initialization step of the field using the constructors; this attribute,
    the interlacing type (full interlace or no interlace field value storage), is set
    automatically. There is a get method but not a set method for this attribute.
    \endif
  */
  MED_EN::medModeSwitch _interlacingType;

  vector<GENDRIVER *> _drivers; // Storage of the drivers currently in use
  static void _checkFieldCompatibility(const FIELD_& m, const FIELD_& n, bool checkUnit=true) throw (MEDEXCEPTION);
  static void _deepCheckFieldCompatibility(const FIELD_& m, const FIELD_& n, bool checkUnit=true) throw (MEDEXCEPTION);
  void _checkNormCompatibility(const FIELD<double>* p_field_volume=NULL,
                               const bool           nodalAllowed = false) const  throw (MEDEXCEPTION);
  FIELD<double>* _getFieldSize(const SUPPORT *subSupport=NULL) const;
 public:
  /*!
    Constructor.
  */
  FIELD_ ();
  /*! \ifnot MEDMEM_ug
    Constructor.
\endif
  */
  FIELD_(const SUPPORT * Support, const int NumberOfComponents);
  /*!  \ifnot MEDMEM_ug
    Copy constructor.
\endif
  */
  FIELD_(const FIELD_ &m);

  /*!
    Destructor.
  */
  virtual ~FIELD_();

public:

  friend class MED_MED_RDONLY_DRIVER21;
  friend class MED_MED_WRONLY_DRIVER21;
  friend class MED_MED_RDWR_DRIVER21;
  friend class MED_MED_RDONLY_DRIVER22;
  friend class MED_MED_WRONLY_DRIVER22;
  friend class MED_MED_RDWR_DRIVER22;
  friend class VTK_MED_DRIVER;

 FIELD_& operator=(const FIELD_ &m);

  virtual  void     rmDriver(int index=0);

  /*! \if MEDMEM_ug
    \addtogroup FIELD_io
    @{
    \endif
  */
  
  /*! Creates a driver for reading/writing fields in a file.
    \param driverType specifies the file type (MED_DRIVER, VTK_DRIVER)
    \param fileName name of the output file
    \param driverFieldName name of the field
    \param access specifies whether the file is opened for read, write or both.
  */

  virtual   int     addDriver(driverTypes driverType,
                              const string & fileName="Default File Name.med",
                              const string & driverFieldName="Default Field Nam",
                              MED_EN::med_mode_acces access=MED_EN::RDWR) ;

  virtual  int      addDriver( GENDRIVER & driver);
  virtual  void     read (driverTypes driverType, const std::string & fileName);
  virtual  void     read (const GENDRIVER &);
  virtual  void     read(int index=0);
  virtual  void     openAppend( void );
  virtual  void     write(const GENDRIVER &, MED_EN::med_mode_acces medMode=MED_EN::RDWR);
  virtual  void     write(driverTypes driverType,
                          const std::string & fileName,
                          MED_EN::med_mode_acces medMode=MED_EN::RDWR);

  /*! Triggers the writing of the field with respect to the driver handle
    \a index given by \a addDriver(...) method. */
  virtual  void     write(int index=0);
  /*!\if MEDMEM_ug @} \endif */

  virtual  void     writeAppend(const GENDRIVER &);
  virtual  void     writeAppend(int index=0, const string & driverName="");

  inline void     setName(const string Name);
  inline string   getName() const;
  inline void     setDescription(const string Description);
  inline string   getDescription() const;
  inline const SUPPORT * getSupport() const;
  inline void     setSupport(const SUPPORT * support);
  inline void     setNumberOfComponents(const int NumberOfComponents);
  inline int      getNumberOfComponents() const;
  inline void     setNumberOfValues(const int NumberOfValues);
  inline int      getNumberOfValues() const;
  inline void     setComponentsNames(const string * ComponentsNames);
  inline void     setComponentName(int i, const string ComponentName);
  inline const string * getComponentsNames() const;
  inline string   getComponentName(int i) const;
  inline void     setComponentsDescriptions(const string * ComponentsDescriptions);
  inline void     setComponentDescription(int i, const string ComponentDescription);
  inline const string * getComponentsDescriptions() const;
  inline string   getComponentDescription(int i) const;

  // provisoire : en attendant de regler le probleme des unites !
  inline void     setComponentsUnits(const UNIT * ComponentsUnits);
  inline const UNIT *   getComponentsUnits() const;
  inline const UNIT *   getComponentUnit(int i) const;
  inline void     setMEDComponentsUnits(const string * MEDComponentsUnits);
  inline void     setMEDComponentUnit(int i, const string MEDComponentUnit);
  inline const string * getMEDComponentsUnits() const;
  inline string   getMEDComponentUnit(int i) const;

  inline void     setIterationNumber(int IterationNumber);
  inline int      getIterationNumber() const;
  inline void     setTime(double Time);
  inline double   getTime() const;
  inline void     setOrderNumber(int OrderNumber);
  inline int      getOrderNumber() const;

  inline MED_EN::med_type_champ getValueType () const;
  inline MED_EN::medModeSwitch  getInterlacingType() const;
  virtual inline bool getGaussPresence() const throw (MEDEXCEPTION);
protected:
  void copyGlobalInfo(const FIELD_& m);
};

// -----------------
// Methodes Inline
// -----------------
/*! \if MEDMEM_ug 
\addtogroup FIELD_getset
@{
\endif
*/
/*!
  Sets FIELD name. The length should not exceed MED_TAILLE_NOM
as defined in Med (i.e. 32 characters).
*/
inline void FIELD_::setName(const string Name)
{
  _name=Name;
}
/*!
  Gets FIELD name.
*/
inline string FIELD_::getName() const
{
  return _name;
}
/*!
  Sets FIELD description. The length should not exceed MED_TAILLE_DESC as defined in Med (i.e. 200 characters).
*/
inline void FIELD_::setDescription(const string Description)
{
  _description=Description;
}
/*!
  Gets FIELD description.
*/
inline string FIELD_::getDescription() const
{
  return _description;
}
/*!
  Sets FIELD number of components.
*/
inline void FIELD_::setNumberOfComponents(const int NumberOfComponents)
{
  _numberOfComponents=NumberOfComponents;
  _componentsTypes.resize(_numberOfComponents);
  _componentsNames.resize(_numberOfComponents);
  _componentsDescriptions.resize(_numberOfComponents);
  _componentsUnits.resize(_numberOfComponents);
  _MEDComponentsUnits.resize(_numberOfComponents);
}
/*!
  Gets FIELD number of components.
*/
inline int FIELD_::getNumberOfComponents() const
{
  return _numberOfComponents ;
}
/*!
  Sets FIELD number of values.

  It must be the same than in the associated SUPPORT object.
*/
inline void FIELD_::setNumberOfValues(const int NumberOfValues)
{
  _numberOfValues=NumberOfValues;
}
/*!
  Gets FIELD number of value.
*/
inline int FIELD_::getNumberOfValues() const
{
  return _numberOfValues ;
}

/*!
  Sets FIELD components names.

  Duplicates the ComponentsNames string array to put components names in
  FIELD. ComponentsNames size must be equal to number of components.
*/
inline void FIELD_::setComponentsNames(const string * ComponentsNames)
{
  _componentsNames.resize(_numberOfComponents);
  for (int i=0; i<_numberOfComponents; i++)
    _componentsNames[i]=ComponentsNames[i] ;
}
/*! \ifnot MEDMEM_ug
  Sets FIELD i^th component name.

  i must be >=1 and <= number of components.
\endif
*/
inline void FIELD_::setComponentName(int i, const string ComponentName)
{
  const char LOC[]= " FIELD_::setComponentName() : ";
  BEGIN_OF_MED(LOC);
  if( i<1 || i>_numberOfComponents )
    throw MEDEXCEPTION(STRING(LOC)<<" invalid index" );

  _componentsNames[i-1]=ComponentName ;
}
/*!
  Gets a reference to the string array which contain the components names.

  This Array size is equal to number of components
*/
inline const string * FIELD_::getComponentsNames() const
{
  return &(_componentsNames[0]) ;
}
/*!\ifnot MEDMEM_ug
  Gets the name of the i^th component.
\endif
*/
inline string FIELD_::getComponentName(int i) const
{
  const char LOC[]= " FIELD_::getComponentName() : ";
  BEGIN_OF_MED(LOC);
  if( i<1 || i>_numberOfComponents )
    throw MEDEXCEPTION(STRING(LOC)<<" invalid index" );

  return _componentsNames[i-1] ;
}
/*!
  Sets FIELD components descriptions.

  Duplicates the ComponentsDescriptions string array to put components
  descriptions in FIELD.
  ComponentsDescriptions size must be equal to number of components.
*/
inline void FIELD_::setComponentsDescriptions(const string * ComponentsDescriptions)
{
  _componentsDescriptions.resize(_numberOfComponents);
  for (int i=0; i<_numberOfComponents; i++)
    _componentsDescriptions[i]=ComponentsDescriptions[i] ;
}
/*!\ifnot MEDMEM_ug
  Sets FIELD i^th component description.

  i must be >=1 and <= number of components.
\endif
*/
inline void FIELD_::setComponentDescription(int i,const string ComponentDescription)
{
  const char LOC[]= " FIELD_::setComponentDescription() : ";
  BEGIN_OF_MED(LOC);
  if( i<1 || i>_numberOfComponents )
    throw MEDEXCEPTION(STRING(LOC)<<" invalid index" );

  _componentsDescriptions[i-1]=ComponentDescription ;
}
/*!
  Gets a reference to the string array which contain the components descriptions.

  This Array size is equal to number of components
*/
inline const string * FIELD_::getComponentsDescriptions() const
{
  return &(_componentsDescriptions[0]);
}
/*!\ifnot MEDMEM_ug
  Gets the description of the i^th component.
\endif
*/
inline string FIELD_::getComponentDescription(int i) const
{
  const char LOC[]= " FIELD_::setComponentDescription() : ";
  BEGIN_OF_MED(LOC);
  if( i<1 || i>_numberOfComponents )
    throw MEDEXCEPTION(STRING(LOC)<<" invalid index" );

  return _componentsDescriptions[i-1];
}

/*!\ifnot MEDMEM_ug
  \todo
  Sets FIELD components UNIT.

  Duplicates the ComponentsUnits UNIT array to put components
  units in FIELD.
  ComponentsUnits size must be equal to number of components.
\endif
*/
inline void FIELD_::setComponentsUnits(const UNIT * ComponentsUnits)
{
  _componentsUnits.resize(_numberOfComponents);
  for (int i=0; i<_numberOfComponents; i++)
    _componentsUnits[i]=ComponentsUnits[i] ;
}
/*!\ifnot MEDMEM_ug
  Gets a reference to the UNIT array which contain the components units.

  This array size is equal to number of components
\endif
*/
inline const UNIT * FIELD_::getComponentsUnits() const
{
  return &(_componentsUnits[0]);
}
/*!\ifnot MEDMEM_ug
  Gets the UNIT of the i^th component.
\endif
*/
inline const UNIT * FIELD_::getComponentUnit(int i) const
{
  const char LOC[]= " FIELD_::getComponentUnit() : ";
  BEGIN_OF_MED(LOC);
  if( i<1 || i>_numberOfComponents )
    throw MEDEXCEPTION(STRING(LOC)<<" invalid index" );

  return &_componentsUnits[i-1] ;
}
/*!
  Sets FIELD components unit.

  Duplicates the MEDComponentsUnits string array to put components
  units in FIELD.
  MEDComponentsUnits size must be equal to number of components.

*/
inline void FIELD_::setMEDComponentsUnits(const string * MEDComponentsUnits)
{
  _MEDComponentsUnits.resize(_numberOfComponents);
  for (int i=0; i<_numberOfComponents; i++)
    _MEDComponentsUnits[i]=MEDComponentsUnits[i] ;
}
/*!\ifnot MEDMEM_ug
  Sets FIELD i^th component unit.

  i must be >=1 and <= number of components.
\endif
*/
inline void FIELD_::setMEDComponentUnit(int i, const string MEDComponentUnit)
{
  const char LOC[]= " FIELD_::setMEDComponentUnit() : ";
  BEGIN_OF_MED(LOC);
  if( i<1 || i>_numberOfComponents )
    throw MEDEXCEPTION(STRING(LOC)<<" invalid index" );

  _MEDComponentsUnits[i-1]=MEDComponentUnit ;
}
/*!
  Gets a reference to the string array which contain the components units.

  This array size is equal to number of components
*/
inline const string * FIELD_::getMEDComponentsUnits() const
{
  return &(_MEDComponentsUnits[0]);
}
/*! \ifnot MEDMEM_ug
  Gets the string for unit of the i^th component.
\endif
*/
inline string FIELD_::getMEDComponentUnit(int i) const
{
  const char LOC[]= " FIELD_::getMEDComponentUnit() : ";
  BEGIN_OF_MED(LOC);
  if( i<1 || i>_numberOfComponents )
    throw MEDEXCEPTION(STRING(LOC)<<" invalid index" );

  return _MEDComponentsUnits[i-1] ;
}
/*!
  Sets the iteration number where FIELD has been calculated.
*/
inline void FIELD_::setIterationNumber(int IterationNumber)
{
  _iterationNumber=IterationNumber;
}
/*!
  Gets the iteration number where FIELD has been calculated.
*/
inline int FIELD_::getIterationNumber() const
{
  return _iterationNumber ;
}
/*!
  Sets the time when FIELD has been calculated.
*/
inline void FIELD_::setTime(double Time)
{
  _time=Time ;
}
/*!
  Gets the time when FIELD has been calculated.
*/
inline double FIELD_::getTime() const
{
  return _time ;
}
/*!
  Sets the order number where FIELD has been calculated.

  It corresponds to internal iteration during one time step.
*/
inline void FIELD_::setOrderNumber(int OrderNumber)
{
  _orderNumber=OrderNumber ;
}
/*!
  Gets the order number where FIELD has been calculated.
*/
inline int FIELD_::getOrderNumber() const
{
  return _orderNumber ;
}
/*!
  Gets a reference to the SUPPORT object associated to FIELD.
*/
inline  const SUPPORT * FIELD_::getSupport() const
{
  return _support ;
}
/*!
  Sets the reference to the SUPPORT object associated to FIELD.

  Reference is not duplicate, so it must not be deleted.
*/
inline void FIELD_::setSupport(const SUPPORT * support)
{
  //A.G. Addings for RC
  if(_support!=support)
    {
      if(_support)
        _support->removeReference();
      _support = support ;
      if(_support)
        _support->addReference();
    }
}
/*!
  Gets the FIELD med value type (MED_INT32 or MED_REEL64).
*/
inline MED_EN::med_type_champ FIELD_::getValueType () const
{
  return _valueType ;
}

/*!
  Gets the FIELD med interlacing type (MED_FULL_INTERLACE or MED_NO_INTERLACE).
*/
  inline MED_EN::medModeSwitch FIELD_::getInterlacingType () const
{
  return _interlacingType ;
}
 /*!\if MEDMEM_ug @} \endif*/

/*!\if MEDMEM_ug 
\addtogroup FIELD_gauss 
@{ 
\endif */

/*!
 Determines whether the field stores several Gauss points per element.
*/
inline bool  FIELD_::getGaussPresence() const throw (MEDEXCEPTION)
  {
    const char LOC[]= "FIELD_::getGaussPresence() : ";
    throw MEDEXCEPTION(STRING(LOC) << " This FIELD_ doesn't rely on a FIELD<T>" );
  }

  /*!\if MEDMEM_ug @} \endif*/

} //End namespace MEDMEM

/////////////////////////
// END OF CLASS FIELD_ //
/////////////////////////

/*!

  This template class contains informations related with a FIELD :
  - Values of the field, their type (real or integer), the storage mode (full interlace,
    no interlace or no interlace by type).

*/


namespace MEDMEM {

  template<class T2> class MED_FIELD_RDONLY_DRIVER;
  template<class T2> class MED_FIELD_WRONLY_DRIVER;
  template<class T2> class VTK_FIELD_DRIVER;


  template <class T,
            class INTERLACING_TAG
            > class FIELD : public FIELD_
{
protected:

  typedef typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,NoGauss>::Array   ArrayNoGauss;
  typedef typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,Gauss>::Array     ArrayGauss;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array       ArrayNo;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array     ArrayFull;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlaceByType,NoGauss>::Array ArrayNoByType;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlaceByType,Gauss>::Array   ArrayNoByTypeGauss;
  typedef MEDMEM_Array_ Array;
  typedef T ElementType;
  typedef INTERLACING_TAG InterlacingTag;
  typedef map<MED_EN::medGeometryElement,GAUSS_LOCALIZATION_*> locMap;

  // array of value of type T
  Array *_value;

  // MESH, to be used for field reading from a file (if desired to link
  // to existing support instead of new support creation for the field)
  GMESH* _mesh;

  // extrema values
  T _vmin;
  T _vmax;

  map<MED_EN::medGeometryElement,GAUSS_LOCALIZATION_*> _gaussModel; //A changer quand les drivers seront template de l'entrelacement

  static T _scalarForPow;
  static T pow(T x);

private:
  void _operation(const FIELD& m,const FIELD& n, const char* Op);
  void _operationInitialize(const FIELD& m,const FIELD& n, const char* Op);
  void _add_in_place(const FIELD& m,const FIELD& n);
  void _sub_in_place(const FIELD& m,const FIELD& n);
  void _mul_in_place(const FIELD& m,const FIELD& n);
  void _div_in_place(const FIELD& m,const FIELD& n) throw (MEDEXCEPTION);
public:
  FIELD();
  FIELD(const FIELD &m);
  FIELD(const SUPPORT * Support, const int NumberOfComponents) throw (MEDEXCEPTION);
  FIELD(driverTypes driverType,
        const string & fileName, const string & fieldDriverName,
        const int iterationNumber=-1, const int orderNumber=-1,
        GMESH* mesh = 0)
    throw (MEDEXCEPTION);
  FIELD(const SUPPORT * Support, driverTypes driverType,
        const string & fileName="", const string & fieldName="",
        const int iterationNumber = -1, const int orderNumber = -1)
    throw (MEDEXCEPTION);
  ~FIELD();

public:
  FIELD & operator=(const FIELD &m);
        FIELD & operator=(T value);
  FIELD *operator+(const FIELD& m) const;
  FIELD *operator-(const FIELD& m) const;
  FIELD *operator*(const FIELD& m) const;
  FIELD *operator/(const FIELD& m) const;
  FIELD *operator-() const;
  FIELD& operator+=(const FIELD& m);
  FIELD& operator-=(const FIELD& m);
  FIELD& operator*=(const FIELD& m);
  FIELD& operator/=(const FIELD& m);

  void          applyLin(T a, T b, int icomp);
  static FIELD* add(const FIELD& m, const FIELD& n);
  static FIELD* addDeep(const FIELD& m, const FIELD& n);
  static FIELD* sub(const FIELD& m, const FIELD& n);
  static FIELD* subDeep(const FIELD& m, const FIELD& n);
  static FIELD* mul(const FIELD& m, const FIELD& n);
  static FIELD* mulDeep(const FIELD& m, const FIELD& n);
  static FIELD* div(const FIELD& m, const FIELD& n);
  static FIELD* divDeep(const FIELD& m, const FIELD& n);
  double normMax() const throw (MEDEXCEPTION);

  //------- TDG and BS addings

  void getMinMax(T &vmin, T &vmax) throw (MEDEXCEPTION);
  vector<int> getHistogram(int &nbint) throw (MEDEXCEPTION);
  FIELD<double>* buildGradient() const throw (MEDEXCEPTION);
  FIELD<double>* buildNorm2Field() const throw (MEDEXCEPTION);

  //-------------------

  double norm2() const throw (MEDEXCEPTION);
  void   applyLin(T a, T b);
  template <T T_function(T)> void applyFunc();
  void applyPow(T scalar);
  static FIELD* scalarProduct(const FIELD& m, const FIELD& n, bool deepCheck=false);
  double normL2(int component, const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
  double normL2(const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
  double normL1(int component, const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
  double normL1(const FIELD<double,FullInterlace> * p_field_volume=NULL) const;
  double integral(const SUPPORT *subSupport=NULL) const throw (MEDEXCEPTION);
  FIELD* extract(const SUPPORT *subSupport) const throw (MEDEXCEPTION);

  friend class MED_FIELD_RDONLY_DRIVER<T>;
  friend class MED_FIELD_WRONLY_DRIVER<T>;
  friend class VTK_FIELD_DRIVER<T>;

  void init ();
  void rmDriver(int index=0);
  int  addDriver(driverTypes driverType,
                 const string & fileName="Default File Name.med",
                 const string & driverFieldName="Default Field Name",
                 MED_EN::med_mode_acces access=MED_EN::RDWR) ;

  int  addDriver(GENDRIVER & driver);

  void allocValue(const int NumberOfComponents);
  void allocValue(const int NumberOfComponents, const int LengthValue);

  void deallocValue();

  inline void read(int index=0);
  inline void read(const GENDRIVER & genDriver);
  inline void read(driverTypes driverType, const std::string& filename);
  inline void write(int index=0);
  inline void write(const GENDRIVER &, MED_EN::med_mode_acces medMode=MED_EN::RDWR);
  inline void write(driverTypes        driverType,
                    const std::string& filename,
                    MED_EN::med_mode_acces medMode=MED_EN::RDWR);

  inline void writeAppend(int index=0, const string & driverName = "");
  inline void writeAppend(const GENDRIVER &);

  inline MEDMEM_Array_  * getArray()        const throw (MEDEXCEPTION);
  inline ArrayGauss     * getArrayGauss()   const throw (MEDEXCEPTION);
  inline ArrayNoGauss   * getArrayNoGauss() const throw (MEDEXCEPTION);
  inline bool            getGaussPresence() const throw (MEDEXCEPTION);

  inline int          getValueLength() const throw (MEDEXCEPTION);

  /*! \if MEDMEM_ug 
\addtogroup FIELD_value
@{
\endif  */
  /*! Returns a pointer to the value array.*/
  inline const T*     getValue()       const throw (MEDEXCEPTION);
  inline const T*     getRow(int i)    const throw (MEDEXCEPTION);
  inline const T*     getColumn(int j) const throw (MEDEXCEPTION);
/*!
  Returns the value of \f$ i^{th} \f$ element and \f$ j^{th}\f$ component.
  This method only works with fields having no particular Gauss point 
definition (i.e., fields having one value per element).
 This method makes the retrieval of the value independent from the
  interlacing pattern, but it is slower than the complete retrieval 
  obtained by the \b getValue() method.
*/

  inline T            getValueIJ(int i,int j) const throw (MEDEXCEPTION);

/*!
  Returns the \f$ j^{th}\f$  component of \f$ k^{th}\f$  Gauss points of \f$ i^{th}\f$  value.
  This method is compatible with elements having more than one Gauss point.
  This method makes the retrieval of the value independent from the
  interlacing pattern, but it is slower than the complete retrieval 
  obtained by the \b getValue() method.
*/
  inline T            getValueIJK(int i,int j,int k) const throw (MEDEXCEPTION);

  inline int          getValueByTypeLength(int t)                const throw (MEDEXCEPTION);
  inline const T*     getValueByType(int t)                      const throw (MEDEXCEPTION);
  inline T            getValueIJByType(int i,int j,int t)        const throw (MEDEXCEPTION);
  inline T            getValueIJKByType(int i,int j,int k,int t) const throw (MEDEXCEPTION);

  /*!
                The following example describes the creation of a FIELD.
                
                \example FIELDcreate.cxx

 \if MEDMEM_ug @} \endif */

  bool                getValueOnElement(int eltIdInSup,T* retValues) const throw (MEDEXCEPTION);
  void                getValueOnPoint(const double* coords, double* output) const throw (MEDEXCEPTION);
  void                getValueOnPoints(int nb_points, const double* coords, double* output) const throw (MEDEXCEPTION);

  const int   getNumberOfGeometricTypes() const throw (MEDEXCEPTION);
  const GAUSS_LOCALIZATION<INTERLACING_TAG> & getGaussLocalization(MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
  const GAUSS_LOCALIZATION<INTERLACING_TAG> * getGaussLocalizationPtr(MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
  const GAUSS_LOCALIZATION_* getGaussLocalizationRoot(MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
  void setGaussLocalization(MED_EN::medGeometryElement geomElement, const GAUSS_LOCALIZATION<INTERLACING_TAG> & gaussloc);
  void setGaussLocalization(MED_EN::medGeometryElement geomElement, GAUSS_LOCALIZATION_* gaussloc);
  const int * getNumberOfGaussPoints() const throw (MEDEXCEPTION);
  const int   getNumberOfGaussPoints( MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION);
  const int   getNbGaussI(int i)          const throw (MEDEXCEPTION);
  const int * getNumberOfElements()       const throw (MEDEXCEPTION);
  const MED_EN::medGeometryElement  * getGeometricTypes()  const throw (MEDEXCEPTION);
  bool        isOnAllElements()           const throw (MEDEXCEPTION);
 
  inline void setArray(MEDMEM_Array_ *value) throw (MEDEXCEPTION);

  FIELD<double, FullInterlace>* getGaussPointsCoordinates() const throw (MEDEXCEPTION);

  /*! \if MEDMEM_ug
 \addtogroup FIELD_value
@{
\endif
  */
/*!
This method makes it possible to have the field pointing to 
an existing value array. The ordering of the elements in the value array must 
conform to the MEDMEM ordering (I,K,J) : the outer loop is on the elements,
the intermediate loop is on the Gauss points, the inner loop is on 
the components. 
*/
  inline void setValue( T* value) throw (MEDEXCEPTION);
  inline void setRow( int i, T* value) throw (MEDEXCEPTION);
  inline void setColumn( int i, T* value) throw (MEDEXCEPTION);
/*!
  Sets the value of \f$ i^{th} \f$ element and \f$ j^{th}\f$ component with \a value.
*/
  inline void setValueIJ(int i, int j, T value) throw (MEDEXCEPTION);
  /*! \if MEDMEM_ug @} \endif */
  inline void setValueIJK(int i, int j, int k, T value) throw (MEDEXCEPTION);
  inline void setValueIJByType(int i, int j, int t, T value) throw (MEDEXCEPTION);
  inline void setValueIJKByType(int i, int j, int k, int t, T value) throw (MEDEXCEPTION);

  typedef void (*myFuncType)(const double *,T*);
  void fillFromAnalytic(myFuncType f) throw (MEDEXCEPTION);
  typedef void (*myFuncType2)(const T *,T*);
  FIELD<T,INTERLACING_TAG> *execFunc(int nbOfComponents, myFuncType2 f) throw (MEDEXCEPTION);
};
}

#include "MEDMEM_DriverFactory.hxx"

namespace MEDMEM {

template <class T,class INTERLACING_TAG> T FIELD<T, INTERLACING_TAG>::_scalarForPow=1;

// --------------------
// Implemented Methods
// --------------------

/*!
  Constructor with no parameter, most of the attribut members are set to NULL.
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>::FIELD(void)
  : FIELD_()
  {
    MESSAGE_MED("Constructeur FIELD sans parametre");

    //INITIALISATION DE _valueType DS LE CONSTRUCTEUR DE FIELD_
    ASSERT_MED(FIELD_::_valueType == MED_EN::MED_UNDEFINED_TYPE);
    FIELD_::_valueType=SET_VALUE_TYPE<T>::_valueType;

    //INITIALISATION DE _interlacingType DS LE CONSTRUCTEUR DE FIELD_
    ASSERT_MED(FIELD_::_interlacingType == MED_EN::MED_UNDEFINED_INTERLACE);
    FIELD_::_interlacingType=SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType;

    _value = ( ArrayNoGauss * ) NULL;

    _mesh  = ( MESH* ) NULL;
  }

        /*!
\addtogroup FIELD_constructors FIELD<T> constructors
@{
        */

/*!
  Constructor that allocates the value array with the dimensions provided by
\a NumberOfComponents and the dimension of \a Support. The value array is
 allocated but not initialized.
This constructor does not allow the creation of fields with Gauss points. 
\param Support support on which the field lies
\param NumberOfComponents number of components of the variable stored. For instance, 
it will be 3 for a (vx,vy,vz) vector.

\code
FIELD<double> field (support, 3);
int nbelem = support->getNumberOfElements(MED_ALL_ELEMENTS);
for (int i=1; i<=nbelem; i++)
   for (j=1; j<=3;j++)
       field->setValueIJ(i,j,0.0);
\endcode
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>::FIELD(const SUPPORT * Support,
                                 const int NumberOfComponents) throw (MEDEXCEPTION) :
  FIELD_(Support, NumberOfComponents),_value(NULL)
{
  const char LOC[]= "FIELD<T>::FIELD(const SUPPORT * Support, const int NumberOfComponents)";
  BEGIN_OF_MED(LOC);
  SCRUTE_MED(this);

  //INITIALISATION DE _valueType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT_MED(FIELD_::_valueType == MED_EN::MED_UNDEFINED_TYPE)
    FIELD_::_valueType=SET_VALUE_TYPE<T>::_valueType;

  //INITIALISATION DE _interlacingType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT_MED(FIELD_::_interlacingType == MED_EN::MED_UNDEFINED_INTERLACE)
    FIELD_::_interlacingType=SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType;

  try
    {
      // becarefull about the numbre of gauss point
      _numberOfValues = Support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
    }
#if defined(_DEBUG_) || defined(_DEBUG)
  catch (MEDEXCEPTION &ex)
#else
  catch (MEDEXCEPTION )
#endif
    {
      MESSAGE_MED("No value defined ! ("<<ex.what()<<")");
    }
  MESSAGE_MED("FIELD : constructeur : "<< _numberOfValues <<" et "<< NumberOfComponents);
  if ( _numberOfValues > 0 )
    {
      if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE )
        {
          const int * nbelgeo = Support->getNumberOfElements();
          vector<int> nbelgeoc( Support->getNumberOfTypes() + 1 );
          nbelgeoc[0] = 0;
          for ( int t = 1; t < (int)nbelgeoc.size(); ++t )
            nbelgeoc[t] = nbelgeoc[t-1] + nbelgeo[t-1];
          _value = new ArrayNoByType (_numberOfComponents,_numberOfValues,
                                      Support->getNumberOfTypes(), &nbelgeoc[0]);
        }
      else
        {
          _value = new ArrayNoGauss (_numberOfComponents,_numberOfValues);
        }
      _isRead = true ;
    }
  _mesh  = ( MESH* ) NULL;

  END_OF_MED(LOC);
}

        /*!
@}
        */
/*!
  \if developper
  \endif
*/
template <class T, class INTERLACING_TAG> void FIELD<T, INTERLACING_TAG>::init ()
{
}

/*!
  Copy constructor.
*/
template <class T, class INTERLACING_TAG> FIELD<T, INTERLACING_TAG>::FIELD(const FIELD & m):
  FIELD_(m)
{
  MESSAGE_MED("Constructeur FIELD de recopie");

  // RECOPIE PROFONDE <> de l'operateur= Rmq from EF
  if (m._value != NULL)
    {
      if ( m.getGaussPresence() )
        _value = new ArrayGauss( *(static_cast< ArrayGauss * > (m._value) ) ,false);
      else
        _value = new ArrayNoGauss( *(static_cast< ArrayNoGauss * > (m._value)) ,false);
    }
  else
    _value = (ArrayNoGauss *) NULL;
  locMap::const_iterator it;
  for ( it = m._gaussModel.begin();it != m._gaussModel.end(); it++ )
    _gaussModel[static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> * > ((*it).second)->getType()]=
      new GAUSS_LOCALIZATION<INTERLACING_TAG>(
                                              *static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> * > ( (*it).second )
                                              );

  _valueType       = m._valueType;
  _interlacingType = m._interlacingType;
  //drivers = m._drivers;
  _mesh            = m._mesh;
  if(_mesh)
    _mesh->addReference();
}

/*!
  \if developper
  Not implemented.
  \endif
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG> & FIELD<T, INTERLACING_TAG>::operator=(const FIELD &m)
{
  MESSAGE_MED("Appel de FIELD<T>::operator=") ;
  if ( this == &m) return *this;

  // copy values array
  FIELD_::operator=(m);  // Driver are ignored & ?copie su pointeur de Support?

  _value           = m._value; //PROBLEME RECOPIE DES POINTEURS PAS COHERENT AVEC LE
                               //CONSTRUCTEUR PAR RECOPIE
                               //CF :Commentaire dans MEDMEM_Array
  locMap::const_iterator it;
  for ( it = m._gaussModel.begin();it != m._gaussModel.end(); it++ )
    _gaussModel[static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> * > ((*it).second)->getType()]=
      new GAUSS_LOCALIZATION<INTERLACING_TAG>(
                                              *static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> * > ( (*it).second )
                                              );

  _valueType       = m._valueType;
  _interlacingType = m._interlacingType;
  if(_mesh!=m._mesh)
    {
      if(_mesh)
        _mesh->removeReference();
      _mesh = m._mesh;
      if(_mesh)
        _mesh->addReference();
    }
  return *this;
}

/*!
        Initializes all the field values to \a value 
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG> & FIELD<T, INTERLACING_TAG>::operator=(T value)
{
  MESSAGE_MED("Appel de FIELD<T>::operator= T") ;
        int size=getNumberOfComponents()*getNumberOfValues();
        T* ptr= const_cast<T*>( getValue());
        for (int i=0; i< size; i++)
                {*ptr++=value;}

  return *this;
}

/*!\addtogroup FIELD_algo 
@{
*/

/*!
     Overload addition operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:\n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name,
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a + b; </tt> \n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator + in associatives expressions like
     <tt> a = b + c + d +e; </tt> \n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG> *FIELD<T, INTERLACING_TAG>::operator+(const FIELD & m) const
{
  const char* LOC = "FIELD<T>::operator+(const FIELD & m)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T, INTERLACING_TAG> *result=new FIELD<T, INTERLACING_TAG>(this->getSupport(),this->getNumberOfComponents());
    result->_operationInitialize(*this,m,"+"); // perform Atribute's initialization
    result->_add_in_place(*this,m); // perform addition

  END_OF_MED(LOC);
    return result;
}

/*!  Overloaded Operator +=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>& FIELD<T, INTERLACING_TAG>::operator+=(const FIELD & m)
{
  const char* LOC = "FIELD<T>::operator+=(const FIELD & m)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    const T* value1=m.getValue(); // get pointers to the values we are adding

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value
    for(;value!=endV; value1++,value++)
        *value += *value1;
  END_OF_MED(LOC);
    return *this;
}


/*! Addition of fields. Static member function.
 *  The function return a pointer to a new created field that holds the addition.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::add(const FIELD& m, const FIELD& n)
{
  const char* LOC = "FIELD<T>::add(const FIELD & m, const FIELD& n)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(m, n); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
                                                                      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"+"); // perform Atribute's initialization
    result->_add_in_place(m,n); // perform addition

  END_OF_MED(LOC);
    return result;
}

/*! Same as add method except that field check is deeper.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::addDeep(const FIELD& m, const FIELD& n)
{
  const char* LOC = "FIELD<T>::addDeep(const FIELD & m, const FIELD& n)";
  BEGIN_OF_MED(LOC);
    FIELD_::_deepCheckFieldCompatibility(m, n); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
                                                                      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"+"); // perform Atribute's initialization
    result->_add_in_place(m,n); // perform addition

  END_OF_MED(LOC);
    return result;
}

/*!
     Overload substraction operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:\n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name,
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a - b; </tt> \n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator - in associatives expressions like
     <tt> a = b - c - d -e; </tt> \n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG> *FIELD<T, INTERLACING_TAG>::operator-(const FIELD & m) const
{
  const char* LOC = "FIELD<T>::operator-(const FIELD & m)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T, INTERLACING_TAG> *result=new FIELD<T, INTERLACING_TAG>(this->getSupport(),this->getNumberOfComponents());
    result->_operationInitialize(*this,m,"-"); // perform Atribute's initialization
    result->_sub_in_place(*this,m); // perform substracion

  END_OF_MED(LOC);
    return result;
}

template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG> *FIELD<T, INTERLACING_TAG>::operator-() const
{
  const char* LOC = "FIELD<T>::operator-()";
  BEGIN_OF_MED(LOC);

    // Creation of the result - memory is allocated by FIELD constructor
  FIELD<T, INTERLACING_TAG> *result=new FIELD<T, INTERLACING_TAG>(this->getSupport(),this->getNumberOfComponents());
    // Atribute's initialization
    result->setName("- "+getName());
    result->setComponentsNames(getComponentsNames());
    // not yet implemented    setComponentType(getComponentType());
    result->setComponentsDescriptions(getComponentsDescriptions());
    result->setMEDComponentsUnits(getMEDComponentsUnits());
    result->setComponentsUnits(getComponentsUnits());
    result->setIterationNumber(getIterationNumber());
    result->setTime(getTime());
    result->setOrderNumber(getOrderNumber());

    const T* value1=getValue();
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (result->getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
        *value = -(*value1);
  END_OF_MED(LOC);
    return result;
}

/*!  Overloaded Operator -=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>& FIELD<T, INTERLACING_TAG>::operator-=(const FIELD & m)
{
  const char* LOC = "FIELD<T>::operator-=(const FIELD & m)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(*this, m); // may throw exception

    const T* value1=m.getValue();

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
        *value -= *value1;

  END_OF_MED(LOC);
    return *this;
}



/*!  Apply to a given field component the linear function x -> ax+b.
 *   calculation is done "in place".
 */
template <class T, class INTERLACIN_TAG> void FIELD<T, INTERLACIN_TAG>::applyLin(T a, T b, int icomp)
{
    // get a non const pointer to the inside array of values and perform operation in place
    T * value=const_cast<T *> (getValue());
         
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array

    if (size>0) // for a negative size, there is nothing to do
    {
                        value+=icomp-1;
                        const T* lastvalue=value+size; // pointer to the end of value
 
                        for(;value!=lastvalue; value+=getNumberOfComponents()) // apply linear transformation
                                *value = a*(*value)+b;
    }
}

/*! Substraction of fields. Static member function.
 *  The function return a pointer to a new created field that holds the substraction.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::sub(const FIELD& m, const FIELD& n)
{
  const char* LOC = "FIELD<T>::sub(const FIELD & m, const FIELD& n)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(m, n); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
                                                                      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"-"); // perform Atribute's initialization
    result->_sub_in_place(m,n); // perform substraction

  END_OF_MED(LOC);
    return result;
}

/*! Same as sub method except that field check is deeper.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::subDeep(const FIELD& m, const FIELD& n)
{
  const char* LOC = "FIELD<T>::subDeep(const FIELD & m, const FIELD& n)";
  BEGIN_OF_MED(LOC);
    FIELD_::_deepCheckFieldCompatibility(m, n); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
                                                                      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"-"); // perform Atribute's initialization
    result->_sub_in_place(m,n); // perform substraction

  END_OF_MED(LOC);
    return result;
}

/*!
     Overload multiplication operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:\n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name,
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a * b; </tt> \n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator * in associatives expressions like
     <tt> a = b * c * d *e; </tt> \n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG> *FIELD<T, INTERLACING_TAG>::operator*(const FIELD & m) const
{
  const char* LOC = "FIELD<T>::operator*(const FIELD & m)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(*this, m, false); // may throw exception

    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T, INTERLACING_TAG> *result=new FIELD<T, INTERLACING_TAG>(this->getSupport(),this->getNumberOfComponents());
    result->_operationInitialize(*this,m,"*"); // perform Atribute's initialization
    result->_mul_in_place(*this,m); // perform multiplication

  END_OF_MED(LOC);
    return result;
}

/*!  Overloaded Operator *=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>& FIELD<T, INTERLACING_TAG>::operator*=(const FIELD & m)
{
  const char* LOC = "FIELD<T>::operator*=(const FIELD & m)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(*this, m, false); // may throw exception

    const T* value1=m.getValue();

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
        *value *= *value1;

  END_OF_MED(LOC);
    return *this;
}


/*! Multiplication of fields. Static member function.
 *  The function return a pointer to a new created field that holds the multiplication.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::mul(const FIELD& m, const FIELD& n)
{
  const char* LOC = "FIELD<T>::mul(const FIELD & m, const FIELD& n)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(m, n, false); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
                                                                      m.getNumberOfComponents());
    result->_operationInitialize(m,n,"*"); // perform Atribute's initialization
    result->_mul_in_place(m,n); // perform multiplication

  END_OF_MED(LOC);
    return result;
}

/*! Same as mul method except that field check is deeper.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::mulDeep(const FIELD& m, const FIELD& n)
{
  const char* LOC = "FIELD<T>::mulDeep(const FIELD & m, const FIELD& n)";
  BEGIN_OF_MED(LOC);
    FIELD_::_deepCheckFieldCompatibility(m, n, false); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T,INTERLACING_TAG>(m.getSupport(),
                                                                     m.getNumberOfComponents());
    result->_operationInitialize(m,n,"*"); // perform Atribute's initialization
    result->_mul_in_place(m,n); // perform multiplication

  END_OF_MED(LOC);
    return result;
}

/*!
     Overload division operator.
     This operation is authorized only for compatible fields that have the same support.
     The compatibility checking includes equality tests of the folowing data members:\n
     - _support
     - _numberOfComponents
     - _numberOfValues
     - _componentsTypes
     - _MEDComponentsUnits.

     The data members of the returned field are initialized, based on the first field, except for the name,
     which is the combination of the two field's names and the operator.
     Advised Utilisation in C++ : <tt> FIELD<T> c = a / b; </tt> \n
     In this case, the (recent) compilators perform optimisation and don't call the copy constructor.
     When using python, this operator calls the copy constructor in any case.
     The user has to be aware that when using operator / in associatives expressions like
     <tt> a = b / c / d /e; </tt> \n
     no optimisation is performed : the evaluation of last expression requires the construction of
     3 temporary fields.
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG> *FIELD<T, INTERLACING_TAG>::operator/(const FIELD & m) const
{
  const char* LOC = "FIELD<T>::operator/(const FIELD & m)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(*this, m, false); // may throw exception

    // Creation of the result - memory is allocated by FIELD constructor
    FIELD<T, INTERLACING_TAG> *result=new FIELD<T, INTERLACING_TAG>(this->getSupport(),this->getNumberOfComponents());
    try
      {
        result->_operationInitialize(*this,m,"/"); // perform Atribute's initialization
        result->_div_in_place(*this,m); // perform division
      }
    catch(MEDEXCEPTION& e)
      {
        result->removeReference();
        throw e;
      }

  END_OF_MED(LOC);
    return result;
}


/*!  Overloaded Operator /=
 *   Operations are directly performed in the first field's array.
 *   This operation is authorized only for compatible fields that have the same support.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>& FIELD<T, INTERLACING_TAG>::operator/=(const FIELD & m)
{
  const char* LOC = "FIELD<T>::operator/=(const FIELD & m)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(*this, m, false); // may throw exception

    const T* value1=m.getValue(); // get pointers to the values we are adding

    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array
    const T* endV=value+size; // pointer to the end of value

    for(;value!=endV; value1++,value++)
        *value /= *value1;

  END_OF_MED(LOC);
    return *this;
}


/*! Division of fields. Static member function.
 *  The function return a pointer to a new created field that holds the division.
 *  Data members are checked for compatibility and initialized.
 *  The user is in charge of memory deallocation.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::div(const FIELD& m, const FIELD& n)
{
  const char* LOC = "FIELD<T>::div(const FIELD & m, const FIELD& n)";
  BEGIN_OF_MED(LOC);
    FIELD_::_checkFieldCompatibility(m, n, false); // may throw exception

    // Creation of a new field
    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
                                                                      m.getNumberOfComponents());
    try
      {
        result->_operationInitialize(m,n,"/"); // perform Atribute's initialization
        result->_div_in_place(m,n); // perform division
      }
    catch(MEDEXCEPTION& e)
      {
        result->removeReference();
        throw e;
      }
  END_OF_MED(LOC);
    return result;
}

/*! Same as div method except that field check is deeper.
 */
template <class T,class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::divDeep(const FIELD& m, const FIELD& n)
{
  const char* LOC = "FIELD<T>::divDeep(const FIELD & m, const FIELD& n)";
  BEGIN_OF_MED(LOC);
  FIELD_::_deepCheckFieldCompatibility(m, n, false); // may throw exception

  // Creation of a new field
  FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),
                                                                    m.getNumberOfComponents());
  try
    {
      result->_operationInitialize(m,n,"/"); // perform Atribute's initialization
      result->_div_in_place(m,n); // perform division
    }
  catch(MEDEXCEPTION& e)
      {
        result->removeReference();
        throw e;
      }
  END_OF_MED(LOC);
  return result;
}


/*! 
@}
*/

/*!
  \if developper
  This internal method initialize the members of a new field created to hold the result of the operation Op .
  Initialization is based on the first field, except for the name, which is the combination of the two field's names
  and the operator.
  \endif
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::_operationInitialize(const FIELD& m,const FIELD& n, const char* Op)
{
    MESSAGE_MED("Appel methode interne " << Op);

    // Atribute's initialization (copy of the first field's attributes)
    // Other data members (_support, _numberOfValues) are initialized in the field's constr.
    setName(m.getName()+" "+Op+" "+n.getName());
    setComponentsNames(m.getComponentsNames());
    setComponentsDescriptions(m.getComponentsDescriptions());
    setMEDComponentsUnits(m.getMEDComponentsUnits());

    // The following data member may differ from field m to n.
    // The initialization is done based on the first field.

    setComponentsUnits(m.getComponentsUnits());

    setIterationNumber(m.getIterationNumber());
    setTime(m.getTime());
    setOrderNumber(m.getOrderNumber());
}


/*!
  \if developper
  Internal method called by FIELD<T>::operator+ and FIELD<T>::add to perform addition "in place".
  This method is applied to a just created field with medModeSwitch mode.
  For this reason, the alternate mode doesn't need to be set to 0 after performing operation :
  it doesn't exist!
  \endif
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::_add_in_place(const FIELD& m,const FIELD& n)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue();
    const T* value2=n.getValue();
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());

    const int size=getNumberOfValues()*getNumberOfComponents();
    SCRUTE_MED(size);
    const T* endV1=value1+size;
    for(;value1!=endV1; value1++,value2++,value++)
        *value=(*value1)+(*value2);
}

/*!
  \if developper
  Internal method called by FIELD<T>::operator- and FIELD<T>::sub to perform substraction "in place".
  This method is applied to a just created field with medModeSwitch mode.
  For this reason, the alternate mode doesn't need to be set to 0 after performing operation :
  it doesn't exist!
  \endif
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::_sub_in_place(const FIELD& m,const FIELD& n)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue();
    const T* value2=n.getValue();
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());

    const int size=getNumberOfValues()*getNumberOfComponents();
    SCRUTE_MED(size);
    const T* endV1=value1+size;
    for(;value1!=endV1; value1++,value2++,value++)
        *value=(*value1)-(*value2);
}

/*!
  \if developper
  Internal method called by FIELD<T>::operator* and FIELD<T>::mul to perform multiplication "in place".
  This method is applied to a just created field with medModeSwitch mode.
  For this reason, the alternate mode doesn't need to be set to 0 after performing operation :
  it doesn't exist!
  \endif
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::_mul_in_place(const FIELD& m,const FIELD& n)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue();
    const T* value2=n.getValue();
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());

    const int size=getNumberOfValues()*getNumberOfComponents();
    SCRUTE_MED(size);
    const T* endV1=value1+size;
    for(;value1!=endV1; value1++,value2++,value++)
        *value=(*value1)*(*value2);
}

/*!
  \if developper
  Internal method called by FIELD<T>::operator/ and FIELD<T>::div to perform division "in place".
  This method is applied to a just created field with medModeSwitch mode.
  For this reason, the alternate mode doesn't need to be set to 0 after performing operation :
  it doesn't exist!
  \endif
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::_div_in_place(const FIELD& m,const FIELD& n) throw (MEDEXCEPTION)
{
    // get pointers to the values we are adding
    const T* value1=m.getValue();
    const T* value2=n.getValue();
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());

    const int size=getNumberOfValues()*getNumberOfComponents();
    SCRUTE_MED(size);
    const T* endV1=value1+size;
    for(;value1!=endV1; value1++,value2++,value++){
      if ( *value2 == 0 ) { // FAIRE PLUTOT UN TRY CATCH Rmq from EF
          string diagnosis;
          diagnosis="FIELD<T,INTERLACING_TAG>::_div_in_place(...) : Divide by zero !";
          throw MEDEXCEPTION(diagnosis.c_str());
        }
        *value=(*value1)/(*value2);
    }
}

/*!
\addtogroup FIELD_algo
@{
*/

/*!  Return maximum of all absolute values contained in the array (all elements and all components are browsed).
 */
template <class T, class INTERLACIN_TAG> double FIELD<T, INTERLACIN_TAG>::normMax() const throw (MEDEXCEPTION)
{
    const T* value=getValue(); // get pointer to the values
    const int size=getNumberOfValues()*getNumberOfComponents();
    if (size <= 0) // Size of array has to be strictly positive
    {
        string diagnosis;
        diagnosis="FIELD<T,INTERLACIN_TAG>::normMax() : cannot compute the norm of "+getName()+
            " : it size is non positive!";
        throw MEDEXCEPTION(diagnosis.c_str());
    }
    const T* lastvalue=value+size; // get pointer just after last value
    const T* pMax=value; // pointer to the max value
    const T* pMin=value; // pointer to the min value

    // get pointers to the max & min value of array
    while ( ++value != lastvalue )
    {
        if ( *pMin > *value )
            pMin=value;
        if ( *pMax < *value )
            pMax=value;
    }

    T Max= *pMax>(T) 0 ? *pMax : -*pMax; // Max=abs(*pMax)
    T Min= *pMin>(T) 0 ? *pMin : -*pMin; // Min=abs(*pMin)

    return Max>Min ? double(Max) : double(Min);
}

/*!  Return Euclidian norm for all elements of the array.
 */
template <class T, class INTERLACIN_TAG> double FIELD<T, INTERLACIN_TAG>::norm2() const throw (MEDEXCEPTION)
{
    const T* value=this->getValue(); // get const pointer to the values
    const int size=getNumberOfValues()*getNumberOfComponents(); // get size of array
    if (size <= 0) // Size of array has to be strictly positive
    {
        string diagnosis;
        diagnosis="FIELD<T,INTERLACIN_TAG>::norm2() : cannot compute the norm of "+getName()+
            " : it size is non positive!";
        throw MEDEXCEPTION(diagnosis.c_str());
    }
    const T* lastvalue=value+size; // point just after last value

    T result((T)0); // init
    for( ; value!=lastvalue ; ++value)
        result += (*value) * (*value);

    return std::sqrt(double(result));
}


//------------- TDG and BS addings 

/*!  Return Extrema of field
 */
 template <class T, class INTERLACIN_TAG> void FIELD<T, INTERLACIN_TAG>::getMinMax(T &vmin, T &vmax) throw (MEDEXCEPTION)
{
  const T* value=getValue(); // get pointer to the values
  const int size=getNumberOfValues()*getNumberOfComponents();
  const T* lastvalue=value+size; // point just after last value
    
  if (size <= 0){ // Size of array has to be strictly positive
      
    string diagnosis;
    diagnosis="FIELD<T,INTERLACIN_TAG>::getMinMax() : cannot compute the extremums of "+getName()+
      " : its size is non positive!";
    throw MEDEXCEPTION(diagnosis.c_str());
  }
    
  if (!_isMinMax){
    vmax=MinMax<T>::getMin(); // init a max value
    vmin=MinMax<T>::getMax(); // init a min value
      
    for( ; value!=lastvalue ; ++value){
      if ( vmin > *value )
        vmin=*value;
      if ( vmax < *value )
        vmax=*value;
    }
    _isMinMax=true;
    _vmin=vmin;
    _vmax=vmax;
  }
  else{
    vmin = _vmin;
    vmax = _vmax;
  }

}

/*!  Return Histogram of field
 */
 template <class T, class INTERLACIN_TAG> vector<int> FIELD<T, INTERLACIN_TAG>::getHistogram(int &nbint) throw (MEDEXCEPTION)
{
  const T* value=getValue(); // get pointer to the values
  const int size=getNumberOfValues()*getNumberOfComponents();
  const T* lastvalue=value+size; // point just after last value

  if (size <= 0){ // Size of array has to be strictly positive

    string diagnosis;
    diagnosis="FIELD<T,INTERLACIN_TAG>::getHistogram() : cannot compute the histogram of "+getName()+
      " : it size is non positive!";
    throw MEDEXCEPTION(diagnosis.c_str());
  }

  vector<int> Histogram(nbint) ;
  T vmin,vmax;
  int j;

  for( j=0 ; j!=nbint ; j++) Histogram[j]=0 ;
    
  getMinMax(vmin,vmax);
  for( ; value!=lastvalue ; ++value){
    if(*value==vmax) j = nbint-1;
    else j = (int)(((double)nbint * (*value-vmin))/(vmax-vmin));
    Histogram[j]+=1 ;
  }

  return Histogram ;

}

/*!  Return vectorial gradient field
 */
template <class T, class INTERLACIN_TAG> 
FIELD<double, FullInterlace>* FIELD<T, INTERLACIN_TAG>::buildGradient() const throw (MEDEXCEPTION)
{
  const char LOC[]= "FIELD<T, INTERLACIN_TAG>::buildGradient() : ";
  BEGIN_OF_MED(LOC);

  // space dimension of input mesh
  int spaceDim = getSupport()->getMesh()->getSpaceDimension();
  double *x = new double[spaceDim];

  FIELD<double, FullInterlace>* Gradient =
    new FIELD<double, FullInterlace>(getSupport(),spaceDim);

  string name("gradient of ");
  name += getName();
  Gradient->setName(name);
  string descr("gradient of ");
  descr += getDescription();
  Gradient->setDescription(descr);

  if( _numberOfComponents > 1 ){
    delete Gradient;
    delete [] x;
    throw MEDEXCEPTION("gradient calculation only on scalar field");
  }

  for(int i=1;i<=spaceDim;i++){
    string nameC("gradient of ");
    nameC += getName();
    Gradient->setComponentName(i,nameC);
    Gradient->setComponentDescription(i,"gradient");
    string MEDComponentUnit = getMEDComponentUnit(1)+getSupport()->getMesh()->getCoordinatesUnits()[i-1];
    Gradient->setMEDComponentUnit(i,MEDComponentUnit);
  }

  Gradient->setIterationNumber(getIterationNumber());
  Gradient->setOrderNumber(getOrderNumber());
  Gradient->setTime(getTime());

  // typ of entity on what is field
  MED_EN::medEntityMesh typ = getSupport()->getEntity();

  const int *C;
  const int *iC;  
  const int *revC;
  const int *indC;
  const double *coord;
  int NumberOf;

  switch (typ) {
  case MED_EN::MED_CELL:
  case MED_EN::MED_FACE:
  case MED_EN::MED_EDGE:
    {
      // read connectivity array to have the list of nodes contained by an element
      C = getSupport()->getMesh()->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,typ,MED_ALL_ELEMENTS);
      iC = getSupport()->getMesh()->getConnectivityIndex(MED_EN::MED_NODAL,typ);
      // calculate reverse connectivity to have the list of elements which contains node i
      revC = getSupport()->getMesh()->getReverseConnectivity(MED_EN::MED_NODAL,typ);
      indC = getSupport()->getMesh()->getReverseConnectivityIndex(MED_EN::MED_NODAL,typ);
      // coordinates of each node
      coord = getSupport()->getMesh()->getCoordinates(MED_EN::MED_FULL_INTERLACE);
      // number of elements
      NumberOf = getSupport()->getNumberOfElements(MED_ALL_ELEMENTS);
      // barycenter field of elements
      FIELD<double, FullInterlace>* barycenter = getSupport()->getMesh()->getBarycenter(getSupport());

      // calculate gradient vector for each element i
      for (int i = 1; i < NumberOf + 1; i++) {

        // listElements contains elements which contains a node of element i
        set <int> listElements;
        set <int>::iterator elemIt;
        listElements.clear();

        // for each node j of element i
        for (int ij = iC[i-1]; ij < iC[i]; ij++) {
          int j = C[ij-1];
          for (int k = indC[j-1]; k < indC[j]; k++) {
            // c element contains node j
            int c = revC[k-1];
            // we put the elements in set
            if (c != i)
              listElements.insert(c);
          }
        }
        // coordinates of barycentre of element i in space of dimension spaceDim
        for (int j = 0; j < spaceDim; j++)
          x[j] = barycenter->getValueIJ(i,j+1);

        for (int j = 0; j < spaceDim; j++) {
          // value of field of element i
          double val = getValueIJ(i,1);
          double grad = 0.;
          // calculate gradient for each neighbor element
          for (elemIt = listElements.begin(); elemIt != listElements.end(); elemIt++) {
            int elem = *elemIt;
            double d2 = 0.;
            for (int l = 0; l < spaceDim; l++) {
              // coordinate of barycenter of element elem
              double xx = barycenter->getValueIJ(elem, l+1);
              d2 += (x[l]-xx) * (x[l]-xx);
            }
            grad += (barycenter->getValueIJ(elem,j+1)-x[j])*(getValueIJ(elem,1)-val)/sqrt(d2);
          }
          if (listElements.size() != 0) grad /= listElements.size();
          Gradient->setValueIJ(i,j+1,grad);
        }
      }
      delete barycenter;
    }
    break;
  case MED_EN::MED_NODE:
    // read connectivity array to have the list of nodes contained by an element
    C = getSupport()->getMesh()->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_ALL_ELEMENTS);
    iC = getSupport()->getMesh()->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
    // calculate reverse connectivity to have the list of elements which contains node i
    revC=getSupport()->getMesh()->getReverseConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL);
    indC=getSupport()->getMesh()->getReverseConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
    // coordinates of each node
    coord = getSupport()->getMesh()->getCoordinates(MED_EN::MED_FULL_INTERLACE);

    // calculate gradient for each node
    NumberOf = getSupport()->getNumberOfElements(MED_ALL_ELEMENTS);
    for (int i=1; i<NumberOf+1; i++){
      // listNodes contains nodes neigbor of node i 
      set <int> listNodes;
      set <int>::iterator nodeIt ;
      listNodes.clear();
      for(int j=indC[i-1];j<indC[i];j++){
        // c element contains node i
        int c=revC[j-1];
        // we put the nodes of c element in set
        for(int k=iC[c-1];k<iC[c];k++)
          if(C[k-1] != i)
            listNodes.insert(C[k-1]);
      }
      // coordinates of node i in space of dimension spaceDim
      for(int j=0;j<spaceDim;j++)
        x[j] = coord[(i-1)*spaceDim+j];
      
      for(int j=0;j<spaceDim;j++){
        // value of field
        double val = getValueIJ(i,1);
        double grad = 0.;
        // calculate gradient for each neighbor node
        for(nodeIt=listNodes.begin();nodeIt!=listNodes.end();nodeIt++){
          int node = *nodeIt;
          double d2 = 0.;
          for(int l=0;l<spaceDim;l++){
            double xx = coord[(node-1)*spaceDim+l];
            d2 += (x[l]-xx) * (x[l]-xx);
          }
          grad += (coord[(node-1)*spaceDim+j]-x[j])*(getValueIJ(node,1)-val)/sqrt(d2);
        }
        if(listNodes.size() != 0) grad /= listNodes.size();
        Gradient->setValueIJ(i,j+1,grad);
      }
    }
    break;
  case MED_ALL_ENTITIES:
    delete [] x;
    delete Gradient;
    throw MEDEXCEPTION("gradient calculation not yet implemented on all elements");
    break;
  }

  delete [] x;

  END_OF_MED(LOC);
  return Gradient;
}

/*!  Return scalar norm2 field
 */
template <class T, class INTERLACIN_TAG> 
FIELD<double, FullInterlace>* FIELD<T, INTERLACIN_TAG>::buildNorm2Field() const throw (MEDEXCEPTION)
{
  const char LOC[]= "FIELD<T, INTERLACIN_TAG>::buildNorm2Field() : ";
  BEGIN_OF_MED(LOC);

  FIELD<double, FullInterlace>* Norm2Field =
    new FIELD<double, FullInterlace>(getSupport(),1);

  string name("norm2 of ");
  name += getName();
  Norm2Field->setName(name);
  string descr("norm2 of ");
  descr += getDescription();
  Norm2Field->setDescription(descr);

  string nameC("norm2 of ");
  nameC += getName();
  Norm2Field->setComponentName(1,nameC);
  Norm2Field->setComponentDescription(1,"norm2");
  string MEDComponentUnit = getMEDComponentUnit(1);
  Norm2Field->setMEDComponentUnit(1,MEDComponentUnit);

  Norm2Field->setIterationNumber(getIterationNumber());
  Norm2Field->setOrderNumber(getOrderNumber());
  Norm2Field->setTime(getTime());

  // calculate nom2 for each element
  int NumberOf = getSupport()->getNumberOfElements(MED_ALL_ELEMENTS);
  for (int i=1; i<NumberOf+1; i++){
    double norm2 = 0.;
    for(int j=1;j<=getNumberOfComponents();j++)
      norm2 += getValueIJ(i,j)*getValueIJ(i,j);
    Norm2Field->setValueIJ(i,1,sqrt(norm2));
  }

  END_OF_MED(LOC);
  return Norm2Field;

}

/*!  Apply to each (scalar) field component the template parameter T_function,
 *   which is a pointer to function.
 *   Since the pointer is known at compile time, the function is inlined into the inner loop!
 *   calculation is done "in place".
 *   Use examples :
 *
 *   \code  myField.applyFunc<std::sqrt>();  // apply sqare root function \endcode
 *     \code myField.applyFunc<myFunction>(); // apply your own created function \endcode
 */
template <class T, class INTERLACIN_TAG> template <T T_function(T)>
void FIELD<T, INTERLACIN_TAG>::applyFunc()
{
  // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array

    if (size>0) // for a negative size, there is nothing to do
    {
        const T* lastvalue=value+size; // pointer to the end of value
        for(;value!=lastvalue; ++value) // apply linear transformation
            *value = T_function(*value);
    }
}

template <class T, class INTERLACIN_TAG> T FIELD<T, INTERLACIN_TAG>::pow(T x)
{
  return (T)::pow((double)x,FIELD<T, INTERLACIN_TAG>::_scalarForPow);
}

/*!  Apply to each (scalar) field component the math function pow.
 *   calculation is done "in place".
 *   Use examples :
 *
 *   \code  myField.applyFunc<std::sqrt>();  // apply sqare root function \endcode
 *     \code myField.applyFunc<myFunction>(); // apply your own created function \endcode
 */
template <class T, class INTERLACIN_TAG> void FIELD<T, INTERLACIN_TAG>::applyPow(T scalar)
{
  FIELD<T, INTERLACIN_TAG>::_scalarForPow=scalar;
  applyFunc<FIELD<T, INTERLACIN_TAG>::pow>();
}

/*!  Apply to each (scalar) field component the linear function x -> ax+b.
 *   calculation is done "in place".
 */
template <class T, class INTERLACIN_TAG> void FIELD<T, INTERLACIN_TAG>::applyLin(T a, T b)
{
    // get a non const pointer to the inside array of values and perform operation in place
    T * value=const_cast<T *> (getValue());
    const int size=getNumberOfValues()*getNumberOfComponents(); // size of array

    if (size>0) // for a negative size, there is nothing to do
    {
        const T* lastvalue=value+size; // pointer to the end of value
        for(;value!=lastvalue; ++value) // apply linear transformation
            *value = a*(*value)+b;
    }
}


/*!
 *   Return a pointer to a new field that holds the scalar product. Static member function.
 *   This operation is authorized only for compatible fields that have the same support.
 *   The compatibility checking includes equality tests of the folowing data members:\n
 *   - _support
 *   - _numberOfComponents
 *   - _numberOfValues
 *   - _componentsTypes
 *   - _MEDComponentsUnits.
 *   Data members are initialized.
 *   The new field point to the same support and has one component.
 *   Each value of it is the scalar product of the two argument's fields.
 *   The user is in charge of memory deallocation.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>*
FIELD<T, INTERLACING_TAG>::scalarProduct(const FIELD & m, const FIELD & n, bool deepCheck)
{
    if(!deepCheck)
      FIELD_::_checkFieldCompatibility( m, n, false); // may throw exception
    else
      FIELD_::_deepCheckFieldCompatibility(m, n, false);

    // we need a MED_FULL_INTERLACE representation of m & n to compute the scalar product
    // result type imply INTERLACING_TAG=FullInterlace for m & n

    const int numberOfElements=m.getNumberOfValues(); // strictly positive
    const int NumberOfComponents=m.getNumberOfComponents(); // strictly positive

    // Creation & init of a the result field on the same support, with one component
    // You have to be careful about the interlacing mode, because in the computation step,
    // it seems to assume the the interlacing mode is the FullInterlacing

    FIELD<T, INTERLACING_TAG>* result = new FIELD<T, INTERLACING_TAG>(m.getSupport(),1);
    result->setName( "scalarProduct ( " + m.getName() + " , " + n.getName() + " )" );
    result->setIterationNumber(m.getIterationNumber());
    result->setTime(m.getTime());
    result->setOrderNumber(m.getOrderNumber());

    const T* value1=m.getValue(); // get const pointer to the values
    const T* value2=n.getValue(); // get const pointer to the values
    // get a non const pointer to the inside array of values and perform operation
    T * value=const_cast<T *> (result->getValue());

    const T* lastvalue=value+numberOfElements; // pointing just after last value of result
    for ( ; value!=lastvalue ; ++value ) // loop on all elements
    {
        *value=(T)0; // initialize value
        const T* endofRow=value1+NumberOfComponents; // pointing just after end of row
        for ( ; value1 != endofRow; ++value1, ++value2) // computation of dot product
            *value += (*value1) * (*value2);
    }
    return result;
}

/*!  Return L2 Norm  of the field's component.
 *   Cannot be applied to a field with a support on nodes.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 *   For the nodal field, p_field_volume must be for all cells even if the field is partial.
 */
template <class T, class INTERLACING_TAG>
double FIELD<T, INTERLACING_TAG>::normL2(int component,
                                         const FIELD<double, FullInterlace> * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume, /*nodalAllowed=*/true); // may throw exception
    if ( component<1 || component>getNumberOfComponents() )
        throw MEDEXCEPTION(STRING("FIELD<T>::normL2() : The component argument should be between 1 and the number of components"));

    const FIELD<double, FullInterlace> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
      p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implÃ©mentation dans mesh]
    else
      p_field_size->addReference();
    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol=p_field_size->getValue();
    // Il n'est vraiment pas optimal de mixer des champs dans des modes d'entrelacement
    // different juste pour le calcul


    double integrale=0.0;
    double totVol=0.0;

    if ( getSupport()->getEntity() == MED_EN::MED_NODE ) // issue 20120: [CEA 206] normL2 on NODE field
    {
      //Most frequently the FIELD is on the whole mesh and
      // there is no need in optimizing iterations from supporting nodes-> back to cells,
      // so we iterate just on all cells
      const MESH * mesh = getSupport()->getMesh()->convertInMESH();
      const int nbCells = mesh->getNumberOfElements(MED_EN::MED_CELL,MED_ALL_ELEMENTS);
      const int *C = mesh->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_ALL_ELEMENTS);
      const int *iC = mesh->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
      for (int i = 0; i < nbCells; ++i, ++vol) {
        // calculate integral on current element as average summ of values on all it's nodes
        double curCellValue = 0;
        try { // we expect exception with partial fields for nodes w/o values
          for (int ij = iC[i]; ij < iC[i+1]; ij++) {
            int node = C[ij-1];
            curCellValue += getValueIJ( node, component );
          }
        }
        catch ( MEDEXCEPTION ) {
          continue;
        }
        int nbNodes = iC[i+1]-iC[i];
        curCellValue /= nbNodes;
        integrale += (curCellValue * curCellValue) * std::abs(*vol);
        totVol+=std::abs(*vol);
      }
      mesh->removeReference();
    }
    else
    {
      if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE ) {
        const T* value = getValue();
        value = value + (component-1) * getNumberOfValues();
        const T* lastvalue = value + getNumberOfValues(); // pointing just after the end of column
        for (; value!=lastvalue ; ++value ,++vol) {
          integrale += double((*value) * (*value)) * std::abs(*vol);
          totVol+=std::abs(*vol);
        }
      }
      else if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE ) {
        ArrayNoByType* anArray = dynamic_cast< ArrayNoByType * > ( getArrayNoGauss() );
        for (int i=1; i <= anArray->getNbElem() ; i++, ++vol ) {
          integrale += anArray->getIJ(i,component) * anArray->getIJ(i,component) * std::abs(*vol);
          totVol+=std::abs(*vol);
        }
      }
      else { // FULL_INTERLACE
        ArrayFull* anArray = dynamic_cast< ArrayFull * > ( getArrayNoGauss() );
        for (int i=1; i <= anArray->getNbElem() ; i++, ++vol ) {
          integrale += anArray->getIJ(i,component) * anArray->getIJ(i,component) * std::abs(*vol);
          totVol+=std::abs(*vol);
        }
      }
    }

    if(p_field_size)
      p_field_size->removeReference(); // delete temporary volume field

    if( totVol <= 0)
        throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));
    return integrale/totVol;
}

/*!  Return L2 Norm  of the field.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 *   For the nodal field, p_field_volume must be for all cells even if the field is partial.
 */
template <class T, class INTERLACING_TAG>
double FIELD<T, INTERLACING_TAG>::normL2(const FIELD<double, FullInterlace> * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume, /*nodalAllowed=*/true); // may throw exception
    const FIELD<double, FullInterlace> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
      p_field_size=_getFieldSize(); // we calculate the volume
    else
      p_field_size->addReference();
    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol=p_field_size->getValue();
    const double* lastvol=vol+getNumberOfValues(); // pointing just after the end of vol


    double integrale=0.0;
    double totVol=0.0;

    if ( getSupport()->getEntity() == MED_EN::MED_NODE ) // issue 20120: [CEA 206] normL2 on NODE field
    {
      //Most frequently the FIELD is on the whole mesh and
      // there is no need in optimizing iterations from supporting nodes-> back to cells,
      // so we iterate just on all cells
      const MESH * mesh = getSupport()->getMesh()->convertInMESH();
      const int nbCells = mesh->getNumberOfElements(MED_EN::MED_CELL,MED_ALL_ELEMENTS);
      const int *C = mesh->getConnectivity(MED_EN::MED_NODAL,MED_EN::MED_CELL,MED_ALL_ELEMENTS);
      const int *iC = mesh->getConnectivityIndex(MED_EN::MED_NODAL,MED_EN::MED_CELL);
      int nbComp = getNumberOfComponents();
      for (int i = 0; i < nbCells; ++i, ++vol) {
        // calculate integral on current element as average summ of values on all it's nodes
        int nbNodes = iC[i+1]-iC[i];
        vector< double > curCellValue( nbComp, 0 );
        try { // we expect exception with partial fields for nodes w/o values
          for (int ij = iC[i]; ij < iC[i+1]; ij++) {
            int node = C[ij-1];
            for ( int j = 0; j < nbComp; ++j )
              curCellValue[ j ] += getValueIJ( node, j+1 ) / nbNodes;
          }
        }
        catch ( MEDEXCEPTION ) {
          continue;
        }

        for ( int j = 0; j < nbComp; ++j ) {
          integrale += (curCellValue[j] * curCellValue[j]) * std::abs(*vol);
        }
        totVol+=std::abs(*vol);
      }
      mesh->removeReference();
      if ( nbCells > 0 && totVol == 0.)
        throw MEDEXCEPTION("can't compute sobolev norm : "
                           "none of elements has values on all it's nodes");
    }
    else
    {
      const double* p_vol=vol;
      for (p_vol=vol; p_vol!=lastvol ; ++p_vol) // calculate total volume
        totVol+=std::abs(*p_vol);

      if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE ) {
        const T* value = getValue();
        for (int i=1; i<=getNumberOfComponents(); ++i) { // compute integral on all components
          for (p_vol=vol; p_vol!=lastvol ; ++value ,++p_vol) {
            integrale += (*value) * (*value) * std::abs(*p_vol);
          }
        }
      }
      else if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE ) {
        ArrayNoByType* anArray = dynamic_cast< ArrayNoByType * > ( getArrayNoGauss() );
        for (int j=1; j<=anArray->getDim(); j++) {
          int i = 1;
          for (p_vol=vol; i<=anArray->getNbElem() || p_vol!=lastvol; i++, ++p_vol ) {
            integrale += anArray->getIJ(i,j) * anArray->getIJ(i,j) * std::abs(*p_vol);
          }
        }
      }
      else { // FULL_INTERLACE
        ArrayFull* anArray = dynamic_cast< ArrayFull * > ( getArrayNoGauss() );
        for (int j=1; j<=anArray->getDim(); j++) {
          int i = 1;
          for (p_vol=vol; i<=anArray->getNbElem() || p_vol!=lastvol; i++, ++p_vol ) {
            integrale += anArray->getIJ(i,j) * anArray->getIJ(i,j) * std::abs(*p_vol);
          }
        }
      }
    }
    if(p_field_size)
        p_field_size->removeReference(); // delete temporary volume field

    if( totVol <= 0)
      throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));
    return integrale/totVol;
}

/*!  Return L1 Norm  of the field's component.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 */
template <class T, class INTERLACING_TAG>
double FIELD<T, INTERLACING_TAG>::normL1(int component,
                                         const FIELD<double, FullInterlace > * p_field_volume) const
{
    _checkNormCompatibility(p_field_volume); // may throw exception
    if ( component<1 || component>getNumberOfComponents() )
        throw MEDEXCEPTION(STRING("FIELD<T,INTERLACING_TAG>::normL1() : The component argument should be between 1 and the number of components"));

    const FIELD<double,FullInterlace> * p_field_size=p_field_volume;
    if(!p_field_volume) // if the user don't supply the volume
      p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implÃÃÅ mentation dans mesh]
    else
      p_field_size->addReference();
    // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
    const double* vol = p_field_size->getValue();

    double integrale=0.0;
    double totVol=0.0;

    if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE ) {
      const T* value = getValue();
      const T* lastvalue = value + getNumberOfValues(); // pointing just after the end of column
      for (; value!=lastvalue ; ++value ,++vol) {
        integrale += std::abs( *value * *vol );
        totVol+=std::abs(*vol);
      }
    }
    else if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE ) {
      ArrayNoByType* anArray = dynamic_cast< ArrayNoByType * > ( getArrayNoGauss() );
      for (int i=1; i <= anArray->getNbElem() ; i++, ++vol ) {
        integrale += std::abs( anArray->getIJ(i,component) * (*vol));
        totVol+=std::abs(*vol);
      }
    }
    else { // FULL_INTERLACE
      ArrayFull* anArray = dynamic_cast< ArrayFull * > ( getArrayNoGauss() );
      for (int i=1; i <= anArray->getNbElem() ; i++, ++vol ) {
        integrale += std::abs( anArray->getIJ(i,component) * *vol);
        totVol+=std::abs(*vol);
      }
    }
    
    if(p_field_size)
      p_field_size->removeReference(); // delete temporary volume field
    if( totVol <= 0)
        throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));
    return integrale/totVol;
}

/*!  Return L1 Norm  of the field.
 *   Cannot be applied to a field with a support on nodes.
 *   If the optional p_field_volume argument is furnished, the volume is not re-calculated.
 */
template <class T, class INTERLACING_TAG>
double FIELD<T, INTERLACING_TAG>::normL1(const FIELD<double, FullInterlace> * p_field_volume) const
{
  _checkNormCompatibility(p_field_volume); // may throw exception
  const FIELD<double, FullInterlace> * p_field_size=p_field_volume;
  if(!p_field_volume) // if the user don't supply the volume
    p_field_size=_getFieldSize(); // we calculate the volume [PROVISOIRE, en attendant l'implÃÂ©mentation dans mesh]
  else
    p_field_size->addReference();
  // get pointer to the element's volumes. MED_FULL_INTERLACE is the default mode for p_field_size
  const double* vol = p_field_size->getValue();
  const double* lastvol = vol+getNumberOfValues(); // pointing just after the end of vol

  double integrale=0.0;
  double totVol=0.0;
  const double* p_vol=vol;
  for (p_vol=vol; p_vol!=lastvol ; ++p_vol) // calculate total volume
    totVol+=std::abs(*p_vol);

  if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE ) {
    const T* value = getValue();
    for (int i=1; i<=getNumberOfComponents(); ++i) { // compute integral on all components
      for (p_vol=vol; p_vol!=lastvol ; ++value ,++p_vol) {
        integrale += std::abs( *value * *p_vol );
      }
    }
  }
  else if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE ) {
    ArrayNoByType* anArray = dynamic_cast< ArrayNoByType * > ( getArrayNoGauss() );
    for (int j=1; j<=anArray->getDim(); j++) {
      int i = 1;
      for (p_vol=vol; i<=anArray->getNbElem() || p_vol!=lastvol; i++, ++p_vol ) {
        integrale += std::abs( anArray->getIJ(i,j) * *p_vol );
      }
    }
  }
  else { // FULL_INTERLACE
    ArrayFull* anArray = dynamic_cast< ArrayFull * > ( getArrayNoGauss() );
    for (int j=1; j<=anArray->getDim(); j++) {
      int i = 1;
      for (p_vol=vol; i<=anArray->getNbElem() || p_vol!=lastvol; i++, ++p_vol ) {
        integrale += std::abs( anArray->getIJ(i,j) * *p_vol );
      }
    }
  }
  if(p_field_size)
    p_field_size->removeReference();
  if( totVol <= 0)
    throw MEDEXCEPTION(STRING("cannot compute sobolev norm : volume is not positive!"));
  return integrale/totVol;
}

/*!
 * \brief Return integral of the field.
 *  \param subSupport - optional part of a field to consider.
 *  \retval double - value of integral
 */
template <class T, class INTERLACING_TAG>
double FIELD<T, INTERLACING_TAG>::integral(const SUPPORT *subSupport) const throw (MEDEXCEPTION)
{
  const char* LOC = "FIELD<>::integral(subSupport): ";

  double integrale = 0;

  if (!subSupport ) subSupport = _support;

  // check feasibility
  if ( getGaussPresence() )
    throw MEDEXCEPTION(STRING(LOC)<<"Gauss numbers greater than one are not yet implemented!");
  if ( subSupport->getEntity() != _support->getEntity())
    throw MEDEXCEPTION(STRING(LOC)<<"Different support entity of this field and subSupport");
  if ( subSupport->getEntity() == MED_EN::MED_NODE )
    throw MEDEXCEPTION(STRING(LOC)<<"Integral of nodal field not yet supported");

  // analyze support
  const int nbElems = subSupport->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
  const bool subOnAll = ( subSupport->isOnAllElements() );
  const bool  myOnAll = ( _support->isOnAllElements() );
  const int* subNums = !subOnAll ? subSupport->getNumber(MED_EN::MED_ALL_ELEMENTS) : 0;
  const int*   myNums = !myOnAll ? _support->getNumber(MED_EN::MED_ALL_ELEMENTS) : 0;
  if ( !subOnAll && !subNums )
    throw MEDEXCEPTION(STRING(LOC)<<"Invalid support: no element numbers");
  if ( !myOnAll && !myNums )
    throw MEDEXCEPTION(STRING(LOC)<<"Invalid field support: no element numbers");
  if ( subOnAll && !myOnAll )
    return integral(NULL);

  // get size of elements
  const FIELD<double, FullInterlace> * cellSize=_getFieldSize(subSupport);
  const double* size = cellSize->getValue();
  const double* lastSize = size + nbElems; // pointing just after the end of size

  const T* value = getValue();

  // calculate integrale
  if ( (subOnAll && _support->isOnAllElements()) || subSupport == _support )
    {
      const double* p_vol;
      if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE )
        {
          for (int j=1; j<=getNumberOfComponents(); ++j)
            for ( p_vol=size; p_vol != lastSize; ++value ,++p_vol)
              integrale += std::abs( *value * *p_vol );
        }
      else if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE )
        {
          typename ArrayNoByType::InterlacingPolicy* indexer =
            dynamic_cast< typename ArrayNoByType::InterlacingPolicy * > ( getArrayNoGauss() );
          for (int i, j=1; j<=getNumberOfComponents(); j++)
            for (i = 1, p_vol=size; p_vol!=lastSize; i++, ++p_vol )
              integrale += std::abs( value[indexer->getIndex(i,j)] * *p_vol );
        }
      else  // FULL_INTERLACE
        {
          for ( p_vol=size; p_vol != lastSize; ++p_vol)
            for (int j=0; j<getNumberOfComponents(); ++j, ++value)
              integrale += std::abs( *value * *p_vol );
        }
    }
  else
    {
      // find index for each element of subSupport
      PointerOf<int> index;
      if ( _support->isOnAllElements() )
        {
          index.set( subNums );
        }
      else // find common of two partial supports
        {
          // hope that numbers are in increasing order
          index.set( nbElems );
          for (int ii = 0; ii < nbElems; ii++)
            index[ii] = 0;
          bool allNumsFound = true;
          int i = 0, iSub = 0;
          for ( ; iSub < nbElems; ++iSub )
            {
              while ( i < getNumberOfValues() && subNums[iSub] > myNums[i] )
                ++i;
              if (i == getNumberOfValues() /*subNums[iSub] > myNums[i]*/) // no more myNums
                {
                  index[iSub] = 0; // no such number in myNums
                  break;
                }
              else if ( subNums[iSub] == myNums[i] ) // elem number found
                index[iSub] = ++i; // -- index counts from 1
              else // subNums[iSub] < myNums[i]
                allNumsFound = (index[iSub] = 0); // no such number in myNums
            }
          if ( iSub != nbElems || !allNumsFound )
            {
              // check if numbers are in increasing order
              bool increasingOrder = true;
              for ( iSub = 1; iSub < nbElems && increasingOrder; ++iSub )
                increasingOrder = ( subNums[iSub-1] < subNums[iSub] );
              for ( i = 1; i < getNumberOfValues() && increasingOrder; ++i )
                increasingOrder = ( myNums[i-1] < myNums[i] );

              if ( !increasingOrder )
                for ( iSub = 0; iSub < nbElems; ++iSub )
                  try
                    {
                      index[iSub] = _support->getValIndFromGlobalNumber( subNums[iSub] );
                    }
                  catch (MEDEXCEPTION)
                    {
                      index[iSub] = 0;
                    }
            }
        }

      // calculation
      if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE )
        {
          for (int j=0; j<getNumberOfComponents(); ++j)
            {
              value = getValue() + j * getNumberOfValues();
              for ( int i = 0; i < nbElems; ++i )
                if ( index[i] )
                  integrale += std::abs( value[ index[i]-1 ] * size[i] );
            }
        }
      else if ( getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE )
        {
          typename ArrayNoByType::InterlacingPolicy* indexer =
            dynamic_cast< typename ArrayNoByType::InterlacingPolicy * > ( getArrayNoGauss() );
          for (int j=1; j<=getNumberOfComponents(); j++)
            for ( int i = 0; i < nbElems; ++i )
              if ( index[i] )
                integrale += std::abs( value[indexer->getIndex(index[i],j)] * size[i] );
        }
      else  // FULL_INTERLACE
        {
          const int dim = getNumberOfComponents();
          for ( int i = 0; i < nbElems; ++i )
            if ( index[i] )
              for (int j=0; j<dim; ++j)
                integrale += std::abs( value[ dim*(index[i]-1) + j] * size[i] );
        }
    }
  cellSize->removeReference();
  return integrale;
}

/*! Return a new field (to deallocate with delete) lying on subSupport that is included by
 *   this->_support with corresponding values extracting from this->_value.
 */
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>* FIELD<T, INTERLACING_TAG>::extract(const SUPPORT *subSupport) const throw (MEDEXCEPTION)
{
  if(!subSupport->belongsTo(*_support))
    throw MEDEXCEPTION("FIELD<T>::extract : subSupport not included in this->_support !");
  if(_support->isOnAllElements() && subSupport->isOnAllElements())
    return new FIELD<T, INTERLACING_TAG>(*this);

  FIELD<T, INTERLACING_TAG> *ret = new FIELD<T, INTERLACING_TAG>(subSupport,
                                                                 _numberOfComponents);

  if(!ret->_value)
    throw MEDEXCEPTION("FIELD<T>::extract : invalid support detected !");

  T* valuesToSet=(T*)ret->getValue();

  int nbOfEltsSub=subSupport->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
  const int *eltsSub=subSupport->getNumber(MED_EN::MED_ALL_ELEMENTS);
  T* tempVals=new T[_numberOfComponents];
  for(int i=0;i<nbOfEltsSub;i++)
    {
      if(!getValueOnElement(eltsSub[i],tempVals))
        throw MEDEXCEPTION("Problem in belongsTo function !!!");
      for(int j=0;j<_numberOfComponents;j++)
        valuesToSet[i*_numberOfComponents+j]=tempVals[j];
    }
  delete [] tempVals;

  ret->copyGlobalInfo(*this);
  return ret;
}
/*!
@}
*/

/*!
        \addtogroup FIELD_io
        @{
*/
/*!
  Constructor with parameters; the object is set via a file and its associated
  driver. For the moment only the MED_DRIVER is considered and if the last two
  argument (iterationNumber and orderNumber) are not set; their default value
  is -1. If the field fieldDriverName with the iteration number
  iterationNumber and the order number orderNumber does not exist in the file
  fieldDriverName; the constructor raises an exception.
*/
template <class T, class INTERLACING_TAG>
FIELD<T, INTERLACING_TAG>::FIELD(const SUPPORT * Support,
                                 driverTypes driverType,
                                 const string & fileName/*=""*/,
                                 const string & fieldDriverName/*=""*/,
                                 const int iterationNumber,
                                 const int orderNumber) throw (MEDEXCEPTION)
{
  const char LOC[] = "template <class T> FIELD<T>::FIELD(const SUPPORT * Support, driverTypes driverType, const string & fileName=\"\", const string & fieldName=\"\", const int iterationNumber=-1, const int orderNumber=-1) : ";
  BEGIN_OF_MED(LOC);

  int current;

  init();

  _mesh  = ( MESH* ) NULL;

  //INITIALISATION DE _valueType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT_MED(FIELD_::_valueType == MED_EN::MED_UNDEFINED_TYPE)
  FIELD_::_valueType=SET_VALUE_TYPE<T>::_valueType;

  //INITIALISATION DE _interlacingType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT_MED(FIELD_::_interlacingType == MED_EN::MED_UNDEFINED_INTERLACE)
  FIELD_::_interlacingType=SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType;

  _support = Support;
  //A.G. Addings for RC
  if(_support)
    _support->addReference();
  // OCC 10/03/2006 -- According to the rules defined with help of 
  // MEDMEM_IntrelacingTraits class, it is not allowed to instantiate
  // MEDMEM_Array<> template using INTERLACING_TAG parameter of 
  // FIELD template - MSVC++ 2003 compiler generated an error here.
  // _value = (MEDMEM_Array<T, INTERLACING_TAG> *) NULL;
  _value = NULL;

  _iterationNumber = iterationNumber;
  _time = 0.0;
  _orderNumber = orderNumber;

  current = addDriver(driverType,fileName,fieldDriverName,MED_EN::RDONLY);

  _drivers[current]->open();
  _drivers[current]->read();
  _drivers[current]->close();

  END_OF_MED(LOC);
}

/*!
  If the mesh argument is not initialized or passed NULL,
  this constructor, at least, allows to create a FIELD without creating any
  SUPPORT then without having to load a MESH object, a support is created. It
  provides the meshName related mesh but doesn't not set a mesh in the created
  support.
  If the passed mesh contains corresponding support, this support will be used
  for the field. This support will be found in mesh by name of one of profiles,
  on which the FIELD lays in MED-file. This has sense for the case, then MED-file
  was created by MEDMEM, and so name of profile contains name of corresponding support.
*/
template <class T, class INTERLACING_TAG>
FIELD<T,INTERLACING_TAG>::FIELD(driverTypes    driverType,
                                const string & fileName,
                                const string & fieldDriverName,
                                const int      iterationNumber,
                                const int      orderNumber,
                                GMESH*         mesh)
  throw (MEDEXCEPTION) :FIELD_()
{
  int current;
  const char LOC[]= "FIELD<T,INTERLACING_TAG>::FIELD( driverTypes driverType, const string & fileName, string & fieldDriverName, int iterationNumber, int orderNumber) : ";
  BEGIN_OF_MED(LOC);

  init();

  _mesh = mesh;
  if(_mesh)
    _mesh->addReference();

  //INITIALISATION DE _valueType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT_MED(FIELD_::_valueType == MED_EN::MED_UNDEFINED_TYPE)
  FIELD_::_valueType = SET_VALUE_TYPE<T>::_valueType;

  //INITIALISATION DE _interlacingType DS LE CONSTRUCTEUR DE FIELD_
  ASSERT_MED(FIELD_::_interlacingType == MED_EN::MED_UNDEFINED_INTERLACE)
  FIELD_::_interlacingType = SET_INTERLACING_TYPE<INTERLACING_TAG>::_interlacingType;

  _support = (SUPPORT *) NULL;
  // OCC 10/03/2006 -- According to the rules defined with help of 
  // MEDMEM_IntrelacingTraits class, it is not allowed to instantiate
  // MEDMEM_Array<> template using INTERLACING_TAG parameter of 
  // FIELD template - MSVC++ 2003 compiler generated an error here.
  // _value = (MEDMEM_Array<T, INTERLACING_TAG> *) NULL;
  _value = NULL;

  _iterationNumber = iterationNumber;
  _time = 0.0;
  _orderNumber = orderNumber;

  current = addDriver(driverType,fileName,fieldDriverName,MED_EN::RDONLY);

  _drivers[current]->open();
  _drivers[current]->read();
  _drivers[current]->close();

  END_OF_MED(LOC);
}
/*! 
@}
*/

/*!
  Destructor.
*/
template <class T, class INTERLACING_TAG> FIELD<T, INTERLACING_TAG>::~FIELD()
{
  const char* LOC = " Destructeur FIELD<T, INTERLACING_TAG>::~FIELD()";
  BEGIN_OF_MED(LOC);
  SCRUTE_MED(this);
  if (_value) delete _value; _value=0;
  locMap::const_iterator it;
  for ( it = _gaussModel.begin();it != _gaussModel.end(); it++ )
    delete (*it).second;
  _gaussModel.clear();
  if(_mesh)
    _mesh->removeReference();
  _mesh=0;
  END_OF_MED(LOC);
}

/*!

*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::allocValue(const int NumberOfComponents)
{
  const char* LOC = "FIELD<T, INTERLACING_TAG>::allocValue(const int NumberOfComponents)";
  BEGIN_OF_MED(LOC);

  _numberOfComponents = NumberOfComponents ;
  _componentsTypes.resize(NumberOfComponents);
  _componentsNames.resize(NumberOfComponents);
  _componentsDescriptions.resize(NumberOfComponents);
  _componentsUnits.resize(NumberOfComponents);
  _MEDComponentsUnits.resize(NumberOfComponents);
  for (int i=0;i<NumberOfComponents;i++) {
    _componentsTypes[i] = 0 ;
  }
  delete _value;
  try {
    // becarefull about the number of gauss point
    _numberOfValues = _support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
    MESSAGE_MED(PREFIX_MED <<" : "<<_numberOfValues <<" et "<< NumberOfComponents);

    //EF : A modifier lors de l'intÃ©gration de la classe de localisation des points de gauss
    _value = new ArrayNoGauss(_numberOfComponents,_numberOfValues);

    _isRead = true ;
  }
  catch (MEDEXCEPTION &ex) {
    MESSAGE_MED("No value defined, problem with NumberOfComponents (and may be _support) size of MEDARRAY<T>::_value !");
    // OCC 10/03/2006 -- According to the rules defined with help of 
    // MEDMEM_IntrelacingTraits class, it is not allowed to instantiate
    // MEDMEM_Array<> template using INTERLACING_TAG parameter of 
    // FIELD template - MSVC++ 2003 compiler generated an error here.
    // _value = (MEDMEM_Array<T, INTERLACING_TAG> *) NULL;
    _value = NULL;
  }

  SCRUTE_MED(_value);
  END_OF_MED(LOC);
}

/*!

*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::allocValue(const int NumberOfComponents,
                                           const int LengthValue)
{
  const char* LOC = "void FIELD<T>::allocValue(const int NumberOfComponents,const int LengthValue)";
  BEGIN_OF_MED(LOC);

  _numberOfComponents = NumberOfComponents ;
  _componentsTypes.resize(NumberOfComponents);
  _componentsNames.resize(NumberOfComponents);
  _componentsDescriptions.resize(NumberOfComponents);
  _componentsUnits.resize(NumberOfComponents);
  _MEDComponentsUnits.resize(NumberOfComponents);
  for (int i=0;i<NumberOfComponents;i++) {
    _componentsTypes[i] = 0 ;
  }

  MESSAGE_MED("FIELD : constructeur : "<<LengthValue <<" et "<< NumberOfComponents);
  _numberOfValues = LengthValue ;
  delete _value;
  //EF : A modifier lors de l'intÃ©gration de la classe de localisation des points de gauss
  _value = new ArrayNoGauss(_numberOfComponents,_numberOfValues);

  _isRead = true ;

  SCRUTE_MED(_value);
  END_OF_MED(LOC);
}

/*!

*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::deallocValue()
{
  const char* LOC = "void FIELD<T, INTERLACING_TAG>::deallocValue()";
  BEGIN_OF_MED(LOC);
  _numberOfValues = 0 ;
  _numberOfComponents = 0 ;
  if (_value != NULL) {
    delete _value;
    _value = NULL;
  }

  END_OF_MED(LOC);
}



/*!\if MEDMEM_ug

\addtogroup FIELD_io
@{ 
\endif */
// -----------------
// Methodes Inline
// -----------------

/*!
  Creates the specified driver and return its index reference to path to
  read or write methods.

\param driverType specifies the file type (MED_DRIVER or VTK_DRIVER)
\param fileName name of the output file
\param driverName name of the field
\param access access type (read, write or both)

*/

template <class T, class INTERLACING_TAG>
int FIELD<T, INTERLACING_TAG>::addDriver(driverTypes driverType,
                                         const string & fileName/*="Default File Name.med"*/,
                                         const string & driverName/*="Default Field Name"*/,
                                         MED_EN::med_mode_acces access)
{
  //jfa tmp (as last argument has no default value):const char LOC[]= "FIELD<T>::addDriver(driverTypes driverType, const string & fileName=\"Default File Name.med\",const string & driverName=\"Default Field Name\",MED_EN::med_mode_acces access) : ";

  GENDRIVER * driver;

  const char* LOC = "FIELD<T>::addDriver(driverTypes driverType, const string & fileName,const string & driverName,MED_EN::med_mode_acces access) :";
  BEGIN_OF_MED(LOC);

  SCRUTE_MED(driverType);

  driver = DRIVERFACTORY::buildDriverForField(driverType,fileName,this,access);

  _drivers.push_back(driver);

  int current = _drivers.size()-1;

  _drivers[current]->setFieldName(driverName);

  END_OF_MED(LOC);

  return current;
}

/*!
  Read FIELD in the file specified in the driver given by its index.
*/
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::read(int index/*=0*/)
{
  const char LOC[]= "FIELD<T, INTERLACING_TAG>::read(int index=0) : ";
  BEGIN_OF_MED(LOC);

  if ( index>=0 && index<(int)_drivers.size() && _drivers[index] ) {
    _drivers[index]->open();
    try
    {
      _drivers[index]->read();
    }
    catch ( const MEDEXCEPTION& ex )
    {
      _drivers[index]->close();
      throw ex;
    }
    _drivers[index]->close();
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "The index given is invalid, index must be between  0 and |"
                                     << _drivers.size()
                                     )
                          );
  END_OF_MED(LOC);
}

/*!
  Read FIELD with the driver. Additional information (name etc.) to select a field
  must be set to the field.
*/
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::read(const GENDRIVER & driver )
{
  const char* LOC = " FIELD<T, INTERLACING_TAG>::read(const GENDRIVER &) : ";
  BEGIN_OF_MED(LOC);

  // For the case where driver does not know about me since it has been created through
  // constructor witout parameters: create newDriver knowing me and get missing data
  // from driver using merge()
  std::unique_ptr<GENDRIVER> newDriver( DRIVERFACTORY::buildDriverForField(driver.getDriverType(),
                                                                         driver.getFileName(),
                                                                         this, RDONLY));
  newDriver->merge( driver );

  newDriver->open();
  try
  {
    newDriver->read();
  }
  catch ( const MEDEXCEPTION& ex )
  {
    newDriver->close();
    throw ex;
  }
  newDriver->close();

  END_OF_MED(LOC);
}

/*!
  Read FIELD with driver of the given type. Additional information (name etc.) to select a field
  must be set to the field.
*/
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::read(driverTypes driverType, const std::string& filename)
{
  const char* LOC = " FIELD<T, INTERLACING_TAG>::read(driverTypes driverType, const std::string& filename) : ";
  BEGIN_OF_MED(LOC);

  std::unique_ptr<GENDRIVER> newDriver( DRIVERFACTORY::buildDriverForField(driverType, filename,
                                                                         this, RDONLY));
  newDriver->open();
  try
  {
    newDriver->read();
  }
  catch ( const MEDEXCEPTION& ex )
  {
    newDriver->close();
    throw ex;
  }
  newDriver->close();

  END_OF_MED(LOC);
}

/*! \if MEDMEM_ug @} \endif */

/*!
  Duplicates the given driver and return its index reference to path to
  read or write methods.
*/
template <class T, class INTERLACING_TAG>
inline int FIELD<T, INTERLACING_TAG>::addDriver (GENDRIVER & driver )
{
  int current;

  const char* LOC = "FIELD<T, INTERLACING_TAG>::addDriver(GENDRIVER &) : ";
  BEGIN_OF_MED(LOC);

  GENDRIVER * newDriver = 
    DRIVERFACTORY::buildDriverForField(driver.getDriverType(),
                                       driver.getFileName(), this,
                                       driver.getAccessMode());
  _drivers.push_back(newDriver);

  current = _drivers.size()-1;
  SCRUTE_MED(current);
  driver.setId(current);

  newDriver->merge( driver );
  newDriver->setId(current);

  return current ;
}

/*!
  Remove the driver referenced by its index.
*/
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::rmDriver (int index/*=0*/)
{
  const char LOC[]= "FIELD<T, INTERLACING_TAG>::rmDriver (int index=0): ";
  BEGIN_OF_MED(LOC);

  if ( index>=0 && index<(int)_drivers.size() && _drivers[index] ) {
    MESSAGE_MED ("detruire");
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "The <index given is invalid, index must be between  0 and  |"
                                     << _drivers.size()
                                     )
                          );

  END_OF_MED(LOC);
}

/*! \if MEDMEM_ug
\addtogroup FIELD_io
@{
\endif */

/*!
  Writes FIELD in the file specified by the driver handle \a index.

Example :
\verbatim
//...
// Attaching the friver to file "output.med", meshname "Mesh"
int driver_handle = mesh.addDriver(MED_DRIVER, "output.med", "Mesh");
// Writing the content of mesh to the file 
mesh.write(driver_handle);
\endverbatim
*/
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::write(int index/*=0*/)
{
  const char LOC[]= "FIELD<T,INTERLACING_TAG>::write(int index=0, const string & driverName = \"\") : ";
  BEGIN_OF_MED(LOC);

  if( index>=0 && index<(int)_drivers.size() && _drivers[index] ) {
    _drivers[index]->open();
    try
    {
      _drivers[index]->write();
    }
    catch ( const MEDEXCEPTION& ex )
    {
      _drivers[index]->close();
      throw ex;
    }
    _drivers[index]->close();
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "The index given is invalid, index must be between  0 and |"
                                     << _drivers.size()
                                     )
                          );
  END_OF_MED(LOC);
}
/*!
  Write FIELD with the given driver.
*/
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::write(const GENDRIVER & driver, MED_EN::med_mode_acces medMode/*=MED_EN::RDWR*/)
{
  const char* LOC = " FIELD<T, INTERLACING_TAG>::write(const GENDRIVER &) : ";
  BEGIN_OF_MED(LOC);

  // For the case where driver does not know about me since it has been created through
  // constructor witout parameters: create newDriver knowing me and get missing data
  // from driver using merge()
  std::unique_ptr<GENDRIVER> newDriver( DRIVERFACTORY::buildDriverForField(driver.getDriverType(),
                                                                         driver.getFileName(),
                                                                         this, WRONLY));
  newDriver->merge( driver );
  if ( newDriver->getDriverType() == MED_DRIVER )
    newDriver->setAccessMode( MED_EN::med_mode_acces( getMedAccessMode( medMode ) ));

  newDriver->open();
  try
  {
    newDriver->write();
  }
  catch ( const MEDEXCEPTION& ex )
  {
    newDriver->close();
    throw ex;
  }
  newDriver->close();

  END_OF_MED(LOC);
}

/*!
  Write FIELD with driver of the given type.
*/
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::write(driverTypes driverType, const std::string& filename, MED_EN::med_mode_acces medMode/*=MED_EN::RDWR*/)
{
  const char* LOC = " FIELD<T, INTERLACING_TAG>::write(driverTypes driverType, const std::string& filename) : ";
  BEGIN_OF_MED(LOC);

  std::unique_ptr<GENDRIVER> newDriver( DRIVERFACTORY::buildDriverForField(driverType, filename,
                                                                         this, WRONLY));
  if ( newDriver->getDriverType() == MED_DRIVER )
    newDriver->setAccessMode( MED_EN::med_mode_acces( getMedAccessMode( medMode ) ));

  newDriver->open();
  try
  {
    newDriver->write();
  }
  catch ( const MEDEXCEPTION& ex )
  {
    newDriver->close();
    throw ex;
  }
  newDriver->close();

  END_OF_MED(LOC);
}

/*! \if MEDMEM_ug @} \endif */
/*!
  Write FIELD in the file specified in the driver given by its index. Use this
  method for ASCII drivers (e.g. VTK_DRIVER)
*/
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::writeAppend(int index/*=0*/, const string & driverName /*= ""*/)
{
  const char LOC[]= "FIELD<T,INTERLACING_TAG>::write(int index=0, const string & driverName = \"\") : ";
  BEGIN_OF_MED(LOC);

  if( index>=0 && index<(int)_drivers.size() && _drivers[index] ) {
    _drivers[index]->openAppend();
    if (driverName != "") _drivers[index]->setFieldName(driverName);
    try
    {
      _drivers[index]->writeAppend();
    }
    catch ( const MEDEXCEPTION& ex )
    {
      _drivers[index]->close();
      throw ex;
    }
    _drivers[index]->close();
  }
  else
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC)
                                     << "The index given is invalid, index must be between  0 and |"
                                     << _drivers.size()
                                     )
                          );
  END_OF_MED(LOC);
}

/*!
  \internal
  Write FIELD with the driver which is equal to the given driver.

  Use by MED object. Use this method for ASCII drivers (e.g. VTK_DRIVER).
*/
template <class T, class INTERLACING_TAG> inline void FIELD<T, INTERLACING_TAG>::writeAppend(const GENDRIVER & genDriver)
{
  const char* LOC = " FIELD<T, INTERLACING_TAG>::write(const GENDRIVER &) : ";
  BEGIN_OF_MED(LOC);

  for (unsigned int index=0; index < _drivers.size(); index++ )
    if ( *_drivers[index] == genDriver ) {
      _drivers[index]->openAppend();
      try
      {
        _drivers[index]->writeAppend();
      }
      catch ( const MEDEXCEPTION& ex )
      {
        _drivers[index]->close();
        throw ex;
      }
      _drivers[index]->close();
    }

  END_OF_MED(LOC);

}

/*!
  Fills in already allocated retValues array the values related to eltIdInSup.
  If the element does not exist in this->_support false is returned, true otherwise.
*/
template <class T, class INTERLACING_TAG>
bool FIELD<T, INTERLACING_TAG>::getValueOnElement(int eltIdInSup,T* retValues)
  const throw (MEDEXCEPTION)
{

  if(eltIdInSup<1)
    return false;
  if(_support->isOnAllElements())
    {
      int nbOfEltsThis=_support->getMesh()->getNumberOfElements(_support->getEntity(),MED_EN::MED_ALL_ELEMENTS);
      if(eltIdInSup>nbOfEltsThis)
        return false;
      const T* valsThis=getValue();
      for(int j=0;j<_numberOfComponents;j++)
        retValues[j]=valsThis[(eltIdInSup-1)*_numberOfComponents+j];
      return true;
    }
  else
    {
      int nbOfEltsThis=_support->getNumberOfElements(MED_EN::MED_ALL_ELEMENTS);
      const int *eltsThis=_support->getNumber(MED_EN::MED_ALL_ELEMENTS);
      int iThis;
      bool found=false;
      for(iThis=0;iThis<nbOfEltsThis && !found;)
        if(eltsThis[iThis]==eltIdInSup)
          found=true;
        else
          iThis++;
      if(!found)
        return false;
      const T* valsThis=getValue();
      for(int j=0;j<_numberOfComponents;j++)
        retValues[j]=valsThis[iThis*_numberOfComponents+j];
      return true;
    }
}

  /*!
   * \brief Retrieve value in a given point
   *  \param coords - point coordinates
   *  \param output - output buffer
   */
  template <class T, class INTERLACING_TAG>
  void FIELD<T, INTERLACING_TAG>::getValueOnPoint(const double* coords, double* output) const throw (MEDEXCEPTION)
  {
    getValueOnPoints(1, coords, output);
  }

  /*!
   * \brief Retrieve values in given points
   *  \param nb_points - number of points
   *  \param coords - point coordinates
   *  \param output - output buffer
   */
  template <class T, class INTERLACING_TAG>
  void FIELD<T, INTERLACING_TAG>::getValueOnPoints(int nb_points, const double* coords, double* output) const throw (MEDEXCEPTION)
  {
    const char* LOC = " FIELD<T, INTERLACING_TAG>::getValueOnPoints(int nb_points, const double* coords, double* output) : ";
    // check operation feasibility
    if ( !getSupport() ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"NULL Support"));
    if ( !getSupport()->getMesh() ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"NULL Mesh"));
    if ( !_value ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"NULL _value"));
    if ( getGaussPresence() ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Not implemeneted for Gauss points"));

    MED_EN::medEntityMesh entity = getSupport()->getEntity();
    if ( entity != MED_EN::MED_CELL &&
         entity != MED_EN::MED_NODE )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support must be on CELLs or NODEs"));

    // initialize output value
    for ( int j = 0; j < nb_points*getNumberOfComponents(); ++j )
      output[j] = 0.0;

    const MEDMEM::MESH* mesh = getSupport()->getMesh()->convertInMESH();
    MEDMEM::AutoDeref derefMesh( mesh );

    const double* point = coords;
    double* value = output;

    if ( entity == MED_EN::MED_CELL )
      {
        MEDMEM::PointLocator pLocator (*mesh);
        for ( int i = 0; i < nb_points; ++i)
          {
            // find the cell enclosing the point
            std::list<int> cellIds = pLocator.locate( point );
            int nbCells = cellIds.size();
            if ( nbCells < 1 )
              throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Point is out of mesh"));

            // retrieve value
            std::list<int>::iterator iCell = cellIds.begin();
            for ( ; iCell != cellIds.end(); ++iCell )
              for ( int j = 1; j <= getNumberOfComponents(); ++j )
                value[j-1] += getValueIJ( *iCell, j ) / nbCells;

            // next point
            point += mesh->getSpaceDimension();
            value += getNumberOfComponents();
          }
      }
    else // MED_EN::MED_NODE
      {
        const double * allCoords = mesh->getCoordinates( MED_EN::MED_FULL_INTERLACE );

        MEDMEM::PointLocatorInSimplex pLocator (*mesh);
        for ( int i = 0; i < nb_points; ++i)
          {
            // find nodes of the simplex enclosing the point
            std::list<int> nodeIds = pLocator.locate( point );
            int nbNodes = nodeIds.size();
            if ( nbNodes < 1 )
              throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Point is out of mesh"));
            if ( nbNodes != mesh->getMeshDimension() + 1 )
              throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Invalid nb of points of simplex: "<<nbNodes));

            // get coordinates of simplex nodes
            std::vector<const double*> nodeCoords( nbNodes );
            std::list<int>::iterator iNode = nodeIds.begin();
            int n = 0;
            for ( ; n < nbNodes; ++iNode, ++n )
              nodeCoords[n] = allCoords + (*iNode-1) * mesh->getSpaceDimension();

            // compute wegths of simplex nodes
            double nodeWgt[4];
            pLocator.getNodeWightsInSimplex( nodeCoords, coords, nodeWgt );

            // retrieve value
            for ( n = 0, iNode = nodeIds.begin(); iNode != nodeIds.end(); ++iNode, ++n )
              for ( int j = 1; j <= getNumberOfComponents(); ++j )
                value[j-1] += getValueIJ( *iNode, j ) * nodeWgt[ n ];

            // next point
            point += mesh->getSpaceDimension();
            value += getNumberOfComponents();
          }
      }
  }


/*!
  Return the coordinates of the gauss points
  The returned field has SPACEDIM components 
*/
template <class T, class INTERLACING_TAG>
FIELD<double, FullInterlace>* FIELD<T, INTERLACING_TAG>::getGaussPointsCoordinates() const
  throw (MEDEXCEPTION)
{
  const char LOC[]= "FIELD::getGaussPointsCoordinates() : ";
  BEGIN_OF_MED(LOC);

  if (!getSupport())
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));

  const MEDMEM::MESH* mesh = getSupport()->getMesh()->convertInMESH();
  MEDMEM::AutoDeref derefMesh( mesh );

  const double * coord = mesh->getCoordinates(MED_EN::MED_FULL_INTERLACE);
  int spaceDim         = mesh->getSpaceDimension();

  //Init calculator of the gauss point coordinates
  INTERP_KERNEL::GaussCoords calculator;
  locMap::const_iterator it;

  int nb_type                     = getSupport()->getNumberOfTypes();
  int length_values               = getSupport()->getNumberOfElements(MED_ALL_ELEMENTS);
  const medGeometryElement* types = getSupport()->getTypes();
  medEntityMesh support_entity    = getSupport()->getEntity();
  bool isOnAll                    = getSupport()->isOnAllElements();

  const int* global_connectivity  = 0;
  const GAUSS_LOCALIZATION<INTERLACING_TAG>* gaussLock = NULL;

  typedef typename MEDMEM_ArrayInterface<double,INTERLACING_TAG,NoGauss>::Array ArrayCoord;
  typedef typename MEDMEM_ArrayInterface<double,INTERLACING_TAG,Gauss>::Array TArrayGauss;

  vector<int>  nbelgeoc, nbgaussgeo;

  nbelgeoc.resize(nb_type+1, 0);
  nbgaussgeo.resize(nb_type+1, 0);

  for ( int iType = 0 ; iType < nb_type ; iType++ ) {

    medGeometryElement elem_type = types[iType] ;
    if(elem_type == MED_EN::MED_POLYGON && elem_type == MED_EN::MED_POLYHEDRA ) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Bad cell type : "<<MED_EN::geoNames[elem_type]<<" !!! "));

    it = _gaussModel.find(elem_type);

    if(it == _gaussModel.end())
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Gauss localization not defined for "<<MED_EN::geoNames[elem_type]<<" type!!! "));
    gaussLock = static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> * > ((*it).second);

    ArrayCoord coord = gaussLock->getGsCoo();
    double* gaussCoord = new double[coord.getNbElem()*coord.getDim()];
    int idx = 0;
    for( int i = 1 ; i <= coord.getNbElem() ; i++ ) {
      for( int j = 1 ; j <= coord.getDim() ; j++ ) {
        gaussCoord[idx++] = coord.getIJ(i,j);
      }
    }

    idx = 0;
    ArrayCoord ref = gaussLock->getRefCoo();
    double* refCoord = new double[ref.getNbElem()*ref.getDim()];
    for( int i = 1 ; i <= ref.getNbElem() ; i++ ) {
      for( int j = 1 ; j <= ref.getDim() ; j++ ) {
        refCoord[idx++] = ref.getIJ(i,j);
      }
    }
      
    INTERP_KERNEL::NormalizedCellType normType;
    switch(elem_type) {
    case MED_EN::MED_SEG2 : normType = INTERP_KERNEL::NORM_SEG2;break;
    case MED_EN::MED_SEG3 : normType = INTERP_KERNEL::NORM_SEG3;break;
    default : normType = (INTERP_KERNEL::NormalizedCellType) ((((unsigned long)elem_type/100-2)*10) + ((unsigned long)elem_type%100));
      break;
    }
      
    calculator.addGaussInfo(normType,
                            elem_type/100,
                            gaussCoord,
                            gaussLock->getNbGauss(),
                            refCoord,
                            elem_type%100
                            );
    // Preapre Info for the gauss array
    nbelgeoc   [ iType+1 ] = nbelgeoc[ iType ] + getSupport()->getNumberOfElements(elem_type);
    nbgaussgeo [ iType+1 ] = gaussLock->getNbGauss();
    
    delete [] gaussCoord;
    delete [] refCoord;
  }

  FIELD<double, FullInterlace>* gpCoord =
    new FIELD<double, FullInterlace>(getSupport(),spaceDim);
  gpCoord->setName("Gauss Points Coordinates");
  gpCoord->setDescription("Gauss Points Coordinates");
  
  for(int dimId = 1 ; dimId <= spaceDim; dimId++) {
    switch(dimId) {
    case 1:
      gpCoord->setComponentName(dimId,"X");
      gpCoord->setComponentDescription(dimId,"X coordinate of the gauss point");
      break;
    case 2:
      gpCoord->setComponentName(dimId,"Y");
      gpCoord->setComponentDescription(dimId,"Y coordinate of the gauss point");
      break;
    case 3:
      gpCoord->setComponentName(dimId,"Z");
      gpCoord->setComponentDescription(dimId,"Z coordinate of the gauss point");
      break;
    }
    
    gpCoord->setMEDComponentUnit(dimId, mesh->getCoordinatesUnits()[dimId-1]);    
  }
  
  gpCoord->setIterationNumber(getIterationNumber());
  gpCoord->setOrderNumber(getOrderNumber());
  gpCoord->setTime(getTime());

  TArrayGauss *arrayGauss = new TArrayGauss(spaceDim, length_values,
                                            nb_type, & nbelgeoc[0], & nbgaussgeo[0]);
  gpCoord->setArray(arrayGauss);


  
    
  //Calculation of the coordinates  
  int index = 1;
  for ( int i = 0 ; i < nb_type ; i++ ) {
    
    medGeometryElement type = types[i] ;
    INTERP_KERNEL::NormalizedCellType normType;
    switch(type) {
    case MED_EN::MED_SEG2 : normType = INTERP_KERNEL::NORM_SEG2;break;
    case MED_EN::MED_SEG3 : normType = INTERP_KERNEL::NORM_SEG3;break;
    default : normType = (INTERP_KERNEL::NormalizedCellType) ((((unsigned long)type/100-2)*10) + ((unsigned long)type%100));
      break;
    }
    
    it = _gaussModel.find(type);
    
    gaussLock = static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> * > ((*it).second);
    int nb_entity_type = getSupport()->getNumberOfElements(type);
    
    
    if (isOnAll) {
      global_connectivity = mesh->getConnectivity(MED_EN::MED_NODAL,support_entity,type);
    }
    else {
      const int * supp_number = getSupport()->getNumber(type);
      const int * connectivity = mesh->getConnectivity(MED_EN::MED_NODAL,support_entity,MED_ALL_ELEMENTS);
      const int * connectivityIndex = mesh->getConnectivityIndex(MED_EN::MED_NODAL,support_entity);
      int * global_connectivity_tmp = new int[(type%100)*nb_entity_type];
      
      for (int k_type = 0; k_type<nb_entity_type; k_type++) {
        for (int j_ent = 0; j_ent<(type%100); j_ent++) {
          global_connectivity_tmp[k_type*(type%100)+j_ent] = connectivity[connectivityIndex[supp_number[k_type]-1]+j_ent-1];
        }
      }
      global_connectivity = global_connectivity_tmp;
    }

    int nbNodes = (type%100);
    double* gCoord = NULL;
    int* Ni = NULL; 
    
    for ( int elem = 0; elem < nb_entity_type; elem++ ) {
      int elem_index = nbNodes*elem;
      Ni = new int[nbNodes];
      for( int idx = 0 ; idx < nbNodes; idx++ ) {
        Ni[idx] = global_connectivity[ elem_index+idx ] - 1;
      }
      
      gCoord = calculator.calculateCoords(normType,
                                          coord,
                                          spaceDim,
                                          Ni);
      int resultIndex = 0;
      for( int k = 0; k < gaussLock->getNbGauss(); k++ ) {
        for( int dimId = 1; dimId <= spaceDim; dimId++ ) {
          gpCoord->setValueIJK(index,dimId,(k+1),gCoord[resultIndex]);
          resultIndex++;
        }
      }
      delete [] gCoord;
      delete [] Ni;
      index++;
    }
    if (!isOnAll && type != MED_EN::MED_POLYHEDRA && type != MED_EN::MED_POLYGON) {
      delete [] global_connectivity ;
    }
  }
  END_OF_MED(LOC);
  return gpCoord;
}

/*!
  \if developper
  Destroy the MEDARRAY<T> in FIELD and put the new one without copy.
  \endif
*/
template <class T, class INTERLACING_TAG>
inline void FIELD<T, INTERLACING_TAG>::setArray(MEDMEM_Array_ * Value)
  throw (MEDEXCEPTION)
{
  if (NULL != _value) delete _value ;
  _value=Value ;
}

/*!
  \if developper
  Return a reference to  the MEDARRAY<T, INTERLACING_TAG> in FIELD.
  \endif
*/
template <class T, class INTERLACING_TAG>
inline MEDMEM_Array_ * FIELD<T, INTERLACING_TAG>::getArray() const throw (MEDEXCEPTION)
{
  const char* LOC = "MEDMEM_Array_ * FIELD<T, INTERLACING_TAG>::getArray() : ";
  BEGIN_OF_MED(LOC);
  END_OF_MED(LOC);
  return _value ;
}
template <class T,class INTERLACING_TAG>  inline
typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,Gauss>::Array *
FIELD<T, INTERLACING_TAG>::getArrayGauss() const throw (MEDEXCEPTION)
{
  const char LOC[]= "FIELD<T, INTERLACING_TAG>::getArrayGauss() : ";
  BEGIN_OF_MED(LOC);

  if ( getGaussPresence() )
    return static_cast<ArrayGauss *> (_value);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<
                                 "The field has no Gauss Point"));

  END_OF_MED(LOC);

}

template <class T,class INTERLACING_TAG>  inline
typename MEDMEM_ArrayInterface<T,INTERLACING_TAG,NoGauss>::Array *
FIELD<T, INTERLACING_TAG>::getArrayNoGauss() const throw (MEDEXCEPTION)
{
  const char LOC[]= "FIELD<T, INTERLACING_TAG>::getArrayNoGauss() : ";
  BEGIN_OF_MED(LOC);

  if ( ! getGaussPresence() )
    return static_cast < ArrayNoGauss * > (_value);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<
                                 "The field has Gauss Point"));

  END_OF_MED(LOC);
}


template <class T,class INTERLACING_TAG> inline bool
FIELD<T, INTERLACING_TAG>::getGaussPresence() const throw (MEDEXCEPTION)
  {
    if(_value != NULL)
      return _value->getGaussPresence();
    else
      throw MEDEXCEPTION("FIELD<T, INTERLACING_TAG>::getGaussPresence() const : Can't call getGaussPresence on a null _value");
  }

/*!
  Return the actual length of the reference to values array returned by getValue.
  Take care of number of components and number of Gauss points by geometric type
*/
template <class T, class INTERLACING_TAG>
inline int FIELD<T, INTERLACING_TAG>::getValueLength() const throw (MEDEXCEPTION)
{
  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->getArraySize() ;
  else
    return static_cast<ArrayNoGauss *>(_value)->getArraySize() ;
}


/*! \if MEDMEM_ug 
\defgroup FIELD_value Field values

These methods are provided for accessing the values of a field. 
There are two ways to do so : one consists in using accessors
that retrieve elements or group of elements from the entire field. Typical use is 
\verbatim
FIELD field(MED_DRIVER, "result.med","Pressure");
double P0=field.getValueIJ(1,1);
\endverbatim

Another way is to retrieve the pointer to the array that contains the 
variable values. In this case, the user should be aware of the interlacing mode
so that no mistakes are made when retrieving the values.

\verbatim
FIELD field(MED_DRIVER, "result.med","Pressure");
double* ptrP=field.getValue();
double P0=ptrP[0];
\endverbatim

@{
\endif
*/
/*!
  Returns a reference to values array to read them.
*/
template <class T, class INTERLACIN_TAG>
inline const T* FIELD<T, INTERLACIN_TAG>::getValue() const throw (MEDEXCEPTION)
{
  const char LOC[]= "FIELD<T, INTERLACING_TAG>::getValue() : ";
  BEGIN_OF_MED(LOC);
  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->getPtr() ;
  else
    return static_cast<ArrayNoGauss *>(_value)->getPtr() ;
}
/*!
  Returns a reference to \f$ i^{th} \f$ row
  of FIELD values array.
  If a faster accessor is intended you may use getArray() once,
  then MEDMEM_Array accessors.
  Be careful if field support is not on all elements getRow
  use support->getValIndFromGlobalNumber(i).
*/
template <class T,class INTERLACING_TAG> inline
const T*
FIELD<T,INTERLACING_TAG>::getRow(int i) const throw (MEDEXCEPTION)
{
  const char LOC[]= "FIELD<T,INTERLACING_TAG>::getRow(int i) : ";

  int valIndex=-1;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));

  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->getRow(valIndex) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->getRow(valIndex) ;
}

/*!
  Returns a reference to $j^{th}$ column
  of FIELD values array.
*/
template <class T,class INTERLACING_TAG> inline const T*
FIELD<T,INTERLACING_TAG>::getColumn(int j) const throw (MEDEXCEPTION)
{
  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->getColumn(j) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->getColumn(j) ;
}

/*!
  Returns the value of $i^{th}$ element and $j^{th}$ component.
  This method only works with fields having no particular Gauss point 
definition (i.e., fields having one value per element).
 This method makes the retrieval of the value independent from the
  interlacing pattern, but it is slower than the complete retrieval 
  obtained by the \b getValue() method.
*/
template <class T,class INTERLACING_TAG> inline T FIELD<T,INTERLACING_TAG>::getValueIJ(int i,int j) const throw (MEDEXCEPTION)
{
  const char LOC[]= "getValueIJ(..)";
  int valIndex=-1;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));

  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->getIJ(valIndex,j) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->getIJ(valIndex,j) ;
}

/*!
  Returns the $j^{th}$ component of $k^{th}$ Gauss points of $i^{th}$ value.
  This method is compatible with elements having more than one Gauss point.
  This method makes the retrieval of the value independent from the
  interlacing pattern, but it is slower than the complete retrieval 
  obtained by the \b getValue() method.
*/
template <class T,class INTERLACING_TAG> inline T FIELD<T,INTERLACING_TAG>::getValueIJK(int i,int j,int k) const throw (MEDEXCEPTION)
{
  const char LOC[]= "getValueIJK(..)";
  int valIndex=-1;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));

  if ( getGaussPresence() )
    return static_cast<ArrayGauss *>(_value)->getIJK(valIndex,j,k) ;
  else
    return static_cast<ArrayNoGauss *>(_value)->getIJK(valIndex,j,k) ;
}
/*! \if MEDMEM_ug @} \endif */

/*!
  Return number of values of a geomertic type in NoInterlaceByType mode
*/
template <class T, class INTERLACIN_TAG>
inline int FIELD<T, INTERLACIN_TAG>::getValueByTypeLength(int t) const throw (MEDEXCEPTION)
{
  const char LOC[]="getValueByTypeLength() : ";
  if ( getInterlacingType() != MED_EN::MED_NO_INTERLACE_BY_TYPE )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"not MED_NO_INTERLACE_BY_TYPE field" ));

  if ( getGaussPresence() ) {
    ArrayNoByTypeGauss* array = static_cast<ArrayNoByTypeGauss *>(_value);
    if (  t < 1 || t > array->getNbGeoType() )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Invalid type: "<< t ));
    return array->getLengthOfType( t );
  }
  else {
    ArrayNoByType* array = static_cast<ArrayNoByType *>(_value);
    if (  t < 1 || t > array->getNbGeoType() )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Invalid type: "<< t ));
    return array->getLengthOfType( t );
  }
}

/*!
  Return a reference to values array to read them.
*/
template <class T, class INTERLACIN_TAG>
inline const T* FIELD<T, INTERLACIN_TAG>::getValueByType(int t) const throw (MEDEXCEPTION)
{
  if ( getInterlacingType() != MED_EN::MED_NO_INTERLACE_BY_TYPE )
    throw MEDEXCEPTION(LOCALIZED("getValueByType() : not MED_NO_INTERLACE_BY_TYPE field" ));

  if ( getGaussPresence() ) {
    ArrayNoByTypeGauss* array = static_cast<ArrayNoByTypeGauss *>(_value);
    return array->getPtr() + array->getIndex( t );
  }
  else {
    ArrayNoByType* array = static_cast<ArrayNoByType *>(_value);
    return array->getPtr() + array->getIndex( t );
  }
}

/*!
  Return the value of i^{th} element in indicated type t and j^{th} component.
*/
template <class T,class INTERLACING_TAG> inline T FIELD<T,INTERLACING_TAG>::getValueIJByType(int i,int j, int t) const throw (MEDEXCEPTION)
{
  const char LOC[]= "getValueIJByType(..)";
  if ( getInterlacingType() != MED_EN::MED_NO_INTERLACE_BY_TYPE )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"not MED_NO_INTERLACE_BY_TYPE field" ));
    
  if ( getGaussPresence() )
    return static_cast<ArrayNoByTypeGauss *>(_value)->getIJByType(i,j,t) ;
  else
    return static_cast<ArrayNoByType *>(_value)->getIJByType(i,j,t) ;
}

/*!
  Return the j^{th} component of k^{th} gauss points of i^{th} value with type t.
*/
template <class T,class INTERLACING_TAG> inline T FIELD<T,INTERLACING_TAG>::getValueIJKByType(int i,int j,int k,int t) const throw (MEDEXCEPTION)
{
  const char LOC[]= "getValueIJKByType(..)";
  if ( getInterlacingType() != MED_EN::MED_NO_INTERLACE_BY_TYPE )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"not MED_NO_INTERLACE_BY_TYPE field" ));

  if ( getGaussPresence() )
    return static_cast<ArrayNoByTypeGauss *>(_value)->getIJKByType(i,j,k,t) ;
  else
    return static_cast<ArrayNoByType      *>(_value)->getIJKByType(i,j,k,t) ;
}


template <class T,class INTERLACING_TAG> const int FIELD<T,INTERLACING_TAG>::getNumberOfGeometricTypes() const throw (MEDEXCEPTION)
{
  const char LOC[]= "getNumberOfGeometricTypes(..)";
  BEGIN_OF_MED(LOC);
  if (_support)
    return _support->getNumberOfTypes();
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));
  END_OF_MED(LOC);
}

/*! \if MEDMEM_ug
\addtogroup FIELD_gauss
@{
\endif
 */

template <class T,class INTERLACING_TAG> const GAUSS_LOCALIZATION<INTERLACING_TAG> &
FIELD<T,INTERLACING_TAG>::getGaussLocalization(MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION)
{
  const char LOC[]="getGaussLocalization(MED_EN::medGeometryElement geomElement) : ";
  const GAUSS_LOCALIZATION_ * locPtr=0;

  locMap::const_iterator it;
  if ( ( it = _gaussModel.find(geomElement)) != _gaussModel.end() ) {
        locPtr = (*it).second;
        return  *static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> *>(locPtr);
  }
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Can't find any GaussLocalization on this geometric type" ));

}

template <class T,class INTERLACING_TAG> const GAUSS_LOCALIZATION<INTERLACING_TAG> *
FIELD<T,INTERLACING_TAG>::getGaussLocalizationPtr(MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION)
{
  const char LOC[]="getGaussLocalizationPtr(MED_EN::medGeometryElement geomElement) : ";
  const GAUSS_LOCALIZATION_ * locPtr=0;

  locMap::const_iterator it;
  if ( ( it = _gaussModel.find(geomElement)) != _gaussModel.end() ) {
        locPtr = (*it).second;
        return  static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> *>(locPtr);
  }
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Can't find any GaussLocalization on this geometric type" ));

}

/*!
 * \brief Return GAUSS_LOCALIZATION_* whose interlacing type may differ from one of the field
 */
template <class T,class INTERLACING_TAG> const GAUSS_LOCALIZATION_ *
FIELD<T,INTERLACING_TAG>::getGaussLocalizationRoot(MED_EN::medGeometryElement geomElement) const throw (MEDEXCEPTION)
{
  const char LOC[]="getGaussLocalizationRoot(MED_EN::medGeometryElement geomElement) : ";

  locMap::const_iterator it;
  if ( ( it = _gaussModel.find(geomElement)) != _gaussModel.end() ) {
        return (*it).second;
  }
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Can't find any GaussLocalization on this geometric type: "<< geomElement ));

}

/*!
 * \brief Take onership of GAUSS_LOCALIZATION_* whose interlacing type may differ from one of the field
 */
template <class T,class INTERLACING_TAG> void
FIELD<T,INTERLACING_TAG>::setGaussLocalization(MED_EN::medGeometryElement geomElement, GAUSS_LOCALIZATION_* gaussloc)
{
  locMap::iterator it = _gaussModel.find(geomElement);
  if ( it != _gaussModel.end() ) {
    delete it->second;
    it->second = gaussloc;
  }
  else {
    _gaussModel[ geomElement ] = gaussloc;
  }
}


template <class T,class INTERLACING_TAG> void
FIELD<T,INTERLACING_TAG>::setGaussLocalization(MED_EN::medGeometryElement geomElement, const GAUSS_LOCALIZATION<INTERLACING_TAG>& gaussloc)
{
  locMap::iterator it = _gaussModel.find(geomElement);
  if ( it != _gaussModel.end() ) {
    delete it->second;
    it->second = new GAUSS_LOCALIZATION<INTERLACING_TAG> (gaussloc);
  }
  else {
    _gaussModel[ geomElement ] = new GAUSS_LOCALIZATION<INTERLACING_TAG> (gaussloc);
  }
}

/*!
  Returns number of Gauss points for this medGeometryElement.

  Note :
  if there is no GAUSS_LOCALIZATION having this medGeometryElement but
  the medGeometryElement exist in the SUPPORT, getNumberOfGaussPoints
  return 1
*/
template <class T,class INTERLACING_TAG> const int FIELD<T,INTERLACING_TAG>::getNumberOfGaussPoints(MED_EN::medGeometryElement geomElement) const
  throw (MEDEXCEPTION)
{
  const char LOC[]="getNumberOfGaussPoints(MED_EN::medGeometryElement geomElement) : ";
  const GAUSS_LOCALIZATION_ * locPtr=0;

  locMap::const_iterator it;
  if ( ( it = _gaussModel.find(geomElement)) != _gaussModel.end() ) {
        locPtr = (*it).second;
        return  static_cast<const GAUSS_LOCALIZATION<INTERLACING_TAG> *>(locPtr)->getNbGauss();
  }
  else
    if (_support)
      try {
        if ( _support->getNumberOfElements(geomElement) ) return 1;
      } catch ( MEDEXCEPTION & ex) {
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<< "GeometricType not found !" )) ;
      }
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));

  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Should never execute this!" ));

}

/*!
  Returns number of Gauss points for each geometric type.

  Note :
  if there is no gauss points whatever the geometric type is
  it returns an exception. (renvoyer un tableau de 1 ?)
*/
template <class T,class INTERLACING_TAG> const int * FIELD<T,INTERLACING_TAG>::getNumberOfGaussPoints() const
  throw (MEDEXCEPTION)
{
  const char LOC[]="const int * getNumberOfGaussPoints(MED_EN::medGeometryElement geomElement) : ";

  if (_value)
    if ( getGaussPresence() ) {
      return static_cast<ArrayGauss *>(_value)->getNbGaussGeo()+1;
    } else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"value hasn't Gauss points " ));

    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Value not defined" ));
}

/*!
  Returns number of Gauss points for element nÂ°i.
  The i index is a global index (take care of previous element
  on different geometric type).
*/
template <class T,class INTERLACING_TAG> const int FIELD<T,INTERLACING_TAG>::getNbGaussI(int i) const throw (MEDEXCEPTION)
{
  const char LOC[]= "getNbGaussI(..)";

  int valIndex=-1;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));

  if (_value)
   if ( getGaussPresence() )
     return static_cast<ArrayGauss *>(_value)->getNbGauss(valIndex) ;
   else
     return static_cast<ArrayNoGauss *>(_value)->getNbGauss(valIndex) ;
 else
   throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"_value not defined" ));
}
/*!
@}
 */


template <class T,class INTERLACING_TAG> const int * FIELD<T,INTERLACING_TAG>::getNumberOfElements() const throw (MEDEXCEPTION)
{
  const char LOC[]= "getNumberOfElements(..)";
  BEGIN_OF_MED(LOC);
  if (_support)
    return _support->getNumberOfElements();
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));
  END_OF_MED(LOC);
}

template <class T,class INTERLACING_TAG> const MED_EN::medGeometryElement  * FIELD<T,INTERLACING_TAG>::getGeometricTypes()  const throw (MEDEXCEPTION)
{
  const char LOC[]= "getGeometricTypes(..)";
  BEGIN_OF_MED(LOC);
  if (_support)
    return _support->getTypes();
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));
  END_OF_MED(LOC);
}
template <class T,class INTERLACING_TAG> bool  FIELD<T,INTERLACING_TAG>::isOnAllElements() const throw (MEDEXCEPTION)
{
  const char LOC[]= "isOnAllElements(..)";
  BEGIN_OF_MED(LOC);
  if (_support)
    return _support->isOnAllElements();
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not defined" ));
  END_OF_MED(LOC);
}


/*! \if MEDMEM_ug
\addtogroup FIELD_value
@{
\endif */

/*!
  Copy new values array in FIELD according to the given mode.

  Array must have right size. If not results are unpredicable.
  In MED_FULL_INTERLACE mode, values are stored elementwise in X1,Y1,Z1,X2,Y2,Z2.. order.
In MED_NO_INTERLACE mode, values are stored componentwise in X1,X2,X3,...,Y1,Y2,Y3,... order.
*/
template <class T,class INTERLACING_TAG> inline void FIELD<T,INTERLACING_TAG>::setValue( T* value) throw (MEDEXCEPTION) 
{
  if ( getGaussPresence() )
    static_cast<ArrayGauss *>(_value)->setPtr(value) ;
  else
    static_cast<ArrayNoGauss *>(_value)->setPtr(value) ;
}

/*!
  Update values array in the j^{th} row of FIELD values array with the given ones and
  according to specified mode.
*/
template <class T,class INTERLACING_TAG>
inline void FIELD<T,INTERLACING_TAG>::setRow( int i, T* value) throw (MEDEXCEPTION)
{
  const char LOC[]= "FIELD<T,INTERLACING_TAG>::setRow(int i, T* value) : ";
  int valIndex=i;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not define |" ));

  if ( getGaussPresence() )
    static_cast<ArrayGauss *>(_value)->setRow(valIndex, value) ;
  else
    static_cast<ArrayNoGauss *>(_value)->setRow(valIndex, value) ;
}

/*!
  Update values array in the $j^{th}$ column of FIELD values array with the given ones and
  according to specified mode.
*/
template <class T,class INTERLACING_TAG>
inline void FIELD<T,INTERLACING_TAG>::setColumn( int j, T* value) throw (MEDEXCEPTION)
{
  if ( getGaussPresence() )
    static_cast<ArrayGauss *>(_value)->setColumn(j, value) ;
  else
    static_cast<ArrayNoGauss *>(_value)->setColumn(j, value) ;
}

/*!
  Sets the value of i^{th} element and j^{th} component with the given one.
*/
template <class T,class INTERLACING_TAG> inline void FIELD<T,INTERLACING_TAG>::setValueIJ(int i, int j, T value) throw (MEDEXCEPTION) 
{
  const char LOC[]= "FIELD<T,INTERLACING_TAG>::setValueIJ(int i, int j, T value) : ";
  int valIndex=-1;
  if (_support)
    valIndex = _support->getValIndFromGlobalNumber(i);
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not define |" ));

  if ( getGaussPresence() )
    static_cast<ArrayGauss *>(_value)->setIJ(valIndex,j,value) ;
  else
    static_cast<ArrayNoGauss *>(_value)->setIJ(valIndex,j,value) ;
}

/*!
  Set the value of i^{th} element, j^{th} component and k^{th} gauss point with the given one.
*/
template <class T,class INTERLACING_TAG> inline void FIELD<T,INTERLACING_TAG>::setValueIJK(int i, int j, int k, T value) throw (MEDEXCEPTION) 
  {
    const char LOC[]= "FIELD<T,INTERLACING_TAG>::setValueIJK(int i, int j, int k, T value) : ";
    int valIndex=-1;
    if(_support)
      valIndex = _support->getValIndFromGlobalNumber(i);
    else
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Support not define |" ));
    if(getGaussPresence())
      static_cast<ArrayGauss *>(_value)->setIJK(valIndex,j,k,value) ;
    else
      static_cast<ArrayNoGauss *>(_value)->setIJK(valIndex,j,k,value) ;
  }

/*!
  Set the value of i^{th} element and j^{th} component with the given one.
*/
template <class T,class INTERLACING_TAG> inline void FIELD<T,INTERLACING_TAG>::setValueIJByType(int i, int j, int t, T value) throw (MEDEXCEPTION) 
{
  const char LOC[]= "FIELD<T,INTERLACING_TAG>::setValueIJByType(int i, int j, int t, T value) : ";
  if ( getInterlacingType() != MED_EN::MED_NO_INTERLACE_BY_TYPE )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"not MED_NO_INTERLACE_BY_TYPE field" ));

  if ( getGaussPresence() )
    return static_cast<ArrayNoByTypeGauss *>(_value)->setIJByType(i,j,t,value) ;
  else
    return static_cast<ArrayNoByType *>(_value)->setIJByType(i,j,t,value) ;
}

/*!
  Set the value of component of k^{th} gauss points of i^{th} element and j^{th} component with the given one.
*/
template <class T,class INTERLACING_TAG> inline void FIELD<T,INTERLACING_TAG>::setValueIJKByType(int i, int j, int k, int t, T value) throw (MEDEXCEPTION) 
{
  const char LOC[]= "FIELD<T,INTERLACING_TAG>::setValueIJKByType(int i, int j, int t, int k, T value) : ";
  if ( getInterlacingType() != MED_EN::MED_NO_INTERLACE_BY_TYPE )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"not MED_NO_INTERLACE_BY_TYPE field" ));

  if ( getGaussPresence() )
    return static_cast<ArrayNoByTypeGauss *>(_value)->setIJKByType(i,j,k,t,value) ;
  else
    return static_cast<ArrayNoByType *>(_value)->setIJKByType(i,j,k,t,value) ;
}
/*! \if MEDMEM_ug @} \endif */

/*
  METHODS
*/

/*!
  Fill array by using T_Analytic.
  WARNING : "this" must have allocated its array by setting this->_support and this->_numberOfComponents properly.
  Typically you should use it on a field built with constructor FIELD<T>::FIELD<T>(SUPPORT *,int nbOfComponents)
 */
template <class T, class INTERLACING_TAG>
void FIELD<T, INTERLACING_TAG>::fillFromAnalytic(myFuncType f) throw (MEDEXCEPTION)
{
  const char LOC[]= "void FIELD<T, INTERLACING_TAG>::fillFromAnalytic(myFuncType f) : ";
  int i,j;
  if (_support == (SUPPORT *) NULL)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"No Support defined."));

  const GMESH * mesh = _support->getMesh();
  int spaceDim = mesh->getSpaceDimension();
  const double * coord;

  const double * bary;
  FIELD<double,FullInterlace> * barycenterField=0;

  double ** xyz=new double* [spaceDim];
  bool deallocateXyz=false;
  if(_support->getEntity()==MED_EN::MED_NODE)
    {
      const MESH * unstructured = _support->getMesh()->convertInMESH();
      if (_support->isOnAllElements())
        {
          coord=unstructured->getCoordinates(MED_EN::MED_NO_INTERLACE);
          for(i=0; i<spaceDim; i++)
            xyz[i]=(double *)coord+i*_numberOfValues;
        }
      else
        {
          coord = unstructured->getCoordinates(MED_EN::MED_FULL_INTERLACE);
          const int * nodesNumber=_support->getNumber(MED_EN::MED_ALL_ELEMENTS);
          for(i=0; i<spaceDim; i++)
            xyz[i]=new double[_numberOfValues];
          deallocateXyz=true;
          for(i=0;i<_numberOfValues;i++)
            {
              for(j=0;j<spaceDim;j++)
                xyz[j][i]=coord[(nodesNumber[i]-1)*spaceDim+j];
            }
        }
      unstructured->removeReference();
    }
  else
    {
      barycenterField = mesh->getBarycenter(_support);
      bary = barycenterField->getValue();
      for(i=0; i<spaceDim; i++)
        xyz[i]=new double[_numberOfValues];
      deallocateXyz=true;
      for(i=0;i<_numberOfValues;i++) {
        for(j=0;j<spaceDim;j++)
          xyz[j][i]=bary[i*spaceDim+j];
      }
    }
  T* valsToSet=(T*)getValue();
  double *temp=new double[spaceDim];
  for(i=0;i<_numberOfValues;i++)
  {
    for(j=0;j<spaceDim;j++)
      temp[j]=xyz[j][i];
    f(temp,valsToSet+i*_numberOfComponents);
  }
  delete [] temp;
  if(barycenterField)
    delete barycenterField;
  if(deallocateXyz)
    for(j=0;j<spaceDim;j++)
      delete [] xyz[j];
  delete [] xyz;
}

/*!
  Execute a function on _values on 'this' and put the result on a newly created field that has to be deallocated.
  WARNING : "this" must have allocated its array by setting this->_support and this->_numberOfComponents properly.
  Typically you should use it on a field built with constructor FIELD<T>::FIELD<T>(SUPPORT *,int nbOfComponents)
 */
template <class T, class INTERLACING_TAG>
FIELD<T,INTERLACING_TAG> *FIELD<T, INTERLACING_TAG>::execFunc(int nbOfComponents,
                                                              myFuncType2 f) throw (MEDEXCEPTION)
{
  FIELD<T,INTERLACING_TAG> *ret=new FIELD<T,INTERLACING_TAG>(_support,nbOfComponents);
  const T* valsInput=getValue();
  T* valsOutPut=(T*)ret->getValue();
  int i;
  for(i=0;i<_numberOfValues;i++)
    f(valsInput+i*_numberOfComponents,valsOutPut+i*nbOfComponents);
  return ret;
}

}//End namespace MEDMEM

#endif /* FIELD_HXX */
