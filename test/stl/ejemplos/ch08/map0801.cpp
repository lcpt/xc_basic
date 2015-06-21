//
// MAP0801.CPP
//
// This program is used in Chapter 8 to demonstrate
// the use of associative containers.  In this case,
// the container is a map which holds two strings.
// The first string contains a person's name, and the
// second holds a company name.
//
// The program shows how an associative array
// can be initialized using conventional syntax.
// The contents of the array can then be accessed
// using several different techniques.
//
// Note that this program uses Borland's string class,
// which may differ slightly from the standard.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <cstring.h>
#include <iostream.h>

#include "map.h"

main()
{
    typedef map< string, string, less<string> > my_map;
    my_map a;

    a[ "Bill Gates" ] = "Microsoft";
    a[ "Steve Jobs" ] = "Next";
    a[ "Bob Frankenberg" ] = "Novell";
    a[ "Charles Wang" ] = "Computer Associates";
    for ( my_map::iterator i = a.begin() ;
          i != a.end() ;
          i++ )
        cout << (*i).first
             << " is CEO of "
             << (*i).second
             << endl;
    cout << "John Sculley is the CEO of ";
    my_map::iterator j = a.find( "John Sculley" );
    if ( j == a.end() )
        cout << "nobody";
    else
        cout << (*j).second;
    cout << endl;
    cout << "Bill Gates is CEO of "
         << a[ "Bill Gates" ]
         << endl;
    return 1;
}
