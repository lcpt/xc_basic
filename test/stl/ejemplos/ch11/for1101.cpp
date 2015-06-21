//
// FOR1101.CPP
//
// This example is used in Chapter 11 to
// show how function objects and function
// pointers can be used with for_each().
// This example uses both types to do the
// same thing.  The major difference between
// the two is that in this case function pointer
// is limited to servicing a single type of
// object, whereas the template function
// object is able to deal with all built in
// types, as well as any others with a defined
// insertion operator.
//

#include <iostream.h>
#include <string.h>

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include "vector.h"
#include "algo.h"

void dump( char c )
{
    cout << c;
}

template<class T>
struct dump_obj {
    void operator()(T x){ cout << x; }
};

main()
{
    char *init = "A string";
    vector<char> a( init, init + strlen( init ) );

    cout << "Using a function pointer: ";
    for_each( a.begin(), a.end(), dump );
    cout << endl;

    cout << "In reverse order, using a function object: ";
    for_each( a.rbegin(), a.rend(), dump_obj<char>() );
    cout << endl;

    return 0;
}

