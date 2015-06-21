/*
 * PUSH1701.CPP
 *
 * This program is used in Chapter 17 to demonstrate
 * the push_heap algorithm.  It simply creates a heap
 * in the data[] array, using the first 10 elements.
 * It prints the initial heap, then prints the heap
 * after each of two new elements is added.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <iomanip.h>
#include <algo.h>
#include <heap.h>

int data[] = { 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, -1, -1 };

void print_heap( int *first, int *last );

int main()
{
    make_heap( data, data + 10 );
    cout << "Before:\n";
    print_heap( data, data + 10 );

    data[ 10 ] = 99;
    push_heap( data, data + 11 );
    cout << "After push_heap( 99 ):\n";
    print_heap( data, data + 11 );

    data[ 11 ] = 10;
    push_heap( data, data + 12 );
    cout << "After push_heap( 10 ):\n";
    print_heap( data, data + 12 );

    return 1;
}

/*
 * This slightly complicated routine is used to
 * print a heap, one row at a time.  The calculations
 * needed to make it look tree-like are what takes up
 * all the space here.
 */

int print_row( int n, const int *p, int last )
{
    int low = ( 1 << n ) - 1;
    int high = 2 * low;

    int max_row = 0;
    int temp = last;
    for ( ; ; ) {
        temp /= 2;
        if ( temp == 0 )
            break;
        max_row++;
    }
    if ( n > max_row )
        return 0;
    int spaces = ( 2 << ( max_row - n ) ) - 1 ;
    for ( int i = low ; i <= high && i < last ; i++ ) {
        for ( int j = 0 ; j < spaces ; j++ )
            cout << " ";
        cout << setw( 2 ) << p[ i ];
        for ( j = 0 ; j < spaces ; j++ )
            cout << " ";
    }
    cout << endl;
    return 1;
}

/*
 * This is the print_heap() routine.  It prints out the
 * heap, one row at a time.
 */

void print_heap( int *first, int *last )
{
    for ( int i = 0 ; ; i++ )
        if ( !print_row( i, first, last - first ) )
            break;
    cout << "\n";
}


