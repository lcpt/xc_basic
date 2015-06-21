//
// ITER1005.CPP
//
// This program creates a vector full of
// Fibonacci numbers, then uses an
// ostream_iterator to write the whole
// vector to an output file.  In then
// uses an istream_iterator to read the
// file back in.  A comparison is then done
// to ensure that the whole thing worked.
//
#include <fstream.h>
#include <iostream.h>
#include <string.h>

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-


#include "vector.h"
#include "iterator.h"

main()
{
    ofstream outfile( "iter1005.dat" );
    ostream_iterator<long> out(outfile, " " );
    vector<long> fib( 25 );
//
// Create a vector full of fibonacci data
//
    fib[ 0 ] = 1;
    fib[ 1 ] = 1;
    for ( int i = 2 ; i < 25 ; i++ )
        fib[ i ] = fib[ i - 1 ] + fib[ i - 2 ];
//
// Write the data out
//
    copy( fib.begin(), fib.end(), out );
    outfile.close();
    ifstream infile( "iter1005.dat" );
//
// Now read it all back in.
//
    vector<long> test( 25 );
    istream_iterator<long, ptrdiff_t> in( infile );
    istream_iterator<long, ptrdiff_t> end;
    copy( in, end, test.begin() );
//
// And report on the results.
//
    if ( test == fib )
        cout << "Comparison passes!\n";
    else
        cout << "Comparison fails!\n";
    return 1;
}

