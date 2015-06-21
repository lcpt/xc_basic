//
// SQR1203.CPP
//
// This program is nearly identical to sqr1202.cpp.
// The only difference is that instead of using a
// function pointer to get transform() to square all
// the elements of a vector, it uses a function object.
// The function object, square<T>, is a local created
// class, not one of the classes provided with the
// library.
//

#define __MINMAX_DEFINED
#pragma option -vi-

#include <algo.h>
#include <function.h>

int test[]={ 4, 5, 2, 4, 100, -1, 2, 8 };
const int NELEM = sizeof( test ) / sizeof( int );

template<class T>
struct square {
    T operator()( T n ){ return n * n ; }
};

main()
{
     ostream_iterator<int> output( cout, " " );

     cout << "Before applying square: ";
     copy( test, test + NELEM, output );
     cout << endl;

     cout << "After squaring: ";
     transform( test,
                test + NELEM,
                output,
                square<int>() );
     cout << endl;

     return 1;
}
