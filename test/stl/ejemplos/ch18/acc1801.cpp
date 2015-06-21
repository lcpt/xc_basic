/*
 * ACC1801.CPP
 *
 * This program is used in Chapter 18 to demonstrate
 * the accumulate algorithm.  The algorithm in this
 * case repeatedly adds a character from an array
 * to a string, which has the effect of appending the
 * entire character array to the string.
 *
 * Note that the use of templates allows this program
 * to use the addition operator inside the accumulate
 * algorithm in a different way than you might normally
 * expect.  It is definitely not being used in a
 * numeric fashion!  Instead, we are using accumulate
 * to add a single character to a C++ string object.
 * This has the effect of appending the character to
 * the string.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <algo.h>

int main()
{
    char data[]= { 'A', 'B', 'C', 'D', 'E' };

    string x = accumulate( data, data + 5, string( "" ) );
    cout << "Accumulated string = "
         << x
         << endl;
    return 0;
}

