/*
 * SORT1501.CPP
 *
 * This program is used in Chapter 15 to demonstrate
 * the sort algorithm.  It creates an array with
 * 10,000 random elements, then sorts them.  The
 * first and last five elements of the array are
 * printed before and after the sort operation.
 */

//
// Borland 4.x workarounds.
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <stdlib.h>
#include <algo.h>

const int n = 10000;

int data[ n ];

main()
{
    for ( int i = 0 ; i < n ; i++ )
        data[ i ] = rand();

    cout << "Before sorting:\n\n";
    cout << "First 5 elements: ";
    copy( data,
          data+ 5,
          ostream_iterator<int>( cout, " " ) );
    cout << "\n\nLast 5 elements: ";
    copy( data + n - 5,
          data + n,
          ostream_iterator<int>( cout, " " ) );
    cout << endl;

    sort( data, data + n );

    cout << "\n\nAfter sorting:\n\n";
    cout << "First 5 elements: ";
    copy( data,
          data+ 5,
          ostream_iterator<int>( cout, " " ) );
    cout << "\n\nLast 5 elements: ";
    copy( data + n - 5,
          data + n,
          ostream_iterator<int>( cout, " " ) );
    cout << endl;

    return 1;
}
