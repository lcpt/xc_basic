/*
 * SORT1503.CPP
 *
 * This program is used in Chapter 15 to demonstrate
 * the partial_sort algorithm.  A random deque of
 * integers is partially sorted.  The input and
 * output results are displayed using the standard STL
 * copy() algorithm.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <stdlib.h>
#include <algo.h>
#include <deque.h>

main()
{
    deque<int> data( 10 );
    deque<int> result( 5 );

    srand( time( 0 ) );
    for ( int i = 0 ; i < 10 ; i++ )
        data[ i ] = rand();

    cout << "Before sorting:\n";
    copy( data.begin(),
          data.end(),
          ostream_iterator<int>( cout, " " ) );

    partial_sort_copy( data.begin(),
                       data.end(),
                       result.begin(),
                       result.end() );

    cout << "\n\nPartial sort result:\n";
    copy( result.begin(),
          result.end(),
          ostream_iterator<int>( cout, " " ) );
    cout << endl;
    return 1;
}
