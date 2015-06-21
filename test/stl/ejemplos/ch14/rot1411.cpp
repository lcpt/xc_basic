/*
 * ROT1411.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the rotate algorithm.  The program makes two calls
 * to the rotate algorithm.  The first uses rotate_copy()
 * to produce an output sequence in a new array.  The
 * second performs an in place rotation of the input
 * sequence.  They are both printed out, and they better
 * be identical!
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>

#include <algo.h>

main()
{
    char *a[] = { "0th", "1st", "2nd", "3rd", "4th",
                  "middle", "6th", "last" };
    char *b[ 8 ];

    cout << "Input: " << endl;
    copy( a, a + 8, ostream_iterator<char*>(cout, " " ) );
    cout << endl << endl;

    rotate_copy( a, a + 5, a + 8, b );
    cout << "Copy output: " << endl;
    copy( b, b + 8, ostream_iterator<char*>(cout, " " ) );
    cout << endl << endl;

    rotate( a, a + 5, a + 8 );
    cout << "In-place output: " << endl;
    copy( a, a + 8, ostream_iterator<char*>(cout, " " ) );
    cout << endl << endl;

    return 1;
}
