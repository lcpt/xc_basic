//
// INS1007.CPP
//
// This program is used in Chapter 10 to demonstrate
// the use of an insert iterator adaptor.  It creates
// an back inserter and uses it to add factorial
// elements to a vector.  Once the elements have all
// been inserted, the vector gets dumped to cout.
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
    vector<long> fact;

    fact.push_back( 1 );
    back_insert_iterator<vector<long> > bi( fact );

    for ( int i = 1 ; i < 10 ; i++ )
        *bi = ( i + 1 ) * fact[ i - 1 ];
    ostream_iterator<long> out(cout, "\n" );
    copy( fact.begin(), fact.end(), out );
    return 1;
}

