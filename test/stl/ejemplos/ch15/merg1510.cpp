/*
 * MERG1510.CPP
 *
 * This program is used in Chapter 15 to demonstrate
 * the inplace_merge algorithm.  It is fundamentally
 * the same program as that shown in MERG1509.CPP, with
 * the only difference begin that the two input arrays
 * are stored in a a single location before merging.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <algo.h>
#include <vector.h>
//
// Normally you would not include a source module
// using the preprocessor.  This program includes
// tempbuf.cpp in an effort to make it easier to
// build the demos.
//
#include <tempbuf.cpp>

string white_house[] =
    { "Al", "Bill", "Hilary", "Tipper",
      "Barbara", "Dan", "George", "Marilyn" };

main()
{
    inplace_merge( white_house,
                   white_house + 4,
                   white_house + 8 );
    copy( white_house,
          white_house + 8,
          ostream_iterator< string >( cout, " " ) );
    return 1;
}
