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
 File MedException.cxx
*/

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_Exception.hxx"

using namespace std;
using namespace MEDMEM;

extern "C"
{
#include <math.h>
#include <stdio.h>
#include <string.h>
}

/*!
  \internal
  Function used to duplicate char * 
*/
char* duplicate( const char *const str ) ;
char* duplicate( const char *const str )
{
        ASSERT_MED(str!=NULL) ;
        const size_t length = strlen( str ) ;
        ASSERT_MED(length>0) ;
        char *new_str = new char[ 1+length ] ;
        ASSERT_MED(new_str) ;
        strcpy( new_str , str ) ;
        return new_str ;
}

/*!
  \internal Default Constructor (Should not be used)
*/
// ------------------------------------------------------ //
MEDEXCEPTION::MEDEXCEPTION( void ): exception() , _text(0)
// ------------------------------------------------------ //
{
  MESSAGE_MED( "You must user the standard builder : MEDEXCEPTION::MEDEXCEPTION( const char *text )" ) ;
  INTERRUPTION_MED(1) ;
}

namespace
{
/*!
  \internal
  Function used to elaborate the text of the MEDEXCEPTION
*/
// --------------------------------------------------------------------------------------- //
char *makeText( const char *text, const char *fileName, const unsigned int lineNumber )
// --------------------------------------------------------------------------------------- //
{
  char *newText = 0 ;

  ASSERT_MED(text) ;
  const size_t l1 = strlen(text) ;

  const char* prefix = "MED Exception" ;
  const size_t l0 = strlen(prefix) ;

  if ( fileName )
  {
      const size_t l2 = strlen(fileName) ;

      ASSERT_MED(lineNumber>=1) ;
      const size_t l3 = 1+int(log10(float(lineNumber))) ;
        
      const size_t l4 =  l0+l1+l2+l3+10+1 ;
      newText = new char [ l4 ] ;
      sprintf( newText , "%s in %s [%u] : %s" , prefix, fileName, lineNumber, text ) ;
      ASSERT_MED(newText[l4-1] == '\0' );

  }
  else
  {
      newText = new char [ l0+l1+3+1 ] ;
      sprintf( newText , "%s : %s" , prefix, text ) ;
   }
  ASSERT_MED(newText) ;
  return newText ;
}
}

/*!
    Constructor : \n
    It will create the text of the MEDEXCEPTION from the different parameters.
    It will take the form : \n
    MEDEXCEPTION, fileName, lineNumber and text of the exception
*/
// ------------------------------------------------------------------------------------------------ //
MEDEXCEPTION::MEDEXCEPTION( const char *text, const char *fileName, const unsigned int lineNumber ) : 
              exception(), _text( makeText( text , fileName , lineNumber ) )
// ------------------------------------------------------------------------------------------------ //
{
  MESSAGE_MED(_text);
}

/*!
  Destructor : \n
  If necessary desallocates Memory
*/

// ------------------------------------//
MEDEXCEPTION::~MEDEXCEPTION() throw ()
// ------------------------------------//
{
  if ( _text )
    {
      delete [] _text ;
      _text = 0 ;
    }
  ASSERT_MED(_text==NULL) ;
}


/*!
  Copy Constructor : \n
  Should not be used very often
*/
// ----------------------------------------------------------------------- //
MEDEXCEPTION::MEDEXCEPTION( const MEDEXCEPTION &ex ): _text(duplicate(ex._text))
// ----------------------------------------------------------------------- //
{
  ;
}
/*!
  Operator << : put the message to the given stream.
*/
// ------------------------------------------------------- //
ostream & MEDMEM::operator<<( ostream &os , const MEDEXCEPTION &ex )
// ------------------------------------------------------- //
{
  os << ex._text ;
  return os ;
}

/*!
  Return a char * which contain the message.
*/
// ------------------------------------------------- //
const char* MEDEXCEPTION::what( void ) const throw ()
// ------------------------------------------------- //
{
  return _text ;
}


// -------------------- class MED_DRIVER_NOT_FOUND_EXCEPTION


MED_DRIVER_NOT_FOUND_EXCEPTION::MED_DRIVER_NOT_FOUND_EXCEPTION(const MED_DRIVER_NOT_FOUND_EXCEPTION &ex ):MEDEXCEPTION ( ex ) {}


MED_DRIVER_NOT_FOUND_EXCEPTION::MED_DRIVER_NOT_FOUND_EXCEPTION
(
 const char *text, const char *fileName/*=0*/, 
 const unsigned int lineNumber/*=0*/ 
 ) : MEDEXCEPTION(text, fileName, lineNumber) {}

MED_DRIVER_NOT_FOUND_EXCEPTION::~MED_DRIVER_NOT_FOUND_EXCEPTION() throw (){}
