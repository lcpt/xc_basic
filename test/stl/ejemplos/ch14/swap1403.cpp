/*
 * SWAP1403.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the swap_ranges() algorithm.  It does so by swapping
 * two randomly selected ranges of iterators.
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

    cout << "Boys before: ";
    copy( boys,
          boys + 4,
          ostream_iterator<char*>( cout, " " ) );
    swap_ranges( boys + 1, boys + 4, girls );
    cout << "\nBoys after: ";
    copy( boys,
          boys + 4,
          ostream_iterator<char*>( cout, " " ) );
    return 1;
}
