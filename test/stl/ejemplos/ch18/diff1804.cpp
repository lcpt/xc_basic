/*
 * DIFF1804.CPP
 *
 * This program is used in Chapter 18 to demonstrate
 * the adjacent_difference algorithm.  It simply outputs
 * the adjacent differences between a vector of input
 * data.
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
    int data[] = { 0, 1, 3, 6, 10, 6, 3, 1, 0, 0 };
    cout << "Input data: ";
    copy( data,
          data + 10,
          ostream_iterator<int>( cout, " " ) );
    cout << endl;
    cout << "Adjacent difference: ";
    adjacent_difference( data,
                 data + 10,
                 ostream_iterator<int>( cout, " " ) );
    cout << endl;
    return 0;
}


