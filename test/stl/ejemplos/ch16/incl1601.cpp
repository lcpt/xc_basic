/*
 * INCL1601.CPP
 *
 * This program is used in Chapter 16 to demonstrate
 * the includes algorithm.  It creates a set that is
 * ordered using a case-insensitive comparison.
 * The program then tests various strings for membership
 * in the set.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <string.h>
#include <ctype.h>
#include <algo.h>
#include <set.h>

//
// The set is missing letter 'm', and is only
// ordered if the comparison object does a
// case insensitive comparison.
//
char *set_data = "abcdefghijklnopqrstuvwXYZ";

struct upper_comp {
    bool operator()( const char &a, const char &b ) const
    {
        return toupper( a ) < toupper( b );
    }
};

void test( set< char, upper_comp> & a, char *b )
{
    bool c;
    c = includes( a.begin(),
                  a.end(),
                  b,
                  b + strlen( b ),
                  upper_comp() );
    cout << b
         << " is "
         << ( c ? "" : "not " )
         << "included in the set"
         << endl;
}

main()
{
    set< char, upper_comp > a;

    while ( *set_data )
        a.insert( *set_data++ );
    cout << "Set: ";
    copy( a.begin(),
          a.end(),
          ostream_iterator<char>( cout ) );
    cout << endl;
    test( a, "ABC" ); //Should pass
    test( a, "ZZZ" ); //Should fail
    test( a, "lmn" ); //Should fail
    test( a, "NML" ); //Bad order, should fail
    test( a, "anz" ); //Should pass
    return 1;
}
