/*
 * REPL1405.CPP
 *
 *  This program demonstrates the replace() algorithm
 *  as described in Chapter 14. I perform a simple
 *  replace_copy() with the results printed before
 *  and after the replacement.
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

main()
{
    vector<string> leaders;
    vector<string> revised( 3 );

    leaders.push_back( string( "Lenin" ) );
    leaders.push_back( string( "Molotov" ) );
    leaders.push_back( string( "Trotsky" ) );

    cout << "Original leaders of the 1917 revolution: ";
    copy( leaders.begin(),
          leaders.end(),
          ostream_iterator<string>( cout, " " ) );
    cout << endl;

    replace_copy( leaders.begin(),
                  leaders.end(),
                  revised.begin(),
                  string( "Trotsky" ),
                  string( "Stalin" ) );

    cout << "Revised leaders of the revolution: ";
    copy( revised.begin(),
          revised.end(),
          ostream_iterator<string>( cout, " " ) );
    cout << endl;
    return 1;
}
