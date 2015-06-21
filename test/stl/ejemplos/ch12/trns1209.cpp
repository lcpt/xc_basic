//
// TRNS1209.CPP
//
// This program is used in Chapter 12 to demonstrate
// how a home grown function object can be used with
// function adaptors. The function object add2x() is
// used to add argument 2 to argument 1 twice, and return
// the result.  The first time I call transform(), add2x()
// is used in a traditional fashion.  The second time,
// however, it is used with the bind2nd() function adaptor.
// This lets add2x() be used with a hardcoded value as the
// second argument.  In this particular example, that
// hard coded argument value is simply the number 2.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <algo.h>
#include <function.h>
#include <projectn.h>

double in1[] = { 1, 2, 3, 4, 5, 6 };
double in2[] = { 1, 3, 5, 7, 9, 11 };
const int NELEM = sizeof( in1 ) / sizeof( double );

template<class T>
struct add2x : public binary_function<T, T, T>
{
    T operator()( T a, T b ) const
    {
        a += b;
        a += b;
        return a;
    };
};

main()
{
     cout << "in1: ";
     copy( in1,
           in1 + NELEM,
           ostream_iterator<double>( cout, " " ) );
     cout << endl
          << "in2: ";
     copy( in2,
           in2 + NELEM,
           ostream_iterator<double>( cout, " " ) );
     cout << endl
          << "in1 + add2x( in2 ): ";
     transform( in1,
                in1 + NELEM,
                in2,
                ostream_iterator<int>( cout, " " ),
                add2x<int>() );
     cout << endl
          << "in1 + add2x( 2 ): ";
     transform( in1,
                in1 + NELEM,
                ostream_iterator<int>( cout, " " ),
                bind2nd( add2x<int>(), 2 ) );
     cout << endl;
     return 1;
}
