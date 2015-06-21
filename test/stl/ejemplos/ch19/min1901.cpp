/*
 * MIN1901.CPP
 *
 * This program is used in Chapter 19 to demonstrate the min
 * algorithm. It simply works its way through a list of strings,
 * applying min() to adjacent pairs and printing the result.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <algo.h>

string data[] = { "Ford",
                  "Chevrolet",
                  "Chrysler",
                  "Toyota",
                  "Honda",
                  "Subaru" };

int main()
{
    for ( int i = 1 ; i <= 5 ; i++ )
        cout << "min( "
             << data[ i - 1 ]
             << ", "
             << data[ i ]
             << " ) = "
             << min( data[ i - 1 ], data[ i ] )
             << endl;
    return 0;
}

