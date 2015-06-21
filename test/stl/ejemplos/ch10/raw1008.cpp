//
// RAW1008.CPP
//
// This program is used in Chapter 10 to demonstrate
// the use of raw storage iterator adaptor.  It creates
// five objects of type foo in raw, unallocated memory.
// Afterward, the raw contents of the memory area are
// dumped to ensure that some valid construction actually
// took place in there.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <stdlib.h>
#include <string.h>

#include "defalloc.h"
#include "iterator.h"

//
// This foo structure has a constructor that
// initializes the array.  By examining the
// array, you can tell whether it was built
// using the default constructor or using
// a single argument.
//
struct foo {
    char a[ 5 ];
    foo( char i = -1 ) { memset( a, i, 5 ); };
};

main()
{
    char *p = (char *) malloc( 5 * sizeof( foo ) );
    raw_storage_iterator<foo *, foo > rsi = (foo *) p;
    for ( int i = '1' ; i <= '5' ; i++ )
        *rsi++ = foo( i );
    for ( i = 0 ; i < ( 5 * sizeof( foo ) ) ; i++ )
        cout << *p++;
    cout << endl;
    return 1;
}

