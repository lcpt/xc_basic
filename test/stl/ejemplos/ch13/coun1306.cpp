/*
 * COUN1306.CPP
 *
 * This program is used in Chapter 13 to demonstrate
 * the count_if() algorithm.  It works by reading in all
 * the words from a text file (coun1306.cpp), then
 * counting all the words that contain only letters.
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
#include <ctype.h>

#include <algo.h>
#include <deque.h>

bool my_count( string &a )
{
    for ( int i = 0 ; i < a.length() ; i++ )
        if ( !isalpha( a[ i ] ) )
            return 0;
    return 1;
}

main()
{
    ifstream f( "coun1306.cpp" );
    if ( !f ) {
        cerr << "Error opening input file!\n";
        return 1;
    }
    deque<string> dq;

    while ( f ) {
        string word;
        f >> word;
        dq.push_back( word );
    }
    f.close();
    int n = 0;
    count_if( dq.begin(), dq.end(), my_count, n );
    cout << "Total words: "
         << dq.size()
         << endl;
    cout << "All alpha count: "
         << n
         << endl;
    return 1;
}
