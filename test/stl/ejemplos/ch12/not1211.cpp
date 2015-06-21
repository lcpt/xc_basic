//
// NOT1211.CPP
//
// This program is used in Chapter 12 to demonstrate
// the use of the function adaptors not1 and not2.
// Note that the function object mod3 that I use here
// is derived from the unary_function<> template
// class.  Remember that this is a necessary
// prequisite for using an adaptor on a function
// object.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <function.h>
#include <algo.h>

struct mod3 : public unary_function<int,int> {
    bool operator()( int a ) const
    {
         return ( a % 3 ) == 0;
    }
};

int a[] = { 1, 2, 3, 4, 5, 6, 7, 7, 6, 5, 4, 3, 2, 1 };
int *aend = a + ( sizeof(a) / sizeof( int ) );

main()
{
    cout << "Input = ";
    copy( a, aend, ostream_iterator<int>(cout, " " ) );
    cout << endl;
    int count = 0;
    count_if( a, aend, mod3(), count );
    cout << "count_if( ... mod3 ... )  returned "
         << count
         << endl;
    count = 0;
    count_if( a, aend, not1( mod3() ), count );
    cout << "count_if( ... not1( mod3 ) ... ) returned "
         << count
         << endl;
    int *i = adjacent_find( a, aend, not2( less<int>() ) );
    cout << "adjacent_find with not2( less<int>() ) "
         << "found a match at "
         << ( i - a )
         << endl;
    return 1;
}
