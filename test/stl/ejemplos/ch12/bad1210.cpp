//
// BAD1210.CPP
//
// This program is used in Chapter 12 to demonstrate
// how a non-mutating algorithm such as for_each() is
// actually at the mercy of its callback function.  The
// callback can modify the objects pointed to in the input
// range, and there isn't much the compiler can do
// about it!
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <algo.h>
#include <iostream.h>

void x2( long & l )
{
    l = l + l;
}

long a[] = { 1, 2, 3, 4, 5 };
const int nelem = sizeof( a ) / sizeof( long );

main()
{
    cout << "Before for_each: ";
    copy( a,
          a + nelem,
          ostream_iterator<long>( cout, " " ) );
    cout << endl;
    for_each( a,
              a + nelem,
              x2 );
    cout << "After for_each: ";
    copy( a,
          a + nelem,
          ostream_iterator<long>( cout, " " ) );
    cout << endl;
    return 1;
}
