/*
 * SORT1704.CPP
 *
 * This program is used in Chapter 17 to demonstrate
 * the sort_heap algorithm.  It simply creates a character
 * array with 40 random elements, then makes the array
 * into a heap, then sorts it.  The array is printed
 * out at each step along the way.
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

int main()
{
    char data[ 41 ];
    srand( time( 0 ) );
    char input_set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                       "abcdefghijklmnopqrstuvwxzy";

    for ( int i = 0 ; i < 40 ; i++ )
        data[ i ] = input_set[ rand() % 52 ];
    data[ 40 ] ='\0';
    cout << "Data   = " << data << endl;
    make_heap( data, data + 40 );
    cout << "Heap   = " << data << endl;
    sort_heap( data, data + 40 );
    cout << "Sorted = " << data << endl;
    return 1;
}

