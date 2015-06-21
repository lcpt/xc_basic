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

#ifndef __PointerOf_HXX__
#define __PointerOf_HXX__

#include <cstdlib>
#include <cstring>
#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_Exception.hxx"

/*!
        The template class PointerOf embedding a standard pointer (_pointer) is in charge of
        managing the pointed memory.\n
        
        the object PointerOf is the memory owner if a size is given at object construction.
        In these cases, memory will be desallocated at object destruction. In all other cases,
        the desallocator will only nullify pointers.
*/
namespace MEDMEM {

template <typename T> 
class PointerOf
{
protected :
                        /*! pointer to the pointed memory */
        T* _pointer ;
                        /*! boolean setted to true if memory has to be desallocated */
        bool _done ;

public :
        PointerOf() ;
        ~PointerOf() ;
        PointerOf( const int &size ) ;
        PointerOf( const T *pointer ) ;
        PointerOf( const int &size, const T *pointer ) ;
        PointerOf( const PointerOf<T> & pointerOf ) ;
  ///PointerOf( const int &size, const PointerOf<T> & pointerOf ) ;
        operator T*() ;
        operator const T*() const ;
        void set( const int &size ) ;
        void set( const T *pointer ) ;
        void set( const int &size, const T *pointer ) ;
        void setShallowAndOwnership( const T *pointer );
        PointerOf<T>& operator=( const PointerOf<T> &pointer ) ;
} ;

// ------------------------------------------------------------ //
//                                                              //
//                      Implementation                          //
//                                                              //
// ------------------------------------------------------------ //

/*! Creates a null T* pointer and sets the boolean (for desallocation) to false. */
template <typename T> PointerOf<T>::PointerOf() : _pointer(0), _done(false)
{
}

/*! Creates a standard T* pointer to the pointed memory. \n
    The boolean for desallocation is setted to false. \n
    Be aware : \n
    - The "old" PointerOf always has propriety of the pointed memory. \n
    - If the "old" PointerOf is detroyed, the "new" PointerOf points
      a desallocated memory zone. */
template <typename T> PointerOf<T>::PointerOf(const PointerOf<T> & pointerOf) :
  _pointer((T*)(const T* const)pointerOf), _done(false)
{
  const char* LOC = "PointerOf<T>::PointerOf(const PointerOf<T> & pointerOf)";
  BEGIN_OF_MED(LOC);
        MESSAGE_MED("Warning ! No Propriety Transfer");
  END_OF_MED(LOC);
}

/*! 
  Duplicate array of size size pointed in pointerOf.
*/
//template <typename T> PointerOf<T>::PointerOf( const int &size, const PointerOf<T> & pointerOf) : 
//  _pointer((size,(T*)pointerOf))
//{
//}

/*! If size < 0, creates a null "T*" pointer\n
    Else allocates memory and sets desallocation boolean to true.\n
    Memory will be desallocated  when erasing this PointerOf*/
template <typename T> PointerOf<T>::PointerOf( const int &size )
{
        if (size < 0)
        {
                _pointer=(T*)NULL;
                _done=false;
        }
        else
        {
                _pointer = new T[ size ] ;
                _done=true;
        }
}

/*! Creates a standard pointer to the memory zone pointed by T*. \n
   T* owner is in charged of memory desallocation. \n
   Memory will not be released when erasing this PointerOf*/
template <typename T> PointerOf<T>::PointerOf( const T* pointer ) : _pointer( (T*)pointer ), _done(false)
{
}

/*! If size < 0, return an exception\n
    Else duplicate array and sets desallocation boolean to true.\n
    Memory will be desallocated  when erasing this PointerOf*/
template <typename T> PointerOf<T>::PointerOf( const int &size, const T* pointer)
{
  if (size < 0)
    throw MEDEXCEPTION("PointerOf( const int,const T*) : array size < 0");

  _pointer = new T[ size ] ;
  memcpy(_pointer,pointer,size*sizeof(T));
  _done=true;
}

/*! The destuctor desallocates memory if necessary (that is if the attribute _done equals true).\n
    The attribute _pointer is nullified */
template <typename T> PointerOf<T>::~PointerOf()
{
        if ( _pointer )
        {
                if( _done )
                {
                        MESSAGE_MED("PointerOf<T>::~PointerOf() --> deleting _pointer") ;
                        delete [] _pointer ;
                        _done = false ;
                }
                else
                {
                        MESSAGE_MED("_pointer is only nullified") ;
                }
                _pointer = 0 ;
        }
}

/*! Creates a standard pointer (T*) to the pointed memory. \n
    The boolean for desallocation is setted to false. \n
    Be aware : \n
    - The "right" PointerOf always has propriety of the pointed memory. \n
    - If the "right" PointerOf is detroyed, the "left" PointerOf points
      a desallocated memory zone.
    - it works the same way as PointerOf(const PointerOf<T> & pointerOf) */
template <typename T> PointerOf<T>& PointerOf<T>::operator=( const PointerOf<T> &pointer )
{
  const char* LOC = "PointerOf<T>::operator=( const PointerOf<T> &pointer )";
  BEGIN_OF_MED(LOC);
        if ( &pointer != this )
        {
                this->set( pointer._pointer ) ;
        }
  END_OF_MED(LOC);
        return *this ;
}

/*! Returns _pointer.*/
template <typename T> PointerOf<T>::operator T*()
{
        return _pointer ;
}


/*! Returns _pointer.*/
template <typename T> PointerOf<T>::operator const T*() const
{
        return _pointer ;
}

/*! If necessary, released memory holded by PointerOf\n.
    Else allocates memory and sets desallocation boolean to true.\n
    Can be used in order to "nullify" an existing PointerOf\n
    Memory will be desallocated  when erasing this PointerOf*/
template <typename T> void PointerOf<T>::set( const int &size )
{
        if ( _pointer && _done )
        {
                delete [] _pointer ;
                _pointer=0 ;
        }
        // if (size < 0) TODO: analyse why it does not work
        if (size <= 0)
        {
                _pointer=(T*)NULL;
        }
        else
        {
                _pointer = new T[ size ] ;
        }
        _done = true ;
        return ;
}

/*! If necessary, released memory holded by PointerOf\n.
    Then, sets _pointer to the memory zone pointed by T*. \n
    T* owner is in charged of memory desallocation. \n
    memory will not be released when erasing this PointerOf*/
template <typename T> void PointerOf<T>::set( const T *pointer )
{
        MESSAGE_MED( "BEGIN PointerOf<T>::set( const T *pointer )" ) ;
        SCRUTE_MED(pointer) ;
        SCRUTE_MED(_done) ;
        if ( _pointer && _done )
        {
                MESSAGE_MED("PointerOf<T>::set --> deleting _pointer") ;
                delete [] _pointer ;
                _pointer=0 ;
                _done=false ;
        }
        _pointer=(T*)pointer ;
        _done=false ;
        MESSAGE_MED( "END PointerOf<T>::set( const T *pointer )" ) ;
        return ;
}

/*! If necessary, released memory holded by PointerOf\n.
    If size < 0, return an exception\n.
    Else allocates memory and sets desallocation boolean to true.\n
    Can be used in order to "nullify" an existing PointerOf\n
    Memory will be desallocated  when erasing this PointerOf*/
template <typename T> void PointerOf<T>::set( const int &size, const T *pointer)
{
  if ( _pointer && _done )
    {
      delete [] _pointer ;
      _pointer = NULL ;
    }
  if (size < 0)
    throw MEDEXCEPTION("PointerOf( const int,const T*) : array size < 0");

  _pointer = new T[ size ] ;
  memcpy(_pointer,pointer,size*sizeof(T));
  _done=true;

  return ;
}

template <typename T> void PointerOf<T>::setShallowAndOwnership( const T *pointer )
{
  if ( _pointer && _done )
    delete [] _pointer;
  _pointer=(T*)pointer;
  _done=true;
}

}//End namespace MEDMEM

# endif         /* # if ! defined( __PointerOf_HXX__ ) */
