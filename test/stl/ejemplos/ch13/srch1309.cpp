/*
 * SRCH1309.CPP
 *
 * This program is used in Chapter 13 to demonstrate
 * the search() algorithm.  It simply searches for the
 * appearance of the key in the text, then prints out
 * information on what was found.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <string.h>

#include <vector.h>
#include <algo.h>

char text[] = "It was the best of times, it "
              "was the worst of times.";
char key[] = "it was";

main()
{
    vector<char> t( text, text + strlen( text ) );
    vector<char> k( key, key + strlen( key ) );
    t.push_back( 0 );

    char *p = search( t.begin(), t.end(), k.begin(), k.end() );
    cout << "Match was found at offset "
         << ( p - t.begin() )
         << endl;
    cout << "Match: "
         << p
         << endl;
    return 1;
}
