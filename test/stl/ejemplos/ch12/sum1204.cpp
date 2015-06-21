//
// SUM1204.CPP
//
// This program demonstrates the ability of
// function objects to encapsulate data. The
// program creates a function object from the
// sum_square class, which contains a reference
// to a user supplied counter.  That counter
// will have the squared quantity added to
// it each time square() is called.  After
// transform() is complete, counter will hold
// the sum of the squares of the input range.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <algo.h>
#include <function.h>

int test[]={ 4, 5, 2, 4, 100, -1, 2, 8 };
const int NELEM = sizeof( test ) / sizeof( int );

template<class T>
struct sum_square {
    T &counter;
    sum_square( int &c ) : counter( c ){ counter = 0; }
    T operator()( T n )
    {
        T temp = n * n;
        counter += temp;
        return temp;
    }
};

main()
{
     ostream_iterator<int> output( cout, " " );

     cout << "Before applying square: ";
     copy( test, test + NELEM, output );
     cout << endl;

     int count;
     cout << "After squaring: ";
     transform( test,
                test + NELEM,
                output,
                sum_square<int>( count ) );
     cout << endl;

     cout << "count = " << count << endl;
     return 1;
}
