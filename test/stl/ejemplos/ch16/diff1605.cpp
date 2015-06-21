/*
 * DIFF1605.CPP
 *
 * This program is used in Chapter 16 to demonstrate
 * the set_symmetric_difference algorithm.  It performs
 * the same batch of operations as DIFF1604.CPP, but
 * uses set_symmetric_difference instead of
 * set_difference, so you can compare the output
 * from the two programs.  Note that one of the
 * input sets is invalid in this program as well.
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

void sdiff( char *a, char *b )
{
    cout << a
         << " sdiff "
         << b
         << " = ";
    set_symmetric_difference( a, a + strlen( a ),
                    b, b + strlen( b ),
                    ostream_iterator<char>( cout, "" ) );
    cout << endl;
}

main()
{
    sdiff( "abc", "cde" );
    sdiff( "abc", "abc" );
    sdiff( "xxxxxxx", "xxx" );
    sdiff( "abcdefg", "" );
    sdiff( "abcdefg", "gfedbca" ); //Error! Not sorted!
    return 1;
}
