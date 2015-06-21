/*
 * NEXT1904.CPP
 *
 * This program is used in Chapter 19 to demonstrate the
 * next_permutation algorithm. It generates all the
 * permutations of the first five letters of the alphabet.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <algo.h>

int main()
{
    char data[] = "ABCDE";
    cout << data << " ";
    while ( next_permutation( data, data + 5 ) )
        cout << data << " ";
    return 0;
}

