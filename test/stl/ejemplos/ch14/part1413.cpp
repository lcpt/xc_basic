/*
 * PART1413.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the use of the partition algorithm.  I implement
 * a divide and conquer sorting algorithm that works
 * by simply recursively breaking a sequence down into
 * to pieces, then sorting the pieces.  This is a
 * very elegant algorithm, except for when the the
 * partition fails because the first element is already
 * the least.  In that case I make the first element be
 * a partition of 1.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <time.h>
#include <function.h>
#include <algo.h>
#include <conio.h>

const int size = 10;

int a[ size ];

//
// This sorting algorithm is essentially
// identical to a quicksort.
//
void my_sort( int *first, int *last )
{
    cout << "Sorting ("
         << (int)( first - a )
         << ", "
         << (int)( last - a )
         << ") : ";
    copy( first, last, ostream_iterator<int>( cout, " " ) );
    cout << endl;
//
// I try to create two partitions, using the first element in
// the range as a pivot.
//
    int *p = partition( first, last, bind2nd( less<int>(), *first ) );
    cout << "Partition at "
         << (int)( p - a )
         << endl;
//
// If the subdivide operation failed, it means the first element
// is already the least, so I make it a partition.  Note that
// partition() can return last if the predicate function always
// returns true, but that can't happen when comparing integers.
// If it did, it would mean every integer was less than every other
// integer!
//
    if ( p == first )
        p++;
//
// Now sort the two pieces.  This is a recursive call.
// Note that I don't sort the pieces if the piece is
// only one element long!  Also note that the pivot
// element isn't included in either piece.  It doesn't
// need to be, since it is already at its proper place
// in the sequence.
//
    if ( ( p - first ) > 1 )
        my_sort( first, p );
    if ( ( last - p ) > 1 )
        my_sort( p, last );
}

main()
{
    srand( time( 0 ) );
    for ( int i = 0 ; i < size ; i++ )
        a[ i ] = rand();

    cout << "Input: ";
    copy( a, a + size, ostream_iterator<int>( cout, " " ) );
    cout << endl;
    my_sort( a, a + size );
    cout << "After: ";
    copy( a, a + size, ostream_iterator<int>( cout, " " ) );
    cout << endl;
    return 1;
}
