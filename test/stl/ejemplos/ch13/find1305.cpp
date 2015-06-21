/*
 * FIND1305.CPP
 *
 * This program is used in Chapter 13 to demonstrate
 * the adjacent_find() algorithm.  It works by reading in all
 * the words from a text file (find1305.cpp!), then
 * searching for all adjacent matches.
 * Here are some matches: match match match
 *                        a a b c c d e e e
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <fstream.h>
#include <cstring.h>

#include <algo.h>
#include <list.h>

main()
{
    ifstream f( "find1505.cpp" );
    if ( !f ) {
        cerr << "Could not open input file!\n";
        return 1;
    }
    list<string> l;

    while ( f ) {
        string word;
        f >> word;
        l.push_back( word );
    }
    f.close();
    for ( ; ; ) {
        list<string>::iterator i;
        i = adjacent_find( l.begin(), l.end() );
        if ( i == l.end() )
            break;
        cout << "Adjacent match: "
             << *i
             << endl;
        l.erase( i );
    }
    return 1;
}
