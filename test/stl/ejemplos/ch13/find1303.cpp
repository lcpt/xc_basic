/*
 * FIND1303.CPP
 *
 * This program is used in Chapter 13 to demonstrate
 * the find_end() algorithm.  It uses a pair of
 * character arrays to perform the test.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <algo.h>

//
// Note that the array was chosen to *not*
// provide a match.
//

char *input = "ABCDEABCDEABCDE";
char *match = "ABCDEF";
const int input_len = sizeof( input ) / sizeof( char );
const int match_len = sizeof( match ) / sizeof( char );

main()
{
    char *result = find_end( input, input + input_len,
                             match, match + match_len );
    cout << "find_end returned an offset of "
         << (result - input)
         << endl;
    return 1;
}
