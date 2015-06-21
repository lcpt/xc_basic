//
// COPY1102.CPP
//
// This example is used in Chapter 11 to
// show an example of a copy variant of
// an algorithm.  rotate_copy() rotates a
// container by a specified amount, and
// copies the result into a new container.
// This program also uses an inserter
// iterator to simplify life a little bit.
//

#include <iostream.h>
#include <string.h>

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include "list.h"
#include "deque.h"
#include "algo.h"

void dump( char c )
{
    cout << c ;
}

main()
{
    char *init = "A new string!";
    deque<char> a( init, init + strlen( init ) );
    list<char> b;

    rotate_copy( a.begin(),
                 a.begin() + 4,
                 a.end(),
                 back_insert_iterator<list<char> >( b ) );
    for_each( b.begin(), b.end(), dump );
    return 0;
}

