//
// VECT0404.CPP
//
// This example program is used in Chapter 4 to demonstrate
// some of the requirements that an object must fulfill in
// order to be inserted in a container class.

//
// Borland 4.x workarounds.
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <iomanip.h>
#include <vector.h>

class foo {
    private :
        int operator=(const foo& rhs){ p = rhs.p; return 1; }
        foo( const foo& rhs );
        char *p;
    public :
        foo( char *name ){ p = name; }
        ~foo(){;}
};

main()
{
    vector<foo> foovec;

    foovec.push_back( foo( "a" ) );
    foovec.push_back( foo( "b" ) );
    foovec.push_back( foo( "c" ) );
    foovec.insert( foovec.begin(), foo( "d" ) );
    foovec.erase( foovec.end() - 2 );
    foovec.pop_back();
    return 1;
}

