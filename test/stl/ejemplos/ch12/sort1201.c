/*
 * SORT1201.C
 *
 * This program is used in Chapter 12 to demonstrate
 * how the C standard RTL function qsort() is used.
 *
 */

#include <stdlib.h>
#include <stdio.h>

/*
 * This is the comparison function passed to
 * qsort().  qsort() expects the function to
 * take two void pointers, instead of pointers
 * to the user specified type.  This means you
 * either have to cast the function prototype
 * in the call to qsort(), or cast the arguments
 * in the function.  I chose the latter for this
 * example.
 */

int int_cmp( const void *p1, const void *p2 )
{
    return * (const int *) p1 - * (const int *) p2;
}

int test[]={ 4, 5, 2, 4, 100, -1, 2, 8 };
#define NELEM ( sizeof( test ) / sizeof( int ) )

main()
{
     int i;

     printf( "Before sorting: " );
     for ( i = 0 ; i < NELEM ; i++ )
        printf( "%d ", test[ i ] );
     printf( "\n" );

     qsort( test, NELEM, sizeof( int ), int_cmp );

     printf( "After sorting: " );
     for ( i = 0 ; i < NELEM ; i++ )
        printf( "%d ", test[ i ] );
     printf( "\n" );

     return 1;
}
