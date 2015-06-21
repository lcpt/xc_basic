/*
 * REV1410.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the reverse algorithm.  An integer array is initialized
 * with the first 15 squared integers.  The array is
 * reversed, with the result being copied into a second
 * array.  The two arrays are then added together using
 * the transform function, and the results is plotted.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <iomanip.h>
#include <algo.h>

main()
{
    int a[ 15 ];
    int b[ 15 ];

    for ( int i = 0 ; i < 15 ; i++ )
        a[ i ] = i * i;
    reverse_copy( a, a + 15, b );
    transform( a, a + 15, b, a, plus<int>() );
//
// Plot the results
//
    for ( i = 0 ; i < 15 ; i++ ) {
        cout << setw( 3 ) << a[ i ] << " ";
        for ( int j = 0 ; j < a[ i ] ; j += 10 )
            cout << '*';
        cout << endl;
    }
    return 1;
}

