//
// COMP1214.CPP
//
// This program is used in Chapter 12 to demonstrate
// the two composition functions.  compose2() combines
// three function objects, compose1() combines two
// function objects.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <string.h>
#include <projectn.h>

#include <algo.h>

double d[]= { 1.0, 2.0, 3.0, 4.0, 5.0 };
double *dend = d + ( sizeof( d ) / sizeof( double ) );

main()
{
    cout << "d[] = ";
    copy( d, dend, ostream_iterator<double>(cout, " "));
    cout << endl
         << "compose2 will perform x = -x * -x\n";
    transform( d,
               dend,
               d,
               compose2( times<double>(),
                         negate<double>(),
                         negate<double>() ) );
    cout << "After compose2: ";
    copy( d, d + 5, ostream_iterator<double>(cout, " " ) );
    cout << endl;

    cout << "compose1 will perform x = ( x * 100 ) + 100\n";
    transform( d,
               dend,
               d,
               compose1( bind1st( plus<double>(), 100.0 ),
                         bind1st( times<double>(), 100.0 ) ) );
    cout << "After compose1: ";
    copy( d, dend, ostream_iterator<double>(cout, " " ) );
    cout << endl;
    return 1;
}
