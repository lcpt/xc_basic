/*
 * PART1803.CPP
 *
 * This program is used in Chapter 18 to demonstrate
 * the partial_sum algorithm.  The program uses
 * partial_sum() to create a running total of the
 * elements in an array of integers. The output of
 * the algorithm is sent directly to an ostream
 * iterator, so it is displayed on cout.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <algo.h>

int main()
{
    int data[] = { 6, 5, 4, 3, 2, 1 };
    partial_sum( data,
                 data + 6,
                 ostream_iterator<int>( cout, " " ) );
    return 0;
}


