/*
 * SWAP1402.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the iter_swap algorithm.  It swaps two elements from
 * a pair of arrays of character strings.
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
    char *boys[] = { "Fred", "Ricky", "Pat", "George" };
    char *girls[] = { "Ethel", "Lucy", "Barbara", "Chris" };

    iter_swap( boys + 2, girls + 3 );
    cout << "Boys: ";
    copy( boys,
          boys + 4,
          ostream_iterator<char*>( cout, " " ) );
    cout << "\nGirls: ";
    copy( girls,
          girls + 4,
          ostream_iterator<char*>( cout, " " ) );
    return 1;
}
