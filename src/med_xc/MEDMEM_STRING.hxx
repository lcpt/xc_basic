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

# ifndef MEDMEM_STRING_HXX
# define MEDMEM_STRING_HXX

#include "MEDMEM.hxx"

using namespace std;

# include <string>
# include <sstream>

/*!
 A class to generate string from any type:

 here is a simple use case
      STRING msgErr;
      msgErr << "This Object cannot be instanciated by now ! Try "<< 11 << "times just to see what happens ";
      throw SALOME_EXCEPTION (LOCALIZED(msgErr)) ;
                                                                */
namespace MEDMEM {
  class MEDMEM_EXPORT STRING : public string
  {

  private :
    ostringstream _s ;

  public :
  
    STRING() :string(), _s() 
    {
    }
    STRING(const STRING& s) :string(s)
    {
      _s << s ;
      this->string::operator =( s );
    }
    STRING& operator= (const STRING& s)
    {
      _s.str("");
      _s << s ;

      this->string::operator = ( _s.str() ) ;  // freeze is true by now

      return *this ;
    }

    ~STRING()
    {
    }

    operator const char * () const 
    {
      // return const_cast <const char *> (this->c_str()) ;
      return this->c_str();
    }

    template <class T> STRING( const T &valeur ) : string(), _s() 
    {
      _s << valeur ;

      this->string::operator =( _s.str());
    }

    template <class T> STRING & operator<<( const T &valeur )
    {
      _s << valeur ;

      this->string::operator = ( _s.str() ) ;  // freeze is true by now

      return *this ;
    }
  } ;
} 

# endif
