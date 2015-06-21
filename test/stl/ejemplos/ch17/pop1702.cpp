/*
 * POP1702.CPP
 *
 * This program is used in Chapter 17 to demonstrate
 * the pop_heap algorithm.  It simply creates a vector
 * with 10 random elements.  The vector is then sorted
 * by first making it into a heap, then popping the top
 * element, one by one, until the whole vector has
 * been processed.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <stdlib.h>

#include <algo.h>
#include <heap.h>
#include <vector.h>

int main()
{
    vector< int > a;
    srand( time( 0 ) );

    cout << "Unsorted vector:\n";
    for ( int i = 0 ; i < 10 ; i++ )
        a.push_back( rand() );
    copy( a.begin(),
          a.end(),
          ostream_iterator<int>( cout, " " ) );
    cout << endl;
    make_heap( a.begin(), a.end() );
    cout << "Sorted vector  :\n";
    while ( a.begin() != a.end() ) {
        pop_heap( a.begin(), a.end() );
        cout << *( a.end() - 1 )
             << " ";
        a.pop_back();
    }
    return 1;
}

