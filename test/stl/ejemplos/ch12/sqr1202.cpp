//
// SQR1202.CPP
//
// This program passes a simple function pointer to
// the transform() algorithm, which allows it to
// square items in the vector.  The output from transform()
// is sent to cout via an ostream iterator.
//

#define __MINMAX_DEFINED
#pragma option -vi-
#include <algo.h>

int test[]={ 4, 5, 2, 4, 100, -1, 2, 8 };
const int NELEM = sizeof( test ) / sizeof( int );

int square( int a )
{
    return a * a;
}

main()
{
     ostream_iterator<int> output( cout, " " );

     cout << "Before squaring: ";
     copy( test, test + NELEM, output );
     cout << endl;

     cout << "After squaring: ";
     transform( test,
                test + NELEM,
                output,
                square );
     cout << endl;

     return 1;
}
