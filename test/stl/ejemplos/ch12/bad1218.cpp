//
// BAD1218.CPP
//
// This program is used in Chapter 12 to show how difficult it
// can be to work with function objects after a certain level
// of complexity is reached.  The function under test here is
// relatively simple, x**2 + x + 5, but the STL function object
// that implements is fairly horrendous.  It works, but the
// hand coded function object used for comparison purposes is
// quite a bit easier to follow.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <function.h>
#include <algo.h>
#include <projectn.h>

int input[]={ 0, 1, 2, 3, 4, 5 };
int output[ 6 ];

struct custom {
    int operator()( int x ) {
        return x * x + x + 5;
    }
};

main()
{
    cout << "This section of the program implements the function\n"
         << "x**2 + x + 5 using STL function objects"
         << endl;
    transform( input,
               input + 6,
               output,
               compose1( bind1st( plus<int>(), 5 ),
                         compose2( plus<int>(),
                                   compose2( times<int>(),
                                             ident<int,int>(),
                                             ident<int,int>() ),
                                   ident<int,int>() ) ) );
    int *p1 = input;
    int *p2 = output;
    for ( ; p1 < ( input + 6 ) ; p1++, p2++ )
        cout << "f("
             << *p1
             << ") = "
             << *p2
             << endl;

    cout << "This section of the program implements the function\n"
         << "x**2 + x + 5 using a custom function object"
         << endl;
    transform( input,
               input + 6,
               output,
               custom() );
    p1 = input;
    p2 = output;
    for ( ; p1 < ( input + 6 ) ; p1++, p2++ )
        cout << "f("
             << *p1
             << ") = "
             << *p2
             << endl;

    return 1;
}
