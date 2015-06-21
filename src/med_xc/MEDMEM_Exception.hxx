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
 File MedException.hxx
*/

#ifndef MEDEXCEPTION_HXX
#define MEDEXCEPTION_HXX

#include "MEDMEM.hxx"

#include <exception>
#include <iostream>

using namespace std;

#ifndef LOCALIZED
#define LOCALIZED(message) static_cast<const char *> (message) , __FILE__ , __LINE__
#endif

#define MED_EXCEPTION MEDMEM::MEDEXCEPTION

/*!
  Class used to throws exception.\n
  Inherits from public exception.
*/
namespace MEDMEM {
  class MEDEXCEPTION;
  MEDMEM_EXPORT ostream & operator<< (ostream &os, const MEDEXCEPTION &ex );
//--------------------------------------//
  class MEDMEM_EXPORT MEDEXCEPTION : public std::exception
//--------------------------------------//
{
private :
  MEDEXCEPTION(void);

protected :
  char* _text ;

public :
  MEDEXCEPTION(const char *text, const char *fileName=0, 
               const unsigned int lineNumber=0 );
  MEDEXCEPTION(const MEDEXCEPTION &ex );
  ~MEDEXCEPTION() throw ();
  friend ostream & operator<< (ostream &os, const MEDEXCEPTION &ex );
  virtual const char *what(void) const throw () ;
} ;


//---------------------------------------------------------//
class MEDMEM_EXPORT MED_DRIVER_NOT_FOUND_EXCEPTION : public MEDEXCEPTION
//---------------------------------------------------------//
{
private :
  MED_DRIVER_NOT_FOUND_EXCEPTION(void);

public :
  MED_DRIVER_NOT_FOUND_EXCEPTION(const char *text, const char *fileName=0, 
                                 const unsigned int lineNumber=0 );
  MED_DRIVER_NOT_FOUND_EXCEPTION(const MED_DRIVER_NOT_FOUND_EXCEPTION &ex );
  ~MED_DRIVER_NOT_FOUND_EXCEPTION() throw();

};
}

#endif  /* MEDEXCEPTION_HXX */
