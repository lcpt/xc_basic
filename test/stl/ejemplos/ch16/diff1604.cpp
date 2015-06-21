/*
 * DIFF1604.CPP
 *
 * This program is used in Chapter 16 to demonstrate
 * the set_difference algorithm.  It simply performs
 * a batch of set_difference algorithms on sorted
 * strings.  Note that one of the test inputs is
 * not sorted, resulting in erroneous output.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <string.h>
#include <algo.h>
#include <set.h>

void diff( char *a, char *b )
{
    cout << a
         << " diff "
         << b
         << " = ";
    set_difference( a, a + strlen( a ),
                    b, b + strlen( b ),
                    ostream_iterator<char>( cout, "" ) );
    cout << endl;
}

main()
{
    diff( "abc", "cde" );
    diff( "abc", "abc" );
    diff( "xxxxxxx", "xxx" );
    diff( "abcdefg", "" );
    diff( "abcdefg", "gfedbca" ); //Error! Not sorted!
    return 1;
}
