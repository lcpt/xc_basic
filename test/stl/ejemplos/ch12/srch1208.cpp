/*
 * SRCH1208.CPP
 *
 *  This program is used in Chapter 12 to demonstrate
 *  the use of a user written function as
 *  a binary STL predicate.  In this case, the
 *  predicate function has to use two different argument
 *  types in its test for equality.
 *
 *  The program uses the search() function to attempt to
 *  find a match for the character array in the integer
 *  array.  This is a somewhat idiosyncratic function,
 *  which probably doesn't belong in a library (who else
 *  would ever use it?)  Because of that, it is written as
 *  a standard C/C++ function and passed to the search()
 *  algorithms using a function pointer.
 */

//
// Borland 4.x workarounds
//

#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <stdlib.h>
#include <algo.h>
#include <vector.h>

int nints[] = { 6, 7, 8, 9, 10, 1, 2, 3, 4, 5 };
char *sints[] = { "1", "2", "3" };

const int count1 = sizeof( nints ) / sizeof( int );
const int count2 = sizeof( sints ) / sizeof( char * );

bool test( int i, char *p )
{
    int j = atoi( p );
    return i == j;
}

main()
{
    int * ip = search( nints,
                       nints + count1,
                       sints,
                       sints + count2,
                       test );
    if ( ip == ( nints + count1 ) )
        cout << "Failed to find a match\n";
    else
        cout << "Found a match at position "
             << ( ip - nints )
             << endl;
     return 1;
}
