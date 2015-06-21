//
// RI1006.CPP
//
// This test program is used in Chapter 10 to demonstrate
// how the reverse iterator adaptor can be applied to an
// existing iterator, as long as it is either a random
// access or bidirectional iterator.
//

#include <iostream.h>
#include <string.h>

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include "iterator.h"

char test[]= "This is a test";
typedef reverse_iterator< char *,
                          char,
                          char &,
                          ptrdiff_t > my_ri;

main()
{
    my_ri r = test + strlen( test );
    my_ri rend = test;
    while ( r != rend )
        cout << *r++;
    cout << endl;
    return 1;
}

