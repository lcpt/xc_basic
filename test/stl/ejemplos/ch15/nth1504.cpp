/*
 * NTH1504.CPP
 *
 * This program is used in Chapter 15 to demonstrate
 * the nth_element algorithm.  A data array of chars
 * is shuffled into a random sequence, then
 * partititioned around the 13 element.  The 13th element
 * should always be letter 'N'.  After the paritioning,
 * everything before 13 should be less than 'N', and
 * everything after should be greater.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <stdlib.h>
#include <algo.h>

//
// This example program uses the code found in
// random.cpp.  In a normal project, you would
// compile this module separately and combine
// at link time.  In this example, the source
// is included directly, in order to simplify
// bulding.
//
#include <random.cpp>

char data[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

main()
{
    random_shuffle( data, data + 26 );
    cout << "Shuffled data: "
         << data
         << endl;

    nth_element( data, data + 13, data + 26 );

    cout << "Lower partition: ";
    for ( int i = 0 ; i < 13 ; i++ )
        cout << data[ i ];
    cout << endl;

    cout << "Pivot: "
         << data[ 13 ]
         << endl;

    cout << "Upper partition: ";
    for ( i = 14 ; i < 26 ; i++ )
        cout << data[ i ];
    cout << endl;

    return 1;
}
