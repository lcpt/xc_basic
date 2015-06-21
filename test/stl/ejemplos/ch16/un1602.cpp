/*
 * UN1602.CPP
 *
 * This program is used in Chapter 16 to demonstrate
 * the set_union algorithm.  It executes the set_union
 * algorithm on several pairs of input sets.  Note that
 * in one case, the input set is not properly sorted,
 * which will result in a bad output set.  The input to
 * set_union() should be sorted, so this program has
 * a short template routine that checks for that.
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

template<class ForwardIterator>
bool sorted( ForwardIterator begin, ForwardIterator end )
{
    if ( begin == end )
        return true;
    ForwardIterator last = begin;
    begin++;
    while ( begin != end )
        if ( *begin++ < *last++ )
            return false;
    return true;
}

void u( char *a, char *b )
{
    cout << a
         << " union "
         << b
         << " = ";
    if ( !sorted( a, a + strlen( a ) ) ) {
        cout << "First range not sorted!\n";
        return;
    } else if ( !sorted( b, b + strlen( b ) ) ) {
        cout << "Second range not sorted!\n";
        return;
    }
    set_union( a,
               a + strlen( a ),
               b,
               b + strlen( b ),
               ostream_iterator<char>( cout, " " ) );
    cout << endl;
}

main()
{
    u( "abc", "cde" );
    u( "abc", "abc" );
    u( "abc", "cba" ); //Error, not sorted!
    u( "xxxxxxx", "xxx" );
    return 1;
}
