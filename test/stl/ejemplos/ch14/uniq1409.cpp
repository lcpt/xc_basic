/*
 * UNIQ1409.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the unique algorithm.  The container used to supply
 * an input sequence to the unique() algorithm is a
 * multiset.  unique() works with any sort of container,
 * but it can only guarantee to remove all duplicates
 * when its input is already sorted.  Given this criteria,
 * input data stored in a multiset is a good choice.  The
 * unique algorithm creates an output sequence that is inserted
 * into a list using an insertion iterator.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <multiset.h>
#include <list.h>
#include <algo.h>

main()
{
    multiset< string, less<string> > a;

    a.insert( "Bill" );
    a.insert( "Mike" );
    a.insert( "Bill" );
    a.insert( "Fred" );
    a.insert( "Andy" );
    a.insert( "Mike" );
    a.insert( "Mike" );

    cout << "Initial multiset:\n";
    copy( a.begin(),
          a.end(),
          ostream_iterator<string>( cout, " " ) );
    cout << endl;

    list<string> b;
    back_insert_iterator< list<string> > i( b );
    unique_copy( a.begin(), a.end(), i );

    cout << "\nList after unique():\n";
    copy( b.begin(),
          b.end(),
          ostream_iterator<string>( cout, " " ) );
    cout << endl;
    return 1;
}
