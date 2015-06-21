/*
 * FIND1302.CPP
 *
 * This program is used in Chapter 13 to demonstrate
 * the find() algorithm.  It works by reading in all
 * the words from a text file (find1302.cpp!), then
 * searching for the appearances of "Woodchuck" and
 * "Womprat".
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
#include <vector.h>

main()
{
    ifstream f( "find1302.cpp" );
    if ( !f ) {
        cerr << "Failed to open input file!\n";
        return 1;
    }
    vector<string> v;

    while ( f ) {
        string word;
        f >> word;
        v.push_back( word );
    }
    f.close();
//
// Search targets : Woodchuck Womprat
//
    vector<string>::iterator i1, i2;
    i1 = find( v.begin(), v.end(), "Woodchuck" );
    i2 = find( v.begin(), v.end(), "Womprat" );
    int n1 = 0, n2 = 0;
    distance( v.begin(), i1, n1 );
    distance( v.begin(), i2, n2 );
    cout << "Woodchuck found at position "
         << n1
         << endl
         << "Womprat found at position "
         << n2
         << endl;
    return 1;
}
