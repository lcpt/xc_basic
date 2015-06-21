/*
 * EQ1308.CPP
 *
 * This program is used in Chapter 13 to demonstrate
 * the equal algorithm.  The flexibility of STL
 * algorithms is demonstrated by using equal() to
 * compare a sequence in a vector with a sequence
 * in a set.
 */

#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <ctype.h>

#include <set.h>
#include <vector.h>
#include <algo.h>

main()
{
//
// Fill up the vector with a bunch of doubles
//
    vector<double> a;
    for ( double d = 0.0 ; d < 3000.0 ; d += 100.0 )
        a.push_back( d );
//
// Insert all the doubles into a set using an
// insertion iterator.
//
    set< int, less<int> > b;
    insert_iterator< set<int, less<int> > > sii( b, b.begin() );
    copy( a.begin(), a.end(), sii );

    bool match = equal( a.begin(), a.end(), b.begin() );
    cout << "Before modification a and b are "
         << ( match ? "" : "not " )
         << "equal\n";

    a[ 1 ] += .5;
    match = equal( a.begin(), a.end(), b.begin() );
    cout << "After modification a and b are "
         << ( match ? "still " : "not " )
         << "equal\n";
    return 1;
}
