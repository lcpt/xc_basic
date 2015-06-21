//
// ALL2001.CPP
//
// This example program is used in Chapter 20 to
// show how allocators are defined in the STL.
// At some point int the future, allocators
// will be defined for objects by way of a
// second argument to the constructor.  The
// default allocator will be defined to be
// the default type used by the class.  However,
// default types for templates are not available
// for most compilers at this time, so
// you have to select the allocator using
// a macro.

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

//
// Allocator selection is done here.
//
#include <faralloc.h>

//
// The following line must be present *before* you
// include vector.h.!
//
#define Allocator far_allocator

#include <vector.h>
#include <iostream.h>
#include <iomanip.h>

main()
{
    vector<double> a( 8000 );
    vector<double> *b[ 20 ];

    for ( int i = 0; i < 20 ; i++ ) {
        cout << "Allocating number " << i << "\n";
        b[ i ] = new vector<double>( a );
    }
    return 1;
}

