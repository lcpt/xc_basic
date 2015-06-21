/*
 * REPL1205.CPP
 *
 * This program is used in Chapter 12 to demonstrate
 * the use of a predicate function with the
 * replace_copy_if() algorithm.  The function object
 * returns a value of true when the language being
 * tested is on the black list.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <algo.h>
#include <vector.h>

char *blacklist[]={ "FORTRAN", "BASIC", "JCL", 0 };

struct pred {
    bool operator()( string& s ) {
        for ( char **p = blacklist ; *p != 0 ; p++ ) {
            if ( s == *p )
                return true;
        }
        return false;
    }
};

main()
{
     vector<string> a;
     a.push_back( "FORTRAN" );
     a.push_back( "C" );
     a.push_back( "C++" );
     a.push_back( "Prolog" );
     a.push_back( "BASIC" );

     cout << "Before: ";
     copy( a.begin(),
           a.end(),
           ostream_iterator<string>( cout, " " ) );
     cout << endl;
     cout << "After: ";
     replace_copy_if( a.begin(),
                      a.end(),
                      ostream_iterator<string>( cout, " " ),
                      pred(),
                      string( "***REPLACED***" ) );
     cout << endl;
     return 1;
}
