/*
 * MISM1307.CPP
 *
 * This program is used in Chapter 13 to demonstrate
 * the mismatch algorithm.  The program creates a big
 * vector of doubles, copies them into a set of integers,
 * then runs mismatch to see if there is one.  There should
 * be a forced error at the very end of the vector, because
 * a value that won't convert to an integer is used.
 *
 * Note also that this program demonstrates the fact that
 * mismatch() can work with different container types, as
 * well as using iterators that point to different types.
 *
 */

//
// Borland 4.x workarounds
//
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
    a.push_back( 3000 + 0.5 );
//
// Insert all the doubles into a set using an
// insertion iterator.
//
    set< int, less<int> > b;
    insert_iterator< set<int, less<int> > > sii( b, b.begin() );
    copy( a.begin(), a.end(), sii );
//
// Now check for a mismatch.
//
    double *mm = mismatch( a.begin(), a.end(), b.begin() ).first;
    if ( mm == a.end() )
        cout << "No mismatch detected!\n";
    else
        cout << "Mismatch at: "
             << *mm
             << endl;
    return 1;
}
