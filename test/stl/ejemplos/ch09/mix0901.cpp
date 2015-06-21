//
// MIX0901.CPP
//
// This program is used in Chapter 9 to give a quick
// demonstration of mixed model programming.  You can
// compile this as a small model program that still is
// able to allocate over 300Kbytes of data in far
// memory.
//
// This program only works with 16 bit PC compilers!
//

#include <iostream.h>
#include <alloc.h>

int _far *tables[ 10 ];

main()
{
//
// Any allocation failures will show up
// as 0000:0000 pointers in the printout.
//
    int i;
    for ( i = 0; i < 10 ; i++ )
        tables[ i ] = new far int[ 16000 ];
    for ( i = 0 ; i < 10 ; i++ )
        cout << i
             << ": "
             << hex
             << (long) tables[ i ]
             << dec
             << endl;
    for ( i = 0; i < 10 ; i++ )
        if ( tables[ i ] )
            delete tables[ i ];
    return 1;
}

