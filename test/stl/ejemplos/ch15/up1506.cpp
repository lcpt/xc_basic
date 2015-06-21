/*
 * UP1506.CPP
 *
 * This program is used in Chapter 15 to demonstrate
 * the upper_bound algorithm.  The lower and upper bounds
 * of some integers are computed and displayed.  The
 * only time the lower and upper bounds are going to
 * be different is when there are runs of duplicate
 * values, so the input data is set up to have a few.
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

    int *p1 = lower_bound( a, a + nelems, 0 );
    int *p2 = upper_bound( a, a + nelems , 0 );
    cout << "lower, upper of 0: "
         << ( p1 - a )
         << ", "
         << ( p2 - a )
         << endl;

    p1 = lower_bound( a, a + 10, 1 );
    p2 = upper_bound( a, a + 10 , 1 );
    cout << "lower, upper of 1: "
         << ( p1 - a )
         << ", "
         << ( p2 - a )
         << endl;

    p1 = lower_bound( a, a + 10, 4 );
    p2 = upper_bound( a, a + 10 , 4 );
    cout << "lower, upper of 4: "
         << ( p1 - a )
         << ", "
         << ( p2 - a )
         << endl;

    return 1;
}
