//
// TRAN1207.CPP
//
// This program uses the transform() algorithm to multiply
// two arrays of doubles.  Note that this is the binary
// version of transform().  The multiplication is done
// by the times<> function object.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <algo.h>
#include <function.h>

double in1[] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 };
double in2[] = { 1.0, 0.5, 1.0/3.0, 0.25, 0.2, 1.0/6.0 };
double out[ 6 ];

main()
{
     transform( in1, in1 + 6, in2, out, times<double>() );
     cout << "transform output: ";
     copy( out,
           out + 6,
           ostream_iterator<double>( cout, " " ) );
     cout << endl;

     return 1;
}
