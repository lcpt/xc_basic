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

#ifndef __MEDARRAY_H__
#define __MEDARRAY_H__

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_Utilities.hxx"

/*!
  A template class to generate an array of any particular type (int, long,
  float, double) for our purpose in the MED++ library.\n\n

  Arrays can be stored in MED_FULL_INTERLACE mode (ie : x1,y1,z1,x2,y2,z2...) or
  in MED_NO_INTERLACE mode ( x1,x2,..xn, y1, y2 ..,yn,z1,...,zn).\n The alternate
  representation mode is calculate ONLY when it is usefull. We assure coherency
  for minor data modifications (element, line or column) if you use set methods.
  But, if you get a pointer and modify the array, no automatical coherency is possible.
  You can use calculateOther to force a recalculation and insure the coherency.\n
  No recalculation is done, when the entire array is modified.\n
  Theses arrays are "Med like" arrays; lower bound equals 1. (first element is element 1,
  first coordinate is coordinate 1). \n

  Available constructors are :\n

  - default constructor (not very usefull)\n
  - constructor asking for array dimensions and mode (does memory allocation for you)\n
  - constructor asking for array dimensions, mode and values (doesn't do memory allocation
    but copies pointers only.)\n
  - a copy constructor which copies only pointers.\n
    (be aware of coherency)
  - a copy constructor which copies data (deepcopy).\n
  - assignement operator is also available and only copies pointers (and not data).\n\n

  Attribute "pointers" aren't standard pointers but class PointerOf objects in order to simplify
  memory management.\n

  A simple test program (testUArray) allows to test this class.
*/

namespace MEDMEM {
template <class T> class MEDARRAY
{
private :

                                /*! leading dimension of value (example : space dimension for coordinates) */
  int   _ldValues;
                                /*! length of values (example : number of nodes for coordinates) */
  int   _lengthValues;
                                /*! data access mode. possible values are :\n
                                  -  MED_FULL_INTERLACE (default mode) \n
                                  -  MED_NO_INTERLACE */
  MED_EN::medModeSwitch _mode;
                                /*! Pointer to representation in mode MED_FULL_INTERLACE */
  PointerOf <T> _valuesFull;
                                /*! Pointer to representation in mode MED_NO_INTERLACE */
  PointerOf <T> _valuesNo;
                                /*! Pointer to representation in mode _mode */
  PointerOf <T> _valuesDefault;
                                /*! Pointer to representation in the other mode (!=_mode) */
  PointerOf <T> _valuesOther;

public :

  inline  MEDARRAY();
  inline ~MEDARRAY();

  MEDARRAY  (const int ld_values, const int length_values,
             const MED_EN::medModeSwitch mode=MED_EN::MED_FULL_INTERLACE);
  MEDARRAY  (T* values, const int ld_values,
             const int length_values, const MED_EN::medModeSwitch mode=MED_EN::MED_FULL_INTERLACE,bool shallowCopy=false, bool ownershipOfValues=false);
  MEDARRAY  (MEDARRAY const &m);
  MEDARRAY  (MEDARRAY const &m, bool copyOther);
  MEDARRAY & operator = (const MEDARRAY & m);

  MEDARRAY & shallowCopy(const MEDARRAY & m);

  inline int getLeadingValue() const;
  inline int getLengthValue()  const;

  const T * get        (const MED_EN::medModeSwitch mode) ;
  const T * getRow     (const int i) ;
  const T * getColumn  (const int j) ;
  const T   getIJ (const int i, const int j) const;
//    T * const get        (const MED_EN::medModeSwitch mode) const;
//    T * const getRow     (const int i) const;
//    T * const getColumn  (const int j) const;
//    T   const getIJ (const int i, const int j) const;

  inline MED_EN::medModeSwitch getMode() const;

  void set   (const MED_EN::medModeSwitch mode,const T* value);
  void setI  (const int i,             const T* value);
  void setJ  (const int j,             const T* value);
  void setIJ (const int i, const int j, const T  value);

  void calculateOther();
  bool isOtherCalculated() const {return (const T*)_valuesOther != NULL;}
  void clearOtherMode();
};

//-------------------------------------------------//
//                                                 //
//              IMPLEMENTED CODE                   //
//                                                 //
//-------------------------------------------------//


template <class T> inline MEDARRAY<T>::MEDARRAY():
  _ldValues(0), _lengthValues(0), _mode(MED_EN::MED_FULL_INTERLACE),
  _valuesFull(), _valuesNo(),
  _valuesDefault(), _valuesOther()
{
}

//                              ------------------

template <class T> inline MEDARRAY<T>::~MEDARRAY()
{
}

//                              ------------------

                                /*! This constructor does allocation and does not set values : \n.
                                    It allocates a "T" array of length_values*ld_values length.\n
                                    You don't have to know the T values when calling this construtor
                                    but you have to call "set" method to initialize them later.
                                    You also can  get the pointer to the memory zone (with "get" method),
                                    and work with it.\n
                                    The desallocation of T array is not your responsability. \n\n
                                    Throws MEDEXCEPTION if  T array length is < 1*/

template <class T> MEDARRAY<T>::MEDARRAY(const int ld_values,
                                         const int length_values,
                                         const MED_EN::medModeSwitch mode):

                                                _ldValues(ld_values),
                                                _lengthValues(length_values),
                                                _mode(mode),
                                                _valuesFull(), _valuesNo(),
                                                _valuesDefault(),_valuesOther()
{
  //  BEGIN_OF_MED("constructor MEDARRAY<T>::MEDARRAY(const int, const int, const medModeSwitch)");

  // if ld_values < 1 or length_values < 1
  // throws an exception
  // Pointers are setted to NULL

  if ((ld_values<1)|(length_values<1))
  {
        throw MEDEXCEPTION(LOCALIZED("MEDARRAY<T>::MEDARRAY(const int, const int, const medModeSwitch) : dimension < 1 !"));
  }

  if ( _mode == MED_EN::MED_FULL_INTERLACE)
  {
        _valuesFull.set(length_values*ld_values);
        _valuesDefault.set((T*) _valuesFull);
  }
  else
  {
        ASSERT_MED (_mode == MED_EN::MED_NO_INTERLACE);
        _valuesNo.set(length_values*ld_values);
        _valuesDefault.set((T*)_valuesNo);
  }

  ASSERT_MED( (T*)_valuesDefault != NULL);
//   SCRUTE_MED((T*)_valuesDefault);
//   SCRUTE_MED((T*)_valuesOther);
//   SCRUTE_MED((T*)_valuesNo);
//   SCRUTE_MED((T*)_valuesFull);

  //  END_OF_MED("constructor MEDARRAY<T>::MEDARRAY(const int, const int, const medModeSwitch ()");
}

//                              ------------------

                                /*! This constructor duplicate T*values.\n

                                    Throws MEDEXCEPTION if  the lenght of T is < 1*/
template <class T> MEDARRAY<T>::MEDARRAY( T*values,
                                          const int ld_values,
                                          const int length_values,
                                          const MED_EN::medModeSwitch mode,
                                          bool shallowCopy,
                                          bool ownershipOfValues):
                                                _ldValues(ld_values),
                                                _lengthValues(length_values),
                                                _mode(mode),
                                                _valuesFull(),_valuesNo(),
                                                _valuesDefault(),_valuesOther()
{
  //  BEGIN_OF_MED("constructor MEDARRAY<T>::MEDARRAY(T* values, const int, const int, const medModeSwitch)");

  // if ld_values < 1 or length_values < 1, we could not allocate
  // throws an exception

  if ( (ld_values<1) | (length_values<1) )
  {
           throw MEDEXCEPTION(LOCALIZED("MEDARRAY<T>::MEDARRAY(T* values, const int, const medModeSwitch) : dimension < 1 !"));
  }
  if ( _mode == MED_EN::MED_FULL_INTERLACE)
  {
        if(shallowCopy)
          {
            if(ownershipOfValues)
              {
                _valuesFull.setShallowAndOwnership((const T*)values);
            }
            else
              {
                _valuesFull.set((const T*)values);
              }
          }
        else
          {
            _valuesFull.set(_ldValues*length_values,values);
          }
        _valuesDefault.set((T*)_valuesFull);
  }
  else
  {
        ASSERT_MED (_mode == MED_EN::MED_NO_INTERLACE);
        if(shallowCopy)
        {
          if(ownershipOfValues)
            {
              _valuesNo.setShallowAndOwnership((const T*)values);
            }
          else
            {
              _valuesNo.set((const T*)values);
            }
        }
        else
          _valuesNo.set(_ldValues*length_values,values);
        _valuesDefault.set((T*)_valuesNo);
  }
  ASSERT_MED( (T*)_valuesDefault != NULL);
//   SCRUTE_MED((T*)_valuesDefault);
//   SCRUTE_MED((T*)_valuesOther);
//   SCRUTE_MED((T*)_valuesNo);
//   SCRUTE_MED((T*)_valuesFull);

  //  END_OF_MED("constructor MEDARRAY<T>::MEDARRAY(T* values, const int, const int, const medModeSwitch)");
}

//                              ------------------

                                /*! This constructor allocates a new medarray and does a copy of pointers\n
                                    It DOES NOT copy the memory . The two objects will share the same data.\n
                                    (for copying data, use constructor MEDARRAY(MEDARRAY<T> const & m,bool copyOther). */
template <class T> MEDARRAY<T>::MEDARRAY(MEDARRAY<T> const & m ):
                                        _ldValues(m._ldValues),
                                        _lengthValues(m._lengthValues),
                                        _mode(m._mode),
                                        _valuesFull((const T*)m._valuesFull),
                                        _valuesNo((const T*)m._valuesNo),
                                        _valuesDefault((const T*)m._valuesDefault),
                                        _valuesOther((const T*)m._valuesOther)
{
  //  BEGIN_OF_MED("constructor MEDARRAY<T>::MEDARRAY(MEDARRAY<T> const & m)");
  ASSERT_MED( (T*)_valuesDefault != NULL);
//   SCRUTE_MED((T*)_valuesDefault);
//   SCRUTE_MED((T*)_valuesOther);
//   SCRUTE_MED((T*)_valuesNo);
//   SCRUTE_MED((T*)_valuesFull);
  //  END_OF_MED("constructor MEDARRAY<T>::MEDARRAY(MEDARRAY<T> const & m)");
}

                                /*! This constructor allocates a new array and does a copy of values
                                    included in the m arrays.\n
                                    If the boolean is setted to true, both representations (in full mode
                                    and no interlace mode)  will be copied.\n
                                    Otherwise, only _valuesDefault will be copied.\n
                                    Desallocation of the arrays is not your reponsability.\n\n
                                    Throws MEDEXCEPTION if _valuesOther is null and copyOther equals true*/
template <class T> MEDARRAY<T>::MEDARRAY(MEDARRAY<T> const & p,bool copyOther ):
                                        _ldValues(p._ldValues),
                                        _lengthValues(p._lengthValues),
                                        _mode(p._mode),
                                        _valuesFull(),
                                        _valuesNo(),
                                        _valuesDefault(),
                                        _valuesOther()
{
  //  BEGIN_OF_MED("Constructeur deepCopy MEDARRAY<T>::MEDARRAY(MEDARRAY<T> const & m,bool copyOther");

  // PG : Non, s'il n'y a rien, on test et on ne copie rien, c'est tout !

//    if ( copyOther==true && ((const T*)p._valuesOther==NULL))
//      {
//        throw MEDEXCEPTION("MEDARRAY MEDARRAY const &m,bool copyOther : No Other values defined and bool = true !");
//      }

  if ( _mode == MED_EN::MED_FULL_INTERLACE)
    {
      _valuesFull.set(p._ldValues*p._lengthValues,(const T*)p._valuesFull);
      _valuesDefault.set((T*)_valuesFull);
      if (copyOther)
        if ((const T*)p._valuesNo != NULL)
          {
            _valuesNo.set(p._ldValues*p._lengthValues,(const T*)p._valuesNo);
            _valuesOther.set((T*)_valuesNo);
          }
    }
  else
    {
      ASSERT_MED (_mode == MED_EN::MED_NO_INTERLACE);
      _valuesNo.set(p._ldValues*p._lengthValues,(const T*)p._valuesNo);
      _valuesDefault.set((T*)_valuesNo);
      if (copyOther)
        if ((const T*)p._valuesFull != NULL)
          {
            _valuesFull.set(p._ldValues*p._lengthValues,(const T*)p._valuesFull);
            _valuesOther.set((T*)_valuesFull);
          }
    }
}

//                              ------------------

/*! This operator makes a deep copy of the arrays.\n */

template <class T> MEDARRAY<T> & MEDARRAY<T>::operator = (const MEDARRAY & m)
{

  //  BEGIN_OF_MED("Operator = MEDARRAY<T>");

  _ldValues=m._ldValues;
  _lengthValues=m._lengthValues;
  _mode=m._mode;

  //  SCRUTE_MED(_mode);

  if ((const T*) m._valuesFull !=NULL)
    _valuesFull.set(_ldValues*_lengthValues,(const T*) m._valuesFull);

  if ((const T*) m._valuesNo !=NULL)
    _valuesNo.set(_ldValues*_lengthValues,(const T*) m._valuesNo);

  if (_mode == MED_EN::MED_FULL_INTERLACE) {
    //PN : pour enlever les warning compilateur
    //_valuesDefault.set((const T*) _valuesFull);
    //_valuesOther.set((const T*) _valuesNo);
    _valuesDefault.set((T*) _valuesFull);
    _valuesOther.set((T*) _valuesNo);
  } else {
    ASSERT_MED (_mode == MED_EN::MED_NO_INTERLACE);
    //PN : pour enlever les warning compilateur
    //_valuesDefault.set((const T*) _valuesNo);
    //_valuesOther.set((const T*) _valuesFull);
    _valuesDefault.set((T*) _valuesNo);
    _valuesOther.set((T*) _valuesFull);
  }

//   SCRUTE_MED((T*)_valuesDefault);
//   SCRUTE_MED((T*)_valuesOther);
//   SCRUTE_MED((T*)_valuesNo);
//   SCRUTE_MED((T*)_valuesFull);

  //  END_OF_MED("Operator = MEDARRAY<T>");
  return *this;
}

/*! Idem operator= but performs only shallow copy (just copy of pointers) of arrays contains in _valuesFull and _valuesNo \n
 WARNING the MEDARRAY returned HAS THE OWNERSHIP OF THE ARRAY !!!! */

template <class T> MEDARRAY<T> & MEDARRAY<T>::shallowCopy(const MEDARRAY & m)
{
  _ldValues=m._ldValues;
  _lengthValues=m._lengthValues;
  _mode=m._mode;
  if ((const T*) m._valuesFull !=NULL)
    _valuesFull.setShallowAndOwnership((const T*) m._valuesFull);
  if ((const T*) m._valuesNo !=NULL)
    _valuesNo.setShallowAndOwnership((const T*) m._valuesNo);
  if (_mode == MED_EN::MED_FULL_INTERLACE) {
    _valuesDefault.set((T*) _valuesFull);
    _valuesOther.set((T*) _valuesNo);
  } else {
    _valuesDefault.set((T*) _valuesNo);
    _valuesOther.set((T*) _valuesFull);
  }
  return *this;
}

//                              ------------------

                                /*! returns _ldValues. (for example, space dimension for coordinates array)*/
template <class T> inline int MEDARRAY<T>::getLeadingValue() const
{
  return _ldValues;
}

//                              ------------------

                                /*! returns _ldValues. (for example, number of nodes for coordinates array)*/
template <class T> inline int MEDARRAY<T>::getLengthValue() const
{
  return _lengthValues;
}

//                              ------------------

                                /*! returns a pointer to _valuesDefault or _valuesOther, depending on
                                    mode value : if mode is the same as _mode, _valuesDefault is returned.
                                    else, if _valuesOther is calculated (if necessary) and then returned.
                                    The pointer can be used to set values */
template <class T> const T* MEDARRAY<T>::get(const MED_EN::medModeSwitch mode)
{
  //  BEGIN_OF_MED("MEDARRAY<T>::get(const medModeSwitch mode)");
  if ((T*)_valuesDefault == NULL)
  {
        throw MEDEXCEPTION("MEDARRAY::get(mode) : No values defined !");
  }
  if (mode == _mode)
  {
        //PN : pour enlever les warning compilateurs
        //return (const T*)_valuesDefault;
        return  (T*) _valuesDefault;
  }
  else
  {
        if ((T*)_valuesOther == NULL)
        {
                calculateOther();
        }
        //PN : pour enlever les warning compilateurs
        //return (const T*)_valuesDefault;
        return  (T*) _valuesOther;
  }
}

//                              ------------------

                                /*! returns a pointer to ith element of the array.
                                    (ith line in a MED_FULL_INTERLACE representation )\n
                                    Be aware : if _mode is MED_NO_INTERLACE, the entire
                                    array will be recalculate in MED_FULL_INTERLACE representation.\n*/

template <class T> const T* MEDARRAY<T>::getRow(const int i)
{
  if ((T*)_valuesDefault == NULL)
  {
        throw MEDEXCEPTION("MEDARRAY::getRow(i) : No values defined !");
  }
  if (i<1)
  {
        throw MEDEXCEPTION("MEDARRAY::getRow(i) : argument i must be >= 1");
  }
  if (i>_lengthValues)
  {
        throw MEDEXCEPTION("MEDARRAY::getRow(i) : argument i must be <= _lengthValues");
  }

  if ((T*)_valuesFull == NULL)
  {
        calculateOther();
  }
  ASSERT_MED((T*)_valuesFull != NULL);

  // PN pour enlever les warning compilateurs
  //const T* ptr = (const T*)_valuesFull + (i-1)*_ldValues;
  const T* ptr =  (T*) _valuesFull + (i-1)*_ldValues;

  return ptr;
}
//                              ------------------

                                /*! this method is similar to getRow method.\n
                                    It returns a pointer to jth line of the array in a MED_NO-INTERLACE representation
                                    (for example, 2nd coordinates).\n
                                    Be aware : if _mode is MED_FULL_INTERLACE, the entire
                                    array will be recalculate in MED_NO_INTERLACE representation.\n*/

template <class T> const T* MEDARRAY<T>::getColumn(const int j)
{
  if ((T*)_valuesDefault == NULL)
  {
        throw MEDEXCEPTION("MEDARRAY::getColumn(j) : No values defined !");
  }
  if (j<1)
  {
        throw MEDEXCEPTION("MEDARRAY::getColumn(j) : argument j must be >= 1");
  }
  if (j>_ldValues)
  {
        throw MEDEXCEPTION("MEDARRAY::getColumn(j) : argument j must be <= _ldValues");
  }

  if ((T*)_valuesNo == NULL)
  {
        ASSERT_MED(((T*) _valuesDefault)==((T*) _valuesFull));
        calculateOther();
  }
  //PN pour enlever les warning compilateur
  //const T* ptr = (const T*)_valuesNo + (j-1)*_lengthValues;
  const T* ptr = ( T*)_valuesNo + (j-1)*_lengthValues;

  return ptr;
}

//                              ------------------

                                /*! returns Jth value of Ith element .\n
                                    don't forget first element is element 1 (and not element 0). */
template <class T> const T MEDARRAY<T>::getIJ(const int i,const  int j) const
{

  if (i<1)
  {
        throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument i must be >= 1");
  }
  if (i>_lengthValues)
  {
        throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument i must be <= _lengthValues");
  }
  if (j<1)
  {
        throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument j must be >= 1");
  }
  if (j>_ldValues)
  {
        throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : argument j must be <= _ldValues");
  }

  if ( (const T*)_valuesDefault ==  NULL)
  {
        throw MEDEXCEPTION("MEDARRAY::getIJ(i,j) : No value in array !");
  }

  if (_mode == MED_EN::MED_FULL_INTERLACE)
  {
        return _valuesDefault[(i-1)*_ldValues+j-1];
  }
  else
  {
        return _valuesDefault[(j-1)*_lengthValues+i-1];
  }

}

//                              ------------------

                                /*! returns the default mode (_mode)\n
                                    (internal use : needed by write method) */
template <class T> inline MED_EN::medModeSwitch MEDARRAY<T>::getMode() const
{
  return _mode;
}

//                              ------------------

                                /*! sets T pointer of _valuesDefault (cf class PointerOf) on value.\n
                                    no copy of value is done. \n
                                    the other representation mode is not recalculate.
                                    the corresponding pointers are setted to null */
//  template <class T> void MEDARRAY<T>::set(const medModeSwitch mode, const T* value)
//  {

//    BEGIN_OF_MED("MEDARRAY<T>::set(mode,value)");

//    _mode = mode;
//    if ( _mode == MED_FULL_INTERLACE)
//    {
//      _valuesFull.set(value);
//      _valuesDefault.set((T*)_valuesFull);
//      _valuesNo.set(0);
//    }
//    else
//    {
//      ASSERT_MED (_mode == MED_NO_INTERLACE);
//      _valuesNo.set(value);
//      _valuesDefault.set((T*)_valuesNo);
//      _valuesFull.set(0);
//    }
//    _valuesOther.set(0);
//    END_OF_MED("MEDARRAY<T>::set(mode,i,value)");
//  }

// set with duplication because we don't know were value come and
// MEDARRAY must have properties on it !!!!
template <class T> void MEDARRAY<T>::set(const MED_EN::medModeSwitch mode, const T* value)
{
  //  BEGIN_OF_MED("MEDARRAY<T>::set(mode,value)");

  _mode = mode;
  if ( _mode == MED_EN::MED_FULL_INTERLACE)
    {
      _valuesFull.set(_ldValues*_lengthValues,value);
      _valuesDefault.set((T*)_valuesFull);
      _valuesNo.set(0);
    }
  else
    {
      ASSERT_MED (_mode == MED_EN::MED_NO_INTERLACE);
      _valuesNo.set(_ldValues*_lengthValues,value);
      _valuesDefault.set((T*)_valuesNo);
      _valuesFull.set(0);
    }
  _valuesOther.set(0);

  //  END_OF_MED("MEDARRAY<T>::set(mode,i,value)");
}

/*! This function clears the other mode of representation if it exists
 *  It is usefull for people who needs for optimisation reasons to work directly
 *  on the inside array without using set-functions
 */
template <class T> void MEDARRAY<T>::clearOtherMode()
{
    if(isOtherCalculated())
    {
        if ( _mode == MED_EN::MED_FULL_INTERLACE)
            _valuesNo.set(0);
        else
            _valuesFull.set(0);
        _valuesOther.set(0);
    }
}


//                              ------------------

                                        /*! Sets ith element to T* values\n
                                            if they both exist, both _valuesFull and _valuesNo arrays will be updated.\n
                                            Throws exception if i < 1 or i > _lengthValues */
template <class T> void MEDARRAY<T>::setI(const int i, const T* value)
{
  //  BEGIN_OF_MED("MEDARRAY<T>::setI(i,value)");

  if ((T*)_valuesDefault == NULL)
  {
        throw MEDEXCEPTION("MEDARRAY::setI(i,value) : No values defined !");
  }
  if (i<=0)
  {
      throw MEDEXCEPTION("MEDARRAY::setI(i,value) : argument i must be > 0");
  }
  if ( i > _lengthValues)
  {
     throw MEDEXCEPTION("MEDARRAY::setI(i,value) : argument i must be <= _lenghtValues");
  }

  if ((T*)_valuesFull != NULL)
  {
   for (int k = 0;k<_ldValues;k++)
   {
                _valuesFull[k+_ldValues*(i-1)] = value[k];
   }
  }

  if ((T*)_valuesNo != NULL)
  {
   for (int k = 0;k<_ldValues;k++)
   {
                _valuesNo[k*_lengthValues +(i-1)] = value[k];
   }
  }

  //  END_OF_MED("MEDARRAY::setI(i,value)");
}
//                              ------------------

                                        /*! Sets ith element to T* values\n
                                            if they both exist, both _valuesFull and _valuesNo arrays will be updated.\n
                                            Throws exception if i < 1 or i > _lengthValues */
template <class T> void MEDARRAY<T>::setJ(const int j, const T* value)
{
  //  BEGIN_OF_MED("MEDARRAY::setJ(j,value)");
  if (( T*)_valuesDefault == NULL)
  {
        throw MEDEXCEPTION("MEDARRAY::setJ(j) : No values defined !");
  }
  if (j<1)
  {
        throw MEDEXCEPTION("MEDARRAY::setJ(j) : argument j must be >= 1");
  }
  if (j>_ldValues)
  {
        throw MEDEXCEPTION("MEDARRAY::setJ(j) : argument j must be <= _ldValues");
  }
  if ((T*)_valuesFull != NULL)
  {
   for (int k = 0;k<_lengthValues;k++)
   {
                _valuesFull[k*_ldValues+(j-1)] = value[k];
   }
  }

  if ((T*)_valuesNo != NULL)
  {
   for (int k = 0;k<_lengthValues;k++)
   {
                _valuesNo[k+_lengthValues*(j-1)] = value[k];
   }
  }
  //  END_OF_MED("MEDARRAY::setJ(j,value)");
}

//                              ------------------

                                        /*! Sets value of Jth coordinate of Ith element to T value.\n
                                            Maintains coherency.\n
                                            Throws exception if we don't have
                                            1<=i<=_lengthValues and 1<=j<=_ldValues */
template <class T> void MEDARRAY<T>::setIJ(const int i, const int j, const T value)
{
  // 1<=i<=_lengthValues and 1<=j<=_ldValues

  if (i<1)
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument i must be >= 1");
  if (i>_lengthValues)
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument i must be <= _lengthValues");

  if (j<1)
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument j must be >= 1");
  if (j>_ldValues)
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : argument j must be <= _ldValues");

  if ((T*)_valuesDefault == NULL)
  {
    throw MEDEXCEPTION("MEDARRAY::setIJ(i,j,value) : No value in array !");
  }

  if ((T*)_valuesFull != NULL)
  {
        _valuesFull[j-1+_ldValues*(i-1)] = value;
  }
  if ((T*)_valuesNo != NULL)
  {
        _valuesNo[(j-1)*_lengthValues+i-1] = value;
  }
}

                                        /*! Calculates the other mode of representation : MED_FULL_INTERLACE
                                            if __mode = MED_NO_INTERLACE and vice versa.\n
                                            Throws exception if no value are setted */
template <class T> void MEDARRAY<T>::calculateOther()
{
  //  BEGIN_OF_MED("MEDARRAY<T>::calculateOther()");
  if ((T*)_valuesDefault == NULL)
  {
        throw MEDEXCEPTION("MEDARRAY::calculateOther() : No values defined !");
  }

  if ((T*)_valuesOther == NULL)
  {
        _valuesOther.set(_ldValues*_lengthValues);
  }
  if (_mode == MED_EN::MED_NO_INTERLACE)
  {
        _valuesFull.set((T*)_valuesOther);
  }
  else
  {
        ASSERT_MED( _mode==MED_EN::MED_FULL_INTERLACE);
        _valuesNo.set((T*)_valuesOther);
  }

  for (int i=0; i<_lengthValues;i++)
  {
        for (int j=0; j<_ldValues; j++)
        {
                if (_mode == MED_EN::MED_NO_INTERLACE)
                {
                        _valuesFull[i*_ldValues+j] = _valuesNo[j*_lengthValues+i];
                }
                else
                {
                        _valuesNo[j*_lengthValues+i]=_valuesFull[i*_ldValues+j];
                }
        }
  }
  //  END_OF_MED("MEDARRAY<T>::calculateOther()");
}

} //End of namespace MEDMEM

# endif         /* # ifndef __MEDARRAY_H__ */
