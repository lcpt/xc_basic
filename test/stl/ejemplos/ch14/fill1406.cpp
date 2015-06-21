/*
 * FILL1406.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the fill algorithm.  It fills the first 50 elements of
 * a 100 element list with a copy of a string.  Note
 * that the value passed to the fill algorithm is not
 * a string object, just a character pointer.  This is
 * okay, because there is a string assignment operator
 * that can convert from character pointers without complaint.
 * This is another flexibility win for templates and
 * the STL.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <list.h>
#include <algo.h>

main()
{
    list<string> a( 100 );
    fill_n( a.begin(), 50, "<null>" );
    for ( list<string>::iterator i = a.begin();
          i != a.end();
          i++ )
        cout << "\""
             << *i
             << "\" ";
    return 1;
}
