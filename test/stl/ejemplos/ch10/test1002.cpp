//
// TEST1002.CPP
//
// This program is used in Chapter 9 as an example
// of passing a type argument to a template function.
// In this case, the third argument to function sum()
// is a type argument that specifies what type the
// function will return.  It isn't used anywhere in
// the function, which is indicated by the fact that
// the third argument to sum() has a type, but no
// parameter name.
//

#include <iostream.h>

template<class Input, class Output >
Output sum( Input a, Input b, Output )
{
    return a + b;
}

main()
{
    cout << "double average of 1, 2 is "
         << ( sum( 1, 2, (double) 0 ) / 2 )
         << endl;
    cout << "integer average of 1, 2 is "
         << ( sum( 1, 2, (int) 0 ) / 2 )
         << endl;
    return 1;
}

