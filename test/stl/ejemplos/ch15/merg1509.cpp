/*
 * MERG1509.CPP
 *
 * This program is used in Chapter 15 to demonstrate
 * the merge algorithm.  It creates two sorted arrays
 * of char pointers, then merges them and print the
 * results.  As always, when sorting pointers, you
 * usually need to define a comparison object to
 * compare the elements found by dereferencing the
 * pointers.  In this case, the comparison object is
 * actually a function pointer.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <string.h>
#include <algo.h>
#include <vector.h>

char *dem[] = { "Al", "Bill", "Hillary", "Tipper" };
const int dem_len = sizeof( dem ) / sizeof( char * );

char *rep[] = { "Barbara", "Dan", "George", "Marilyn" };
const int rep_len = sizeof( rep ) / sizeof( char * );

bool comp( const char *a, const char *b )
{
    return strcmp( a, b ) < 0;
}

main()
{
    vector<char *> result;

    back_insert_iterator< vector< char * > > i( result );
    merge( dem, dem + dem_len, rep, rep + rep_len, i, comp );
    copy( result.begin(),
          result.end(),
          ostream_iterator< char * >( cout, " " ) );
    cout << endl;
    return 1;
}
