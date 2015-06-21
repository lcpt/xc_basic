/*
 * EACH1301.CPP
 *
 * This program is used in Chapter 13 to demonstrate
 * the for_each() algorithm.  for_each() is called
 * here using a function object called sum_of_squares.
 * sum_of_squares is set up so that each instance has
 * a reference to an integer used to total the
 * values.  This shows off one of the advantages of
 * a function object: encapsulated data.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <algo.h>

struct sum_of_squares {
    int &sum;
    sum_of_squares( sum_of_squares &a ) : sum( a.sum ){;}
    sum_of_squares( int &i ) : sum( i ){;}
    void operator()( int a )
    {
        sum += a * a;
    }
};

main()
{
    int a[] = { 1, 2, 3, 4 };
    int total = 0;
    sum_of_squares sos( total );

    for_each( a, a + 4, sos );
    cout << "sum of squares = " << total << endl;
/*
 * Since for_each() returns a function object, we
 * can use it to retrieve the results just as easily.
 */
    total = 0;
    cout << "sum of squares = "
         << for_each( a, a + 4, sos ).sum
         << endl;
    return 1;
}
