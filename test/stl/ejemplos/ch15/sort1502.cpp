/*
 * SORT1502.CPP
 *
 * This program is used in Chapter 15 to demonstrate
 * the stable_sort algorithm.  It creates an array with
 * 1024 random elements, then sorts them.  To make it
 * it interesting, this program compares sort() with
 * stable_sort() by counting comparisons and assighments.
 *
 * Note that this program needs to include an STL C++
 * source file, tempbuf.cpp.  The normal way to do this
 * would be to add it to your project, and compile it
 * separately.  To simplify building these example programs,
 * it is included here using the preprocessor.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <stdlib.h>
#include <algo.h>
#include <vector.h>

long foo_count;

struct foo {
    int i;
    foo( int a = 0 ) : i( a ){ foo_count++; }
    foo( const foo &a ){ i = a.i ; foo_count++; }
    void operator=( const foo &a )
    {
        i = a.i;
        foo_count++;
    }
};

long comp_count;

bool int_comp( const foo &a, const foo &b )
{
    comp_count++;
    return a.i < b.i;
}

vector<foo> data( 1024 );

//
// By changing the "#if 0" to "#if 1", you can
// force stable_sort() to run without any temporary
// buffer space, which increases both the number of
// comparisons and assignments it has to make.
//

#include <tempbuf.cpp>
#if 0
pair<foo*, int> get_temporary_buffer(int, foo*) {
    return pair<foo*, int>( 0, 0 );
}
#endif

main()
{
    long seed = time( 0 );

    srand( seed );
    generate( data.begin(), data.end(), rand );
    comp_count = 0;
    foo_count = 0;
    sort( data.begin(), data.end(), int_comp );
    cout << "Sort performed "
         << comp_count
         << " comparisons, "
         << foo_count
         << " assignments\n";

    srand( seed );
    generate( data.begin(), data.end(), rand );
    comp_count = 0;
    foo_count = 0;
    stable_sort( data.begin(), data.end(), int_comp );
    cout << "Stable sort performed "
         << comp_count
         << " comparisons, "
         << foo_count
         << " assignments\n";

    return 1;
}
