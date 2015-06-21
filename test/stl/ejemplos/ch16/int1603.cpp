/*
 * INT1603.CPP
 *
 * This program is used in Chapter 16 to demonstrate
 * the set_intersection algorithm.  It executes the
 * algorithm on one particular set of data, storing the
 * result in some reserved space in a deque<double>
 * object.  There are a couple of interesting points
 * to note here.  First, the output sequence is of a
 * different type than the two input sequences, which
 * works fine, since operator=() is able to convert
 * from one type to the other.  Second, to see where
 * the end of the output data is, I have to look at the
 * iterator returned from the set_intersection algorithm,
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <iomanip.h>
#include <algo.h>
#include <deque.h>

main()
{
    int a[] = { 1, 1, 1, 5, 10 };
    int b[] = { 1, 1, 4, 5 };
    deque<double> c( 10 );
    deque<double>::iterator j;

    j = set_intersection( a, a + 5,
                          b, b + 4,
                          c.begin() );
    cout << "Intersection: ";
    cout.setf( ios::showpoint );
    for ( deque<double>::iterator i = c.begin() ;
          i != j;
          i++ )
        cout << *i << " ";
    cout << endl;
    return 1;
}
