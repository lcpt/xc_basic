//
// REP1103.CPP
//
// This example is used in Chapter 11 to demonstrate the
// replace_if() function.  It goes through a set of
// names, and replaces every one that passes the test()
// function.  test() just tests to see if the name
// has exactly six letters!
//
// Note that this program uses Borland's implementation
// of the string class.  You may need to make minor
// modifications to adapt to your compiler.

#include <iostream.h>
//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <cstring.h>
#include "set.h"
#include "algo.h"

void dump( string s )
{
    cout << s << " ";
}

bool test( string s )
{
    if ( s.length() == 6 )
        return true;
    else
        return false;
}

main()
{
    set<string, less<string> > a;

    a.insert( "Bill" );
    a.insert( "George" );
    a.insert( "Ronald" );
    a.insert( "Jimmy" );
    a.insert( "Gerald" );
    cout << "Before: ";
    for_each( a.begin(), a.end(), dump );
    cout << endl
         << "After: ";
    replace_if( a.begin(), a.end(), test, "<None of the above>" );
    for_each( a.begin(), a.end(), dump );
    cout << endl;
    return 0;
}

