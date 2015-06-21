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

#ifndef __MEDMEM_UTILITIES
#define __MEDMEM_UTILITIES

// standart Linux/Unix functions 
#include <string>

#ifndef WIN32
#include <libgen.h>
#endif

namespace MEDMEM
{
  inline std::string getBaseName( const std::string& dataname )
  {
    std::string aBaseName = "";
#ifndef WIN32
    aBaseName = basename((char*)dataname.c_str());
#else
    for ( int i = dataname.size()-1; i >= 0; i-- ) {
      char aSymb = dataname[i];
      if ( dataname[i] == '\\' || dataname[i] == '/' )
        break;
      aBaseName = dataname[i] + aBaseName;
    }
#endif
    return aBaseName;
  }

  inline std::string getDirName(const std::string& dataname )
  {
    std::string aDirName = "";
#ifndef WIN32
    aDirName = dirname((char*)dataname.c_str());
#else
    bool aFindLine = false;
    for ( int i = dataname.size()-1; i >= 0; i-- ) {
      char aSymb = dataname[i];
      if ( !aFindLine )
        aFindLine = dataname[i] == '\\' || dataname[i] == '/';
      else
        aDirName = dataname[i] + aDirName;
    }
    if ( !aFindLine )
      aDirName = '.';
#endif
    return aDirName;
  }

  /*!
   * \brief Make a name valid. So far, removes white spaces from name end
   */
  inline std::string healName(const std::string& name )
  {
    size_t last = name.size()-1;
    while ( last >= 0 && ( isspace( name[last] ) || !name[last] ))
      last--;
    return name.substr( 0, last + 1 );
  }

  /*!
   * Change order of bytes for other endianness
   */
  inline int swapBytes(const int theValue)
  {
    return (0 | (( theValue & 0x000000ff ) << 24 )
            |   (( theValue & 0x0000ff00 ) << 8  )
            |   (( theValue & 0x00ff0000 ) >> 8  )
            |   (( theValue >> 24 ) & 0x000000ff ) );
  }
}

#  include <cstdlib>
#  include <iostream>
using namespace std;

/* ---  INFOS is always defined (without _DEBUG_): to be used for warnings, with release version --- */

# define HEREWEARE_MED {cout<<flush ; cerr << __FILE__ << " [" << __LINE__ << "] : " << flush ;}
# define INFOS_MED(chain) {HEREWEARE_MED ; cerr << chain << endl ;}
# define PYSCRIPT_MED(chain) {cout<<flush ; cerr << "---PYSCRIPT--- " << chain << endl ;}


/* --- To print date and time of compilation of current source on stdout --- */

# if defined ( __GNUC__ )
# define COMPILER_MED           "g++" ;
# elif defined ( __sun )
# define COMPILER_MED           "CC" ;
# elif defined ( __KCC )
# define COMPILER_MED           "KCC" ;
# elif defined ( __PGI )
# define COMPILER_MED           "pgCC" ;
# else
# define COMPILER_MED           "undefined" ;
# endif

# ifdef INFOS_COMPILATION_MED
# undef INFOS_COMPILATION_MED
# endif
# define INFOS_COMPILATION_MED  {\
  cerr << flush;\
  cout << __FILE__ ;\
  cout << " [" << __LINE__ << "] : " ;\
  cout << "COMPILED with " << COMPILER_MED ;\
  cout << ", " << __DATE__ ; \
  cout << " at " << __TIME__ << endl ;\
  cout << "\n\n" ;\
  cout << flush ;\
}

#if ( defined(_DEBUG_) || defined(_DEBUG) ) && !defined(_NO_MED_TRACE_)

/* --- the following MACROS are useful at debug time --- */

# define HERE_MED {cout<<flush ; cerr << "- Trace " << __FILE__ << " [" << __LINE__ << "] : " << flush ;}
# define SCRUTE_MED(var) {HERE_MED ; cerr << #var << "=" << var << endl ;}
# define MESSAGE_MED(chain) {HERE_MED ; cerr << chain << endl ;}
# define INTERRUPTION_MED(code) {HERE_MED ; cerr << "INTERRUPTION return code= " << code << endl ; exit(code) ;}

# ifndef ASSERT_MED
# define ASSERT_MED(condition) if (!(condition)){ HERE_MED ; cerr << "CONDITION " << #condition << " NOT VERIFIED"<< endl ; INTERRUPTION_MED(1) ;}
# endif /* ASSERT_MED */
#define REPERE_MED {cout<<flush ; cerr << "   --------------" << endl << flush ;}
#define __PREFIX_MED const char* __LOC_MED
#define PREFIX_MED __LOC_MED 
#define BEGIN_OF_MED(chain) __PREFIX_MED = chain; {REPERE_MED ; HERE_MED ; cerr << "Begin of: " << PREFIX_MED << endl ; REPERE_MED ; }
#define END_OF_MED(chain) {REPERE_MED ; HERE_MED ; cerr << "Normal end of: " << chain << endl ; REPERE_MED ; }


# else /* ifdef _DEBUG_*/


# define HERE_MED
# define SCRUTE_MED(var) {}
# define MESSAGE_MED(chain) {}
# define INTERRUPTION_MED(code) {}

# ifndef ASSERT_MED
# define ASSERT_MED(condition) {}
# endif /* ASSERT */

# define REPERE_MED
# define BEGIN_OF_MED(chain)  const char* __LOC_MED; {__LOC_MED=chain; (void)(__LOC_MED); }
# define END_OF_MED(chain)  const char* __LOC_END_MED; {__LOC_END_MED=chain; (void)(__LOC_END_MED);}

#endif /* ifdef _DEBUG_*/

#endif
