/*
 * COPY1401.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the copy algorithm.  It does so using the extremely
 * simple example of copying an array to an ostream
 * iterator.  Note that this technique provides a
 * handy way to display an entire container.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <algo.h>

main()
{
    char *names[] = { "Abel", "Baker", "Charlie", "Delta" };

    copy( names,
          names + 5,
          ostream_iterator<char*>( cout, " " ) );
    return 1;
}
