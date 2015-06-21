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

#include <fstream>
#include <sstream>
#include <iomanip>

#ifndef WNT
#include <fenv.h>
#endif

#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_Field.hxx"
#include "MEDMEM_EnsightUtils.hxx"

#define TStrTool _ASCIIFileReader

using namespace MED_EN;
using namespace MEDMEM_ENSIGHT;

namespace {

  // ==============================================================================
  /*!
   * \brief Iterator on specified values of a field component
   */
  template <typename T> class _SelectedValueIterator
  {
    int        myDelta;
    const T*   myPtr;
    int        myIndexDelta;
    const int* myIndex;
  public:
    _SelectedValueIterator() // by default next() returns zero
      : myDelta( 0 ),      myPtr  (_ValueIterator<T>::zeroPtr()),
        myIndexDelta( 0 ), myIndex(_ValueIterator<int>::zeroPtr()) {}

    _SelectedValueIterator(const T* values, int delta, const int* index)
      : myDelta( delta ), myPtr(values), myIndexDelta(1), myIndex(index-1) {}

    const T & next() { myIndex += myIndexDelta; return myPtr[ (*myIndex) * myDelta ]; }
  };

  // ==============================================================================
  /*!
   * \brief Container of values of any type to read/write from/to EnSight variable.
   *  It stores values relating to one element type within a part
   */
  struct _SubPartValues
  {
    _SubPart      mySubPart;
    const char *  myValues;
    medModeSwitch myInterlace;
    bool          myOwnValues; // to delete or not myValues
    string        myConstValue; // if myConstValue is set, other members are meaningless

    // for writing values of a field-on-all-entities for a group
    const int *   myNumbers;

    string        myUndefValue;
    set<int>      myUndefIndices;
    vector<int>   myPartialIndices;

    _SubPartValues(const _SubPart& sub=_SubPart())
      : mySubPart(sub), myValues(0), myInterlace(MED_EN::MED_NO_INTERLACE), myOwnValues(true) {};

    ~_SubPartValues() { clear(); }
    void clear() { if ( myOwnValues ) delete [] myValues; myValues = 0; }

    // -------------------------------------------------------------------
    //!< copying removes data from the source object
    _SubPartValues(const _SubPartValues& other)
      :mySubPart(other.mySubPart), myValues(other.myValues),
       myInterlace(other.myInterlace), myOwnValues(true)
    { ((_SubPartValues&)other).myValues=0; }

    // -------------------------------------------------------------------
    //!< Return iterator on values of a component counted from 1
    //   nbElements is a number of values of a component located one by one in
    //   not full interlace
    template <typename T> _ValueIterator<T>
    getValues( int component, int nbElements, int nbComponents ) const
    {
      const T* values = (T*) myValues;
      if ( myInterlace == MED_EN::MED_FULL_INTERLACE ) {
        values += (component - 1);
        return _ValueIterator<T>( values, nbComponents );
      }
      else {
        values += (component - 1) * nbElements;
        return _ValueIterator<T>( values, 1 );
      }
    }
    // -------------------------------------------------------------------
    //!< Return iterator on specified values of a component counted from 1.
    //   nbElements is a number of values of a component located one by one in
    //   not full interlace
    template <typename T> _SelectedValueIterator<T>
    getSelectedValues( int component, int nbElements, int nbComponents ) const
    {
      // Values are specified by myNumbers. myValues points to the value
      // relating to myNumbers[0] element.
      const T* values = (T*) myValues;
      if ( myInterlace == MED_EN::MED_FULL_INTERLACE ) {
        values -= myNumbers[0] * nbComponents;
        values += (component - 1);
        return _SelectedValueIterator<T>( values, nbComponents, myNumbers );
      }
      else {
        values -= myNumbers[0];
        values += (component - 1) * nbElements;
        return _SelectedValueIterator<T>( values, 1, myNumbers );
      }
    }
    // -------------------------------------------------------------------
    //!< Return iterators on all values of all components
    template <typename T> vector< _ValueIterator<T> >
    getAllValues( int nbElements, int nbComponents, bool add3dComp ) const
    {
      // add the 3-d component to a vector in 2D space
      int addComp = ( add3dComp && nbComponents == 2 ) ? 1 : 0;
      vector< _ValueIterator<T> > compValIt( nbComponents + addComp);
      for ( int j = 1; j <= nbComponents; ++j )
        compValIt[ j-1 ] = getValues<T>( j, nbElements, nbComponents );
      return compValIt;
    }
    // -------------------------------------------------------------------
    //!< Return iterators on specified values for all components
    template <typename T> vector< _SelectedValueIterator<T> >
    getAllSelectedValues( int nbElements, int nbComponents, bool add3dComp ) const
    {
      // add the 3-d component to a vector in 2D space
      int addComp = ( add3dComp && nbComponents == 2 ) ? 1 : 0;
      vector< _SelectedValueIterator<T> > compValIt( nbComponents + addComp);
      for ( int j = 1; j <= nbComponents; ++j )
        compValIt[ j-1 ] = getSelectedValues<T>( j, nbElements, nbComponents );
      return compValIt;
    }
  };

  //=======================================================================
  /*!
    use file reader to get values of a _SubPart
  */
  template <class FileReader>
  void readSubPartValues( FileReader&      reader,
                          const FIELD_*    field,
                          _SubPartValues & subValues)
  {
    medEntityMesh entity = field->getSupport()->getEntity();
    int     nbComponents = field->getNumberOfComponents();

    int nbElements = 0;
    if ( entity == MED_EN::MED_NODE )
      nbElements = subValues.mySubPart.myNbNodes;
    else
      nbElements = subValues.mySubPart.myNbCells;

    int nbValues = nbElements * nbComponents;
    if ( nbValues < 1 )
      return;

    const char* undefValue = 0;
    set<int>* undefIndices = &subValues.myUndefIndices;
    vector<int>* partial   = 0;
    if ( !subValues.myUndefValue.empty() )
      undefValue = subValues.myUndefValue.c_str();
    if ( !subValues.myPartialIndices.empty() )
      partial = &subValues.myPartialIndices;

    if ( field->getValueType() == MED_REEL64 )
      subValues.myValues = reader.template convertReals<double>( nbValues,
                                                                 undefValue, undefIndices,
                                                                 partial, nbComponents );
    else if ( field->getValueType() == MED_EN::MED_INT32 )
      subValues.myValues = reader.template convertReals<int>   ( nbValues,
                                                                 undefValue, undefIndices,
                                                                 partial, nbComponents );
    else
      subValues.myValues = reader.template convertReals<long>  ( nbValues,
                                                                 undefValue, undefIndices,
                                                                 partial, nbComponents );
    // convert partial indices into undef indices
    if ( !subValues.myPartialIndices.empty() )
    {
      // sort partial
      set<int> sortedPartial;
      vector<int>::iterator p = partial->begin(), pEnd = partial->end();
      while ( p != pEnd )
        sortedPartial.insert( sortedPartial.end(), *p++ );
      partial->clear();

      // fill undef indices
      set<int> & undef = subValues.myUndefIndices;
      set<int>::iterator sp = sortedPartial.begin();
      int i = 1;
      for ( ; sp != sortedPartial.end() && i <= nbElements; ++i, ++sp ) {
        while ( i < *sp )
          undef.insert( undef.end(), i++ );
      }
      while ( i <= nbElements )
        undef.insert( undef.end(), i++ );
    }
  }
  //=======================================================================
  /*!
    set values to an array
  */
  template <class T, class TMailIter, class TArray >
  void _setValuesToArray( vector< _ValueIterator<T> > & compValIt,
                          const int                     nbValues,
                          TArray*                       array,
                          TMailIter&                    cellIt,
                          _Support*                     interSupport,
                          const set<int>&               localUndefIndices,
                          set<int> &                    globalUndefIndices)
  {
    int nbComponents = array->getDim();
    if ( localUndefIndices.empty() )
    {
      for ( int c = 0; c < nbValues; ++c, ++cellIt )
      {
        int i = interSupport->getIndex( *cellIt );
        for ( int j = 1; j <= nbComponents; ++j )
          array->setIJ( i, j, compValIt[ j ].next() );
      }
    }
    else
    {
      // set values from the first till undefined end
      set<int>::const_iterator undef = localUndefIndices.begin();
      int c = 1, last = min( nbValues, *localUndefIndices.rbegin() );
      for ( ; c <= last; ++c, ++cellIt )
      {
        int i = interSupport->getIndex( *cellIt );
        if ( c == *undef ) {
          globalUndefIndices.insert( i );
          ++undef;
        }
        for ( int j = 1; j <= nbComponents; ++j )
          array->setIJ( i, j, compValIt[ j ].next() );
      }
      // set all the rest values
      for ( ; c <= nbValues; ++c, ++cellIt )
      {
        int i = interSupport->getIndex( *cellIt );
        for ( int j = 1; j <= nbComponents; ++j )
          array->setIJ( i, j, compValIt[ j ].next() );
      }
    }
  }

  //================================================================================
  /*!
   * \brief creates a new support excluding undef indices
   */
  //================================================================================

  SUPPORT* makeShiftedSupport(const SUPPORT* support,
                              const set<int> undefIndices)
  {
    int nbElements = support->getNumberOfElements(MED_ALL_ELEMENTS);
    int nbTypes    = support->getNumberOfTypes();

    int i, shitf = 1;
    set<int>::const_iterator undef;

    // shift support numbers
    int * index  = new int[ nbTypes + 1 ];
    int * number = new int[ nbElements - undefIndices.size() ];
    if ( support->isOnAllElements() ) {
      // make shifted number
      int * numberPtr = number;
      for ( i = 0, undef = undefIndices.begin(); undef != undefIndices.end(); ++undef )
        while ( ++i < *undef )
          *numberPtr++ = i;
      while ( ++i <= nbElements )
        *numberPtr++ = i;
      // fill index but without shift
      const int * oldNbElemByType = support->getNumberOfElements();
      index[0] = 1;
      for ( int t = 0; t < nbTypes; ++t )
        index[ t+1 ] = index[ t ] + oldNbElemByType[ t ];
    }
    else {
      // shift existing number
      shitf = 1;
      const int * oldNumber = support->getNumber( MED_ALL_ELEMENTS );
      std::copy( oldNumber, oldNumber + *undefIndices.begin()-1, number ); // copy [0:firstUndef]
      for ( undef = undefIndices.begin(); undef != undefIndices.end(); ) {
        i = *undef++;
        int nextUndef = ( undef != undefIndices.end() ) ? *undef : nbElements + 1;
        while ( ++i < nextUndef )
          number[ i-1 - shitf ] = oldNumber[ i-1 ]; // shift number
        ++shitf;
      }
      // copy index
      const int * oldIndex  = support->getNumberIndex();
      std::copy( oldIndex, oldIndex + nbTypes + 1, index );
    }
    // update index
    {
      set<int>::const_reverse_iterator undef = undefIndices.rbegin();
      for ( ; undef != undefIndices.rend(); ++undef ) {
        for ( int t = nbTypes; t ; --t )
          if ( *undef < index[ t ] )
            --index[ t ];
          else
            break;
      }
    }
    // count number of elements by type
    vector<int> nbElem( nbTypes );
    for ( int t = 0; t < nbTypes; ++t )
      nbElem[ t ] = index[ t+1 ] - index[ t ];

    SUPPORT* newSup = new SUPPORT;
    newSup->setMesh                 ( support->getMesh() );
    newSup->setNumberOfGeometricType( nbTypes );
    newSup->setGeometricType        ( support->getTypes() );
    newSup->setNumberOfElements     ( &nbElem[0] );
    newSup->setNumber               ( index, number, /*shallowCopy =*/ true );
    newSup->setEntity               ( support->getEntity() );
    newSup->setAll                  ( false );

    return newSup;
  }

  //=======================================================================
  /*!
    set values to a field
  */
  template <class T, class INTERLACE>
  void _setValuesToField( FIELD<T,INTERLACE>*    field,
                          _Support*              interSupport,
                          list<_SubPartValues> & subPartValues )
  {
    medEntityMesh entity = field->getSupport()->getEntity();
    SUPPORT* support = interSupport->medSupport( entity );
    field->setSupport( new SUPPORT( *support ));
    field->getSupport()->removeReference(); // support belongs to field only

    int j, nbComponents = field->getNumberOfComponents();
    int      nbElements = field->getSupport()->getNumberOfElements(MED_ALL_ELEMENTS);

    typedef typename MEDMEM_ArrayInterface<T,INTERLACE,NoGauss>::Array TArray;

    const string& constValue = subPartValues.front().myConstValue;
    if ( !constValue.empty() )
    {
      vector<T> values(nbComponents * nbElements, (T)atof(constValue.c_str()));
      field->setArray( new TArray( &values[0], nbComponents, nbElements ));
      field->setNumberOfValues( nbElements );
      return;
    }

    field->allocValue( nbComponents, nbElements );
    TArray * array = field->getArrayNoGauss();

    set<int> undefIndices; // indices of undefined values
    
    // loop on subParts conatining ensight field values
    list<_SubPartValues>::iterator subValue = subPartValues.begin();
    for ( ; subValue != subPartValues.end(); ++subValue )
    {
      _SubPart & subPart = subValue->mySubPart;
      int nbValues = entity==MED_EN::MED_NODE ? subPart.myNbNodes : subPart.myNbCells;
      if ( nbValues == 0 )
        continue;
      // value iterator for each component
      vector< _ValueIterator<T> > compValIt( nbComponents+1 );
      for ( j = 1; j <= nbComponents; ++j )
        compValIt[ j ] = subValue->template getValues<T>( j, nbValues, nbComponents );

      // Set values

      if ( entity == MED_EN::MED_NODE ) {
        map< int, _noeud >::iterator inode = subPart.myFirstNode;
        _setValuesToArray( compValIt, nbValues, array, inode, interSupport,
                           subValue->myUndefIndices, undefIndices );
      }
      else {
        _groupe::TMailleIter cell = subPart.myFirstCell;
        _setValuesToArray( compValIt, nbValues, array, cell, interSupport,
                           subValue->myUndefIndices, undefIndices );
      }
    }

    if ( !undefIndices.empty() )
    {
      // Some values are undefined; it's necessary to compact values in the array
      // and create a new support of defined numbers only.

      // shift values
      int i, shitf = 1;
      set<int>::iterator undef = undefIndices.begin();
      while ( undef != undefIndices.end() ) {
        i = *undef++;
        int nextUndef = ( undef != undefIndices.end() ) ? *undef : nbElements + 1;
        while ( ++i < nextUndef ) {
          for ( int j = 1; j <= nbComponents; ++j )
            array->setIJ( i - shitf, j, array->getIJ( i, j )); // shift value
          //number[ i-1 - shitf ] = oldNumber[ i-1 ]; // shift number
        }
        ++shitf;
      }
      array->_nbelem -= undefIndices.size();
      array->_arraySize = array->_nbelem * nbComponents;

      support = makeShiftedSupport( support, undefIndices );
      support->setName( STRING("Partial for ") << field->getName() );
      field->setSupport( support );
      field->setNumberOfValues( array->_nbelem );
    }
    if ( support->getName().empty() )
      support->setName( STRING("Support for ") << field->getName() );
  }

  //=======================================================================
  /*!
    set values to a field
  */
  void setValuesToField( FIELD_*                field,
                         _Support*              interSupport,
                         list<_SubPartValues> & subPartValues )
  {
    switch ( field->getInterlacingType() ) {
    case MED_EN::MED_FULL_INTERLACE:
      if ( field->getValueType() == MED_REEL64 )
        _setValuesToField( static_cast< FIELD<double, FullInterlace>* >( field ),
                           interSupport, subPartValues );
      else
        _setValuesToField( static_cast< FIELD<int, FullInterlace>* >( field ),
                           interSupport, subPartValues );
      break;
    case MED_EN::MED_NO_INTERLACE:
      if ( field->getValueType() == MED_REEL64 )
        _setValuesToField( static_cast< FIELD<double, NoInterlace>* >( field ),
                           interSupport, subPartValues );
      else
        _setValuesToField( static_cast< FIELD<int, NoInterlace>* >( field ),
                           interSupport, subPartValues );
      break;
    case MED_EN::MED_NO_INTERLACE_BY_TYPE:
      if ( field->getValueType() == MED_REEL64 )
        _setValuesToField( static_cast< FIELD<double, NoInterlaceByType>* >( field ),
                           interSupport, subPartValues );
      else
        _setValuesToField( static_cast< FIELD<int, NoInterlaceByType>* >( field ),
                           interSupport, subPartValues );
      break;
    default:;
    }
  }


  //================================================================================
  /*!
   * \brief Return node numbers of a non-nodal support
   */
  //================================================================================

  int* getNodeNumbers( const SUPPORT* support, int & nbNodes )
  {
    map<int, int> nodeNumbers;
    _CaseFileDriver_User::getSupportNodes( support, nodeNumbers );
    nbNodes = nodeNumbers.size();
    int* nNumbers = new int[ nbNodes ];
    int* nPtr = nNumbers;
    map<int, int>::iterator n = nodeNumbers.begin(), nEnd = nodeNumbers.end();
    while ( n != nEnd )
      *nPtr++ = n->first, n++;

    return nNumbers;
  }

} // namespace

//=======================================================================
/*!
  Constructor.
*/
ENSIGHT_FIELD_DRIVER::ENSIGHT_FIELD_DRIVER():
  _CaseFileDriver_User(),
  _ptrField((FIELD_ *) 0)
{}
//=======================================================================
/*!
  Constructor.
*/
ENSIGHT_FIELD_DRIVER::ENSIGHT_FIELD_DRIVER(const string &  fileName,
                                           FIELD_       *  ptrField,
                                           MED_EN::med_mode_acces  accessMode):
  _CaseFileDriver_User(fileName,accessMode),
  _ptrField((FIELD_ *) ptrField)
{
}
//=======================================================================
/*!
  Copy constructor.
*/
ENSIGHT_FIELD_DRIVER::ENSIGHT_FIELD_DRIVER(const ENSIGHT_FIELD_DRIVER & fieldDriver):
  _CaseFileDriver_User(fieldDriver),
  _ptrField(fieldDriver._ptrField),
  _fieldName(fieldDriver._fieldName)
{
}
//=======================================================================
/*!
  Take missing data from other driver.
*/
void ENSIGHT_FIELD_DRIVER::merge ( const GENDRIVER& driver )
{
  _CaseFileDriver_User::merge( driver );

  const ENSIGHT_FIELD_DRIVER* other = dynamic_cast< const ENSIGHT_FIELD_DRIVER* >( &driver );
  if ( other ) {
    if ( !_ptrField )
      _ptrField = other->_ptrField;
    if ( _constantValue.empty() )
      _constantValue = other->_constantValue;
    // _fieldName is copied by GENDRIVER
  }
}
//=======================================================================
/*!
  Destructor.
*/
ENSIGHT_FIELD_DRIVER::~ENSIGHT_FIELD_DRIVER()
{
}
//=======================================================================
/*!
  Set the name of the FIELD in EnSight file
*/
void ENSIGHT_FIELD_DRIVER::setFieldName(const string & fieldName) throw (MEDEXCEPTION)
{
  const char* LOC = "ENSIGHT_FIELD_DRIVER::setFieldName(): ";
  if ( (int)fieldName.size() > MAX_FIELD_NAME_LENGTH )
    throw MEDEXCEPTION( compatibilityPb(LOC) << "too long name (> " <<
                        MAX_FIELD_NAME_LENGTH << "): " << fieldName);

  // look for illegal characters
  string::size_type pos = fieldName.find_first_of( ILLEGAL_FIELD_NAME_CHARACTERS );
  if ( pos != fieldName.npos )
    throw MEDEXCEPTION( compatibilityPb(LOC) << "Character " << pos << " (" << fieldName[pos] <<
                        ") "<< " in " << fieldName << " is not allowed in field name in EnSight\n"
                        "The illegal characters are: '" << ILLEGAL_FIELD_NAME_CHARACTERS << "'");
  _fieldName = fieldName;
}
//=======================================================================
/*!
  Check possibility to open a case file or a data file
*/
void ENSIGHT_FIELD_DRIVER::openConst(bool checkDataFile) const throw (MEDEXCEPTION)
{
  const char * LOC ="ENSIGHT_FIELD_DRIVER::open() : ";
  BEGIN_OF_MED(LOC);

  if ( checkDataFile )
  {
    if ( !getConstantValue().empty() )
      return; // constant value is either stored in case file or is read by _CaseFileDriver

    if ( getDataFileName().empty() )
      throw MED_EXCEPTION
        ( LOCALIZED( STRING(LOC) << "Internal error, variable file name is empty"));

    if (!canOpenFile( getDataFileName(), getAccessMode() ))
      throw MED_EXCEPTION
        ( LOCALIZED( STRING(LOC) << "Can not open Ensight Variable file " << getDataFileName()
                     << " in access mode " << getAccessMode()));
  }
  else
  {
    if ( getCaseFileName().empty() )
      throw MED_EXCEPTION
        ( LOCALIZED( STRING(LOC) << "Case file name is empty, "
                     "please set a correct fileName before calling open()"));

    if ( !canOpenFile( getCaseFileName(), getAccessMode() ))
      throw MED_EXCEPTION
        ( LOCALIZED( STRING(LOC) << "Can not open Ensight Case file " << getCaseFileName()
                     << " in access mode " << getAccessMode()));
  }

  END_OF_MED(LOC);
}
// ==========================================================================================
//function : ENSIGHT_FIELD_RDONLY_DRIVER

ENSIGHT_FIELD_RDONLY_DRIVER::ENSIGHT_FIELD_RDONLY_DRIVER()
  :ENSIGHT_FIELD_DRIVER(), _fieldStep(1) 
{
}

//=======================================================================
/*!
  Constructor to read a field of index-th time step
*/
//=======================================================================

ENSIGHT_FIELD_RDONLY_DRIVER::ENSIGHT_FIELD_RDONLY_DRIVER(const string & fileName,
                                                         FIELD_ *       ptrField,
                                                         int            step)
  :ENSIGHT_FIELD_DRIVER(fileName,ptrField,MED_EN::RDONLY),_fieldStep(step)
{
}

//=======================================================================
//function : ENSIGHT_FIELD_RDONLY_DRIVER
//=======================================================================

ENSIGHT_FIELD_RDONLY_DRIVER::ENSIGHT_FIELD_RDONLY_DRIVER(const ENSIGHT_FIELD_RDONLY_DRIVER & drv)
  :ENSIGHT_FIELD_DRIVER(drv),_fieldStep(drv._fieldStep)
{
}

//=======================================================================
//function : ~ENSIGHT_FIELD_RDONLY_DRIVER
//=======================================================================

ENSIGHT_FIELD_RDONLY_DRIVER::~ENSIGHT_FIELD_RDONLY_DRIVER()
{
  close();
}

//=======================================================================
//function : copy
//=======================================================================

GENDRIVER * ENSIGHT_FIELD_RDONLY_DRIVER::copy(void) const
{
  ENSIGHT_FIELD_RDONLY_DRIVER * myDriver = new ENSIGHT_FIELD_RDONLY_DRIVER(*this);

  return myDriver ;
}
//=======================================================================
//  Take missing data from other driver.
//=======================================================================

void ENSIGHT_FIELD_RDONLY_DRIVER::merge ( const GENDRIVER& driver )
{
  ENSIGHT_FIELD_DRIVER::merge( driver );

  const ENSIGHT_FIELD_RDONLY_DRIVER* other =
    dynamic_cast< const ENSIGHT_FIELD_RDONLY_DRIVER* >( &driver );
  if ( other ) {
    if ( _fieldStep < other->_fieldStep )
      _fieldStep = other->_fieldStep;
  }
}

//=======================================================================
//function : read
//=======================================================================

void ENSIGHT_FIELD_RDONLY_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  const char * LOC = "ENSIGHT_FIELD_RDONLY_DRIVER::read() : " ;
  BEGIN_OF_MED(LOC);

  _CaseFileDriver caseFile( getCaseFileName(), this);

  if ( getDataFileName().empty() ) // find out what to read
  {
    openConst(false); // check if can read case file

    caseFile.read();

    // find out index of variable to read
    int variableIndex = caseFile.getVariableIndex( _fieldName );
    if ( !variableIndex )
      variableIndex = caseFile.getVariableIndex( _ptrField->getName() );
    if ( !variableIndex ) {
      if ( !_fieldName.empty() )
        throw MEDEXCEPTION
          (LOCALIZED(STRING(LOC) << "no field found by name |" << _fieldName << "|"));
      else
        throw MEDEXCEPTION
          (LOCALIZED(STRING(LOC) << "no field found by name |" << _ptrField->getName() << "|"));
    }

    //  here data from Case File is passed:
    // * field name
    // * number of components
    // * etc.
    caseFile.setDataFileName( variableIndex, _fieldStep, this );
  }

  openConst(true); // check if can read data file

  getInterData(); // get data on nb of entities by type and on their relocation

  cout << "-> Entering into the field file " << getDataFileName() << endl  ;

  if ( !getConstantValue().empty() )
  {
    // Create a constant value field

    medEntityMesh entity = MED_EN::MED_CELL;
    GROUP* support = new GROUP;
    support->setName( string("SupportOnAll_")+entNames[entity]);
    support->setMesh( getInterData()->_medMesh );
    support->setAll( true );
    support->setEntity( entity );
    support->update();
                                    
    _groupe interGroup;
    interGroup.medGroup = support;
    _Support interSupport;
    interSupport.setGroup( &interGroup );

    list<_SubPartValues> subPartValues( 1 );
    subPartValues.back().myConstValue = getConstantValue();

    setValuesToField( _ptrField, &interSupport, subPartValues );
  }
  else
  {
    // Read values

#ifndef WNT
    int curExcept = fedisableexcept( FE_ALL_EXCEPT ); //!< there may be nan values
#endif

    if ( isBinaryDataFile( getDataFileName() ) ) // binary
    {
      if ( isGoldFormat() ) // Gold
      {
        readGoldBinary();
      }
      else // EnSight6
      {
        read6Binary();
      }
    }
    else // ASCII
    {
      if ( isGoldFormat() ) // Gold
      {
        readGoldASCII();
      }
      else // EnSight6
      {
        read6ASCII();
      }
    }

#ifndef WNT
    feclearexcept( FE_ALL_EXCEPT );
    if ( curExcept >= 0 )
      feenableexcept( curExcept );
#endif
  }
}

//=======================================================================
/*!
  Read Ensight Gold ASCII
*/
//=======================================================================

void ENSIGHT_FIELD_RDONLY_DRIVER::readGoldASCII ()
{

  // data that was set by CaseFile Driver
  medEntityMesh entity = _ptrField->getSupport()->getEntity();

  _SupportDesc         supportDescriptor;
  list<_SubPartValues> subPartValues;

  _ASCIIFileReader varFile( getDataFileName() );

  if ( isSingleFileMode() ) {
    int curTimeStep = 1;
    while ( curTimeStep < getIndexInDataFile() ) {
      while ( !varFile.isTimeStepEnd() )
        varFile.getLine();
      curTimeStep++;
    }
    while ( !varFile.isTimeStepBeginning() )
      varFile.getLine();
  }
  string description = varFile.getLine(); // description line
  _ptrField->setDescription( description );

  int partNumber = 0;
  _SubPartValues subValuesSample; // to keep interlace
  subValuesSample.myInterlace = MED_EN::MED_NO_INTERLACE;
  while ( !varFile.isTimeStepEnd() )
  {
    string word, restLine, line = varFile.getLine();
    TStrTool::split( line, word, restLine );
    if ( word == "part" ) {
      partNumber = varFile.getInt(); // part #
      continue;
    }
    subPartValues.push_back( subValuesSample );
    _SubPartValues & subValues = subPartValues.back(); 

    if ( restLine == "undef" )
      subValues.myUndefValue = varFile.getWord(); // undef

    if ( restLine == "partial" ) {
      int nbPartial = varFile.getInt(); // ne
      subValues.myPartialIndices.reserve( nbPartial );
      while ( nbPartial-- )
        subValues.myPartialIndices.push_back( varFile.getInt() ); // i_ne
    }
    _SubPartDesc desc( partNumber, word );
    subValues.mySubPart = *getSubPart( desc );
    readSubPartValues( varFile, _ptrField, subValues );
    supportDescriptor.insert( desc );
  }

  _Support* interSupport = getSupport( supportDescriptor, entity );
  setValuesToField( _ptrField, interSupport, subPartValues );
  
}

//=======================================================================
/*!
  Read Ensight GOLD binary
*/
//=======================================================================

void ENSIGHT_FIELD_RDONLY_DRIVER::readGoldBinary()
{

  // data that was set by CaseFile Driver
  medEntityMesh entity = _ptrField->getSupport()->getEntity();

  _SupportDesc         supportDescriptor;
  list<_SubPartValues> subPartValues;

  _BinaryFileReader varFile( getDataFileName() );

  // check if swapping bytes needed
  try {
    skipTimeStamp( varFile );
  }
  catch (...) {
    varFile.swapBytes();
    varFile.rewind();
  }
  if ( getIndexInDataFile() <= 1 )
    varFile.rewind();

  if ( isSingleFileMode() ) {
    // one time step may be skipped by skipTimeStamp()
    int curTimeStep = varFile.getPosition() ? 2 : 1 ;
    while ( curTimeStep < getIndexInDataFile() ) {
      skipTimeStamp( varFile );
      curTimeStep++;
    }
    varFile.skipTimeStepBeginning();
  }
  TStrOwner description( varFile.getLine() ); // description line
  _ptrField->setDescription( description.myValues );

  int partNumber = 0;
  _SubPartValues subValuesSample; // to keep interlace
  subValuesSample.myInterlace = MED_EN::MED_NO_INTERLACE;
  while ( !varFile.eof() )
  {
    TStrOwner line( varFile.getLine() );
    if ( isTimeStepEnd( line.myValues ))
      break;
    string word, restLine;
    TStrTool::split( line.myValues, word, restLine );
    if ( word == "part" ) {
      partNumber = *TIntOwner( varFile.getInt(1)); // part #
      continue;
    }
    subPartValues.push_back( subValuesSample );
    _SubPartValues & subValues = subPartValues.back(); 

    if ( restLine == "undef" )
      subValues.myUndefValue = (STRING( *TFltOwner( varFile.getFlt(1) ))); // undef

    if ( restLine == "partial" ) {
      int nbPartial = *TIntOwner( varFile.getInt(1) ); // ne
      TIntOwner partial( varFile.getInt( nbPartial ));
      subValues.myPartialIndices.assign( partial.myValues,
                                         partial.myValues+nbPartial ); // i_ne
    }
    _SubPartDesc desc( partNumber, word );
    subValues.mySubPart = *getSubPart( desc );
    readSubPartValues( varFile, _ptrField, subValues );
    supportDescriptor.insert( desc );
  }

  _Support* interSupport = getSupport( supportDescriptor, entity );
  setValuesToField( _ptrField, interSupport, subPartValues );
  
}

//=======================================================================
/*!
  Read Ensight6 ASCII
*/
//=======================================================================

void ENSIGHT_FIELD_RDONLY_DRIVER::read6ASCII()
{

  // data that was set by CaseFile Driver
  medEntityMesh entity = _ptrField->getSupport()->getEntity();

  _SupportDesc         supportDescriptor;
  list<_SubPartValues> subPartValues;

  _ASCIIFileReader varFile( getDataFileName() );

  if ( isSingleFileMode() ) {
    int curTimeStep = 1;
    while ( curTimeStep < getIndexInDataFile() ) {
      while ( !varFile.isTimeStepEnd() )
        varFile.getLine();
      curTimeStep++;
    }
    while ( !varFile.isTimeStepBeginning() )
      varFile.getLine();
  }
  string description = varFile.getLine(); // description line
  _ptrField->setDescription( description );

  const _SubPart* subPart = 0;
  if ( entity == MED_EN::MED_NODE ) // variable per node
  {
    _SubPartDesc desc = _SubPartDesc::globalCoordDesc();
    subPart = getSubPart( desc );
    if ( subPart ) {
      supportDescriptor.insert( desc );
      _SubPartValues subValues( *subPart );
      subValues.myInterlace = MED_EN::MED_FULL_INTERLACE;
      readSubPartValues( varFile, _ptrField, subValues );
      subPartValues.push_back( subValues );
    }
  }
  int partNumber = 0;
  while ( !varFile.isTimeStepEnd() )
  {
    string word = varFile.getWord();
    if ( word == "part" ) {
      partNumber = varFile.getInt();
      continue;
    }
    _SubPartDesc desc( partNumber, word );
    supportDescriptor.insert( desc );
    subPart = getSubPart( desc );
    _SubPartValues subValues( *subPart );
    if ( desc.typeName() == "block" )
      subValues.myInterlace = MED_EN::MED_NO_INTERLACE;
    else
      subValues.myInterlace = MED_EN::MED_FULL_INTERLACE;
    readSubPartValues( varFile, _ptrField, subValues );
    subPartValues.push_back( subValues );
  }

  _Support* interSupport = getSupport( supportDescriptor, entity );
  setValuesToField( _ptrField, interSupport, subPartValues );
  
}

//=======================================================================
/*!
  Read Ensight6 binary
*/
//=======================================================================

void ENSIGHT_FIELD_RDONLY_DRIVER::read6Binary()
{

  // data that was set by CaseFile Driver
  medEntityMesh entity = _ptrField->getSupport()->getEntity();

  _SupportDesc         supportDescriptor;
  list<_SubPartValues> subPartValues;
  const _SubPart*      subPart = 0;
  int                  partNumber = 0;

  _BinaryFileReader varFile( getDataFileName() );

  // check if swapping bytes needed
  try {
    skipTimeStamp( varFile );
  }
  catch (...) {
    varFile.swapBytes();
    varFile.rewind();
  }
  if ( getIndexInDataFile() <= 1 )
    varFile.rewind();

  if ( isSingleFileMode() ) {
    // one time step may be skipped by skipTimeStamp()
    int curTimeStep = varFile.getPosition() ? 2 : 1 ;
    while ( curTimeStep < getIndexInDataFile() ) {
      skipTimeStamp( varFile );
      curTimeStep++;
    }
    varFile.skipTimeStepBeginning();
  }

  TStrOwner description( varFile.getLine() ); // description line
  _ptrField->setDescription( description.myValues );

  if ( entity == MED_EN::MED_NODE ) // global unstructured node values
  {
    _SubPartDesc desc = _SubPartDesc::globalCoordDesc();
    subPart = getSubPart( desc );
    if ( subPart ) {
      supportDescriptor.insert( desc );
      _SubPartValues subValues( *subPart );
      subValues.myInterlace = MED_EN::MED_FULL_INTERLACE;
      readSubPartValues( varFile, _ptrField, subValues );
      subPartValues.push_back( subValues );
    }
  }
  while ( !varFile.eof() )
  {
    TStrOwner line( varFile.getLine() );
    if ( isTimeStepEnd( line ))
      break;
    string word, restLine;
    TStrTool::split( line.myValues, word, restLine );
    if ( word == "part" ) {
      partNumber = atoi( restLine.c_str() );
      continue;
    }
    _SubPartDesc desc( partNumber, word );
    supportDescriptor.insert( desc );
    subPart = getSubPart( desc );
    _SubPartValues subValues( *subPart );
    if ( desc.typeName() == "block" )
      subValues.myInterlace = MED_EN::MED_NO_INTERLACE;
    else
      subValues.myInterlace = MED_EN::MED_FULL_INTERLACE;
    readSubPartValues( varFile, _ptrField, subValues );
    subPartValues.push_back( subValues );
  }

  _Support* interSupport = getSupport( supportDescriptor, entity );
  setValuesToField( _ptrField, interSupport, subPartValues );

}

//================================================================================
/*!
 * \brief Looks for beginning of the next time stamp
 */
//================================================================================

void ENSIGHT_FIELD_RDONLY_DRIVER::skipTimeStamp(_BinaryFileReader& varFile)
{
  medEntityMesh entity = _ptrField->getSupport()->getEntity();
  int     nbComponents = _ptrField->getNumberOfComponents();

  if ( isSingleFileMode() )
    varFile.skipTimeStepBeginning();

  varFile.skip( 80 ); // description

  _SubPart* subPart;
  if ( entity == MED_EN::MED_NODE && !isGoldFormat() ) { // skip values on global nodes
    subPart = getSubPart( _SubPartDesc::globalCoordDesc() );
    if ( subPart )
      varFile.skip( subPart->myNbNodes * nbComponents * sizeof(float) );
  }
  int partNumber= 0;
  while ( !varFile.eof() )
  {
    TStrOwner line( varFile.getLine() );
    if ( isTimeStepEnd( line ))
      break;

    string word, restLine;
    TStrTool::split( line.myValues, word, restLine );

    if ( word == "part" ) {
      if ( isGoldFormat() )
        partNumber = *TIntOwner( varFile.getInt(1));
      else
        partNumber = atoi( restLine.c_str() );
      continue;
    }
    if ( restLine == "undef" ) {
      varFile.skip( sizeof(int) );
    }
    if ( restLine == "partial" ) {
      int nbPartial = *TIntOwner( varFile.getInt(1)); // ne
      varFile.skip( nbPartial * sizeof(int));
    }

    _SubPartDesc desc( partNumber, word );
    subPart = getSubPart( desc );
    int nbElems = ( entity == MED_EN::MED_NODE ) ? subPart->myNbNodes : subPart->myNbCells;
    varFile.skip( nbElems * nbComponents * sizeof(float) );
  }
}

//=======================================================================
//function : write
//=======================================================================

void ENSIGHT_FIELD_RDONLY_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("ENSIGHT_FIELD_RDONLY_DRIVER::write : Can't write with a RDONLY driver !");
}

//=======================================================================
//function : ENSIGHT_FIELD_WRONLY_DRIVER
//=======================================================================

ENSIGHT_FIELD_WRONLY_DRIVER::ENSIGHT_FIELD_WRONLY_DRIVER()
  :ENSIGHT_FIELD_DRIVER()
{
}

//=======================================================================
//function : ENSIGHT_FIELD_WRONLY_DRIVER
//=======================================================================

ENSIGHT_FIELD_WRONLY_DRIVER::ENSIGHT_FIELD_WRONLY_DRIVER(const string & fileName,
                                                         const FIELD_ * ptrField)
  :ENSIGHT_FIELD_DRIVER(fileName,(FIELD_*)ptrField,MED_EN::WRONLY)
{
}

//=======================================================================
//function : ENSIGHT_FIELD_WRONLY_DRIVER
//=======================================================================


ENSIGHT_FIELD_WRONLY_DRIVER::ENSIGHT_FIELD_WRONLY_DRIVER(const ENSIGHT_FIELD_WRONLY_DRIVER & drv)
  :ENSIGHT_FIELD_DRIVER(drv)
{
}

//=======================================================================
//function : ~ENSIGHT_FIELD_WRONLY_DRIVER
//=======================================================================

ENSIGHT_FIELD_WRONLY_DRIVER::~ENSIGHT_FIELD_WRONLY_DRIVER()
{
  close();
}


//=======================================================================
//function : copy
//=======================================================================

GENDRIVER * ENSIGHT_FIELD_WRONLY_DRIVER::copy(void) const
{
  ENSIGHT_FIELD_WRONLY_DRIVER * myDriver = new ENSIGHT_FIELD_WRONLY_DRIVER(*this);

  return myDriver ;
}

//=======================================================================
//function : read
//=======================================================================

void ENSIGHT_FIELD_WRONLY_DRIVER::read (void)
  throw (MEDEXCEPTION)
{
  throw MEDEXCEPTION("ENSIGHT_FIELD_WRONLY_DRIVER::read : Can't read with a write only!");
}

//================================================================================
/*!
 * \brief Return pointer to the value of the i-th element
 */
//================================================================================
const char* getValuePointer( int i, const FIELD_* field );
const char* getValuePointer( int i, const FIELD_* field )
{
  switch ( field->getInterlacingType() ) {
  case MED_EN::MED_FULL_INTERLACE:
    if ( field->getValueType() == MED_REEL64 )
      return (const char*) & static_cast<const FIELD<double, FullInterlace>* >
        ( field )->getArrayNoGauss()->getIJ( i, 1 );
    else
      return (const char*) & static_cast<const FIELD<int, FullInterlace>* >
        ( field )->getArrayNoGauss()->getIJ( i, 1 );
  case MED_EN::MED_NO_INTERLACE:
    if ( field->getValueType() == MED_REEL64 )
      return (const char*) & static_cast<const FIELD<double, NoInterlace>* >
        ( field )->getArrayNoGauss()->getIJ( i, 1 );
    else
      return (const char*) & static_cast<const FIELD<int, NoInterlace>* >
        ( field )->getArrayNoGauss()->getIJ( i, 1 );
  case MED_EN::MED_NO_INTERLACE_BY_TYPE:
    if ( field->getValueType() == MED_REEL64 )
      return (const char*) & static_cast<const FIELD<double, NoInterlaceByType>* >
        ( field )->getArrayNoGauss()->getIJ( i, 1 );
    else
      return (const char*) & static_cast<const FIELD<int, NoInterlaceByType>* >
        ( field )->getArrayNoGauss()->getIJ( i, 1 );
  default:;
  }
  return 0;
}

//=======================================================================
/*!
 * \brief Write field values in EnSight6 ASCII format
*/
//=======================================================================

template< typename T >
void write6ASCIIValues( const _SubPartValues& subPartValues,
                        int                   nbValues,
                        int                   nbComponents,
                        int                   componentShift,
                        ofstream &            ensightDataFile )
{
  // add the 3-d component to a vector in 2D space
  const bool add3dComp = true;
  int mypoint=0;
  if ( subPartValues.myNumbers )
  {
    vector< _SelectedValueIterator<T> > compValIt =
      subPartValues.template getAllSelectedValues<T>( componentShift, nbComponents, add3dComp );
    nbComponents = compValIt.size();
    // in full interlace
    for (int i=0; i<nbValues; i++)
      for(int j=0; j<nbComponents; j++) {
        ensightDataFile << setw(12) << _toFloat( compValIt[j].next() );
        if (++mypoint == 6) {
          ensightDataFile << endl ;
          mypoint=0;
        }
      }
  }
  else {
    vector< _ValueIterator<T> > compValIt =
      subPartValues.template getAllValues<T>( componentShift, nbComponents, add3dComp );
    nbComponents = compValIt.size();
    // in full interlace
    for (int i=0; i<nbValues; i++)
      for(int j=0; j<nbComponents; j++) {
        ensightDataFile << setw(12) << _toFloat( compValIt[j].next() );
        if (++mypoint == 6) {
          ensightDataFile << endl ;
          mypoint=0;
        }
      }
  }
  if ( nbValues * nbComponents % 6 )
    ensightDataFile << endl;
}

//=======================================================================
/*!
 * \brief Write field values in EnSight GOLD ASCII format
*/
//=======================================================================

template< typename T >
void writeGoldASCIIValues( const _SubPartValues& subPartValues,
                           int                   nbValues,
                           int                   nbComponents,
                           int                   componentShift,
                           ofstream &            ensightDataFile )
{
  // in no-interlace
  for(int j=1; j<=nbComponents; j++) {
    if ( subPartValues.myNumbers )
    {
      _SelectedValueIterator<T> values
        = subPartValues.template getSelectedValues<T>( j, componentShift, nbComponents );
      for (int i=0; i<nbValues; i++)
        ensightDataFile << setw(12) << _toFloat( values.next() ) << endl;
    }
    else {
      _ValueIterator<T> values
        = subPartValues.template getValues<T>( j, componentShift, nbComponents );
      for (int i=0; i<nbValues; i++)
        ensightDataFile << setw(12) << _toFloat( values.next() )<< endl;
    }
  }
  // add the 3-d component to a vector in 2D space
  if ( nbComponents == 2 ) {
    for (int i=0; i<nbValues; i++)
      ensightDataFile << " 0.00000e+00" << endl;
  }
}

//=======================================================================
/*!
 * \brief Write field values in EnSight6 Binary format
*/
//=======================================================================

template< typename T >
void write6BinaryValues( const _SubPartValues& subPartValues,
                         int                   nbValues,
                         int                   nbComponents,
                         int                   componentShift,
                         _BinaryFileWriter &   ensightDataFile )
{
  const bool add3dComp = true;
  if ( subPartValues.myNumbers )
  {
    vector< _SelectedValueIterator<T> > compValIt =
      subPartValues.template getAllSelectedValues<T>( componentShift, nbComponents, add3dComp );
    ensightDataFile.addReal( compValIt, nbValues, MED_EN::MED_FULL_INTERLACE );
  }
  else
  {
    vector< _ValueIterator<T> > compValIt =
      subPartValues.template getAllValues<T>( componentShift, nbComponents, add3dComp );
    ensightDataFile.addReal( compValIt, nbValues, MED_EN::MED_FULL_INTERLACE );
  }
}

//=======================================================================
/*!
 * \brief Write field values in EnSight GOLD Binary format
*/
//=======================================================================

template< typename T >
void writeGoldBinaryValues( const _SubPartValues& subPartValues,
                            int                   nbValues,
                            int                   nbComponents,
                            int                   componentShift,
                            _BinaryFileWriter &   ensightDataFile )
{
  const bool add3dComp = false;
  if ( subPartValues.myNumbers )
  {
    vector< _SelectedValueIterator<T> > compValIt =
      subPartValues.template getAllSelectedValues<T>( componentShift, nbComponents, add3dComp );
    ensightDataFile.addReal( compValIt, nbValues, MED_EN::MED_NO_INTERLACE );
  }
  else
  {
    vector< _ValueIterator<T> > compValIt =
      subPartValues.template getAllValues<T>( componentShift, nbComponents, add3dComp );
    ensightDataFile.addReal( compValIt, nbValues, MED_EN::MED_NO_INTERLACE );
  }
  // add the 3-d component to a vector in 2D space
  if ( nbComponents == 2 ) {
    vector<float> values( nbValues, 0. );
    ensightDataFile.addReal( & values[0], nbValues );
  }
}

//=======================================================================
//function : write
//=======================================================================

void ENSIGHT_FIELD_WRONLY_DRIVER::write(void) const
  throw (MEDEXCEPTION)
{
  const char * LOC = "ENSIGHT_FIELD_WRONLY_DRIVER::write(void) const " ;
  BEGIN_OF_MED(LOC);

  openConst(false) ; // check if can write to case file

  // Ensight case organization requires a main file (filename.case) which defines organization

  _CaseFileDriver caseFile( getCaseFileName(), this);
  caseFile.read();
  caseFile.addField( this ); 
  caseFile.write(); // all fields of _CaseFileDriver_User are set by this method

  openConst(true) ; // check if can write to data file

  const FIELD_* field     = _ptrField;
  const SUPPORT * support = field->getSupport();
  const GMESH * mesh      = support->getMesh();

  int dt              = field->getIterationNumber();
  int it              = field->getOrderNumber();
  int nbComponents    = field->getNumberOfComponents();
  MED_EN::med_type_champ type = field->getValueType();

  medEntityMesh entity = support->getEntity();
  int totalNbValues    = support->getNumberOfElements(MED_ALL_ELEMENTS);
  //const int* mainNbValsByType = support->getNumberOfElements();

  int nbValuesByType = 0;
  int& componentShift = (type == MED_EN::MED_NO_INTERLACE_BY_TYPE) ? nbValuesByType : totalNbValues;

  bool isOnAll = support->isOnAllElements();
  if (!isOnAll && !mesh)
    throw MED_EXCEPTION(STRING("Can't write field ") << field->getName() <<
                        ", which is not on all elements while mesh is not set to its support");
  if (!isOnAll)
    isOnAll = ( mesh->getNumberOfElements(entity,MED_ALL_ELEMENTS) == componentShift );
  if (!isOnAll && entity == MED_EN::MED_NODE && !isGoldFormat() ) {
    throw MED_EXCEPTION(compatibilityPb("Can't write field ") << field->getName() <<
                        " which is not on all nodes of the mesh in EnSight6 format,"
                        " use EnSight Gold format instead");
  }
  if ( entity == MED_EN::MED_EDGE && mesh && !isToWriteEntity( MED_EN::MED_EDGE, mesh ))
    throw MED_EXCEPTION(STRING(LOC) << "Can't write field " << field->getName()
                        << ", fields on MED_EN::MED_EDGE in volumic mesh are not supported");

  // part number
  int partNum = getPartNumber( support );
  string isPartial = "";
  if ( !partNum ) {
    if ( !isGoldFormat() )
      throw MED_EXCEPTION(STRING("Can't write field ") << field->getName()
                          << " in EnSight6 format, it's support " << support->getName()
                          << " is not stored in geo file, use EnSight Gold format instead");
    isPartial = " partial";
    SUPPORT *tmpSupport=new SUPPORT;
    tmpSupport->setAll(true);
    tmpSupport->setEntity( entity );
    partNum = getPartNumber( tmpSupport );
    tmpSupport->removeReference();
  }

  // supports to write the field for
  map< int, const SUPPORT* > parts;
  map< int, const SUPPORT* >::iterator partNumIt;
  parts[ partNum ] = support;

  // In Ensight 6, nodal field is visible on all parts as nodes are global, for
  // the rest cases we write field for all groups in order to see values on them
  if ( isOnAll && ( isGoldFormat() || entity != MED_EN::MED_NODE ) && mesh )
  {
    // In Ensight Gold, to see nodal field on all groups we write them all
    int ent = entity, toEntity = entity + 1;
    if ( isGoldFormat() && entity == MED_EN::MED_NODE )
      ent = MED_EN::MED_CELL, toEntity = MED_ALL_ENTITIES;

    for ( ; ent < toEntity; ++ent ) {
      int nbGroups = mesh->getNumberOfGroups(ent);
      for ( int i=1; i<=nbGroups; i++) {
        const GROUP* group = mesh->getGroup( ent, i );
        if (  group != support && !group->isOnAllElements() ) {
          partNum = getPartNumber( group );
          if ( partNum )
            parts.insert( make_pair( partNum, group ));
        }
      }
    }
  }

  // description
  string description = field->getDescription();
  if ( description.empty() )
    description = STRING(field->getName()) << " dt=" << dt << " it=" << it;

  cout << "-> Creating the Ensight data file " << getDataFileName() << endl ;


  _SubPartValues geoTypeValues;
  geoTypeValues.myOwnValues = false;
  geoTypeValues.myInterlace = field->getInterlacingType();
  int nbWrittenValues = 0;

  if ( isBinaryEnSightFormatForWriting() )
  {
    // ======================================================
    //                          Binary
    // ======================================================

    // function to write values
    typedef void (* TWriteValues) (const _SubPartValues& , int, int, int, _BinaryFileWriter &);
    TWriteValues writeValues;
    if ( isGoldFormat() ) {
      if ( type == MED_REEL64 )     writeValues = writeGoldBinaryValues<double>;
      else if ( type == MED_EN::MED_INT32 ) writeValues = writeGoldBinaryValues<int>;
      else                          writeValues = writeGoldBinaryValues<long>;
    }
    else {
      if ( type == MED_REEL64 )     writeValues = write6BinaryValues<double>;
      else if ( type == MED_EN::MED_INT32 ) writeValues = write6BinaryValues<int>;
      else                          writeValues = write6BinaryValues<long>;
    }
    _BinaryFileWriter ensightDataFile( getDataFileName() );

    // description
    ensightDataFile.addString( description );

    for ( partNumIt = parts.begin(); partNumIt != parts.end(); ++partNumIt)
    {
      const SUPPORT* partSup = partNumIt->second;
      partNum                = partNumIt->first;
      const bool otherEntity = ( entity != partSup->getEntity() ); // Gold, nodal field
      nbWrittenValues        = 0;

      // part number
      if ( isGoldFormat() ) {
        ensightDataFile.addString("part");
        ensightDataFile.addInt( partNum );
      }
      else if ( entity != MED_EN::MED_NODE ) {
        ensightDataFile.addString( STRING("part ") << partNum );
      }
      // loop on types
      int                       nbTypes = partSup->getNumberOfTypes();
      const medGeometryElement* geoType = partSup->getTypes();
      for (int i=0; i<nbTypes; i++)
      {
        const medGeometryElement medType = geoType[i];
        nbValuesByType = partSup->getNumberOfElements(medType);

        // type name
        if ( entity != MED_EN::MED_NODE ) {
          const TEnSightElemType& ensightType = getEnSightType(medType);
          ensightDataFile.addString( ensightType._name + isPartial );
        }
        else if ( isGoldFormat() ) {
          ensightDataFile.addString( "coordinates" + isPartial );
        }

        // supporting numbers (Gold only)
        if ( !isPartial.empty() ) {
          const int *number = partSup->getNumber(medType);
          ensightDataFile.addInt( nbValuesByType );
          ensightDataFile.addInt( number, nbValuesByType );
        }

        // get field values
        if ( otherEntity ) { // Gold, nodal field, non-nodal group (partSup)
          if ( isOnAll && partSup->isOnAllElements() ) {
            geoTypeValues.myNumbers = 0;
            geoTypeValues.myValues  = getValuePointer( 1, field );
            nbValuesByType = totalNbValues;
          }
          else {
            geoTypeValues.myNumbers = getNodeNumbers( partSup, nbValuesByType );
            geoTypeValues.myValues  = getValuePointer( geoTypeValues.myNumbers[0], field );
          }
        }
        else if ( partSup->getNumberOfElements(MED_ALL_ELEMENTS) != totalNbValues ) {
          geoTypeValues.myNumbers = partSup->getNumber(medType);
          geoTypeValues.myValues  = getValuePointer( geoTypeValues.myNumbers[0], field );
        }
        else {
          geoTypeValues.myNumbers = 0;
          geoTypeValues.myValues  = getValuePointer( nbWrittenValues + 1, field );
        }
        // write values
        writeValues(geoTypeValues, nbValuesByType, nbComponents, componentShift, ensightDataFile);
        nbWrittenValues += nbValuesByType;

        if ( otherEntity ) {
          if ( geoTypeValues.myNumbers ) delete [] geoTypeValues.myNumbers;
          break; // all nodal values are written at once
        }
      }
    }
  }
  else
  {
    // ======================================================
    //                           ASCII
    // ======================================================

    // function to write values
    typedef void (* TWriteValues) (const _SubPartValues& , int, int, int, ofstream &);
    TWriteValues writeValues;
    if ( isGoldFormat() ) {
      if ( type == MED_REEL64 )     writeValues = writeGoldASCIIValues<double>;
      else if ( type == MED_EN::MED_INT32 ) writeValues = writeGoldASCIIValues<int>;
      else                          writeValues = writeGoldASCIIValues<long>;
    }
    else {
      if ( type == MED_REEL64 )     writeValues = write6ASCIIValues<double>;
      else if ( type == MED_EN::MED_INT32 ) writeValues = write6ASCIIValues<int>;
      else                          writeValues = write6ASCIIValues<long>;
    }
    const int iw = isGoldFormat() ? 10 : 8; // width of integer

    ofstream ensightDataFile( getDataFileName().c_str(), ios::out);
    ensightDataFile.setf(ios::scientific);
    ensightDataFile.precision(5);

    // description
    ensightDataFile << description << endl;

    for ( partNumIt = parts.begin(); partNumIt != parts.end(); ++partNumIt)
    {
      const SUPPORT* partSup = partNumIt->second;
      partNum                = partNumIt->first;
      const bool otherEntity = ( entity != partSup->getEntity() ); // Gold, nodal field
      nbWrittenValues        = 0;

      // part number
      if ( isGoldFormat() )
        ensightDataFile << "part" << endl << setw(iw) << partNum << endl;
      else if ( entity != MED_EN::MED_NODE ) {
        ensightDataFile << "part " << partNum << endl;
      }
      // loop on types
      int                       nbTypes = partSup->getNumberOfTypes();
      const medGeometryElement* geoType = partSup->getTypes();
      for (int i=0; i<nbTypes; i++)
      {
        const medGeometryElement medType = geoType[i];
        nbValuesByType = partSup->getNumberOfElements(medType);

        // type name
        if ( entity != MED_EN::MED_NODE ) {
          const TEnSightElemType& ensightType = getEnSightType(medType);
          ensightDataFile << ensightType._name << isPartial << endl;
        }
        else if ( isGoldFormat() ) {
          ensightDataFile << "coordinates" << isPartial << endl;
        }

        // supporting numbers (Gold only)
        if ( !isPartial.empty() ) {
          const int *number = partSup->getNumber(medType);
          const int *nEnd   = number + nbValuesByType;
          ensightDataFile << setw(iw) << nbValuesByType << endl;
          while ( number < nEnd )
            ensightDataFile << setw(iw) << *number++ << endl;
        }

        // get field values
        if ( otherEntity ) { // Gold, nodal field, non-nodal group (partSup)
          if ( isOnAll && partSup->isOnAllElements() ) {
            geoTypeValues.myNumbers = 0;
            geoTypeValues.myValues  = getValuePointer( 1, field );
            nbValuesByType = totalNbValues;
          }
          else {
            geoTypeValues.myNumbers = getNodeNumbers( partSup, nbValuesByType );
            geoTypeValues.myValues  = getValuePointer( geoTypeValues.myNumbers[0], field );
          }
        }
        else if ( partSup->getNumberOfElements(MED_ALL_ELEMENTS) != totalNbValues ) {
          geoTypeValues.myNumbers = partSup->getNumber(medType);
          geoTypeValues.myValues  = getValuePointer( geoTypeValues.myNumbers[0], field );
        }
        else {
          geoTypeValues.myNumbers = 0;
          geoTypeValues.myValues  = getValuePointer( nbWrittenValues + 1, field );
        }

        // write values
        writeValues(geoTypeValues, nbValuesByType, nbComponents, componentShift, ensightDataFile);
        nbWrittenValues += nbValuesByType;

        if ( otherEntity ) {
          if ( geoTypeValues.myNumbers ) delete [] geoTypeValues.myNumbers;
          break; // all nodal values are written at once
        }
      }
    } // loop on supports

    ensightDataFile.close();  
  }
}
