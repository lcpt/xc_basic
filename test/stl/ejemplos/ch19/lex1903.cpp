/*
 * LEX1903.CPP
 *
 * This program is used in Chapter 19 to demonstrate the
 * lexicographical_compare function. I do this by creating
 * a functional equivalent of the stricmp() function, which
 * compares two input strings in a case-insensitive manner.
 */

//
// Borland 4.x workarounds.
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <string.h>
#include <ctype.h>
#include <algo.h>

bool comp( char a, char b )
{
    return toupper( a ) < toupper( b );
}

void my_stricmp( char *a, char *b )
{
    bool result;
    result = lexicographical_compare( a, a + strlen( a ),
                                      b, b + strlen( b ),
                                      comp );
    cout << a
         << " is "
         << (result ? "less than " : "greater or equal to ")
         << b
         << endl;
}

int main()
{
    my_stricmp( "Mark", "mark" );
    my_stricmp( "TEST", "test1" );
    my_stricmp( "test1", "test" );
    my_stricmp( "A", "Z" );
    return 0;
}

