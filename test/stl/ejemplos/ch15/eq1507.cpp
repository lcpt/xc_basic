/*
 * EQ1507.CPP
 *
 * This program is used in Chapter 15 to demonstrate
 * the equal_range algorithm.  The lower and upper bounds
 * of some integers are computed and displayed.  Note
 * that this program is almost identical to up1506.cpp,
 * which does the same thing using dual calls to
 * lower_bound and upper_bound.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <algo.h>

int a[] = { 0, 0, 0, 2, 2, 2, 4, 4, 4, 4 };
const int nelems = sizeof( a ) / sizeof( int );

main()
{
    cout << "Input sequence : "
         << "\n";
    copy( a, a + nelems, ostream_iterator<int>( cout, " " ) );
    cout << endl;

    pair< int *, int * > p = equal_range( a, a + nelems, 0 );
    cout << "lower, upper of 0: "
         << ( p.first - a )
         << ", "
         << ( p.second - a )
         << endl;

    p = equal_range( a, a + nelems, 1 );
    cout << "lower, upper of 1: "
         << ( p.first - a )
         << ", "
         << ( p.second - a )
         << endl;

    p = equal_range( a, a + nelems, 5 );
    cout << "lower, upper of 5: "
         << ( p.first - a )
         << ", "
         << ( p.second - a )
         << endl;

    return 1;
}
