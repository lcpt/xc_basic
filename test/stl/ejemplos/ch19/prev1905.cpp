/*
 * PREV19P05.CPP
 *
 * This program is used in Chapter 19 to demonstrate the
 * prev_permutation algorithm. It generates all the
 * different permutations of three ones and two zeroes.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <algo.h>

bool comp( char a, char b )
{
            return a <= b;
}

int main()
{
            char data[] = "11100";
    cout << data << endl;
    while ( prev_permutation( data, data + 5, comp ) )
                cout << data << endl;
    return 0;
}

