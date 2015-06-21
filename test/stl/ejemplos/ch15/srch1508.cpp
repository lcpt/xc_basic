/*
 * SRCH1508.CPP
 *
 * This program is used in Chapter 15 to demonstrate
 * the binary_search algorithm.  It reads in all of
 * the strings from this source file, then attempts
 * to locate a few of them.  Remember that the strings
 * will be read in according to the rules the string
 * insertion operator uses for parsing.
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

void test( vector<string> &v, string s )
{
    bool result;
    result = binary_search( v.begin(), v.end(), s );
    cout << "\"" << s << "\""
         << " was "
         << ( result ? "" : "not " )
         << "found"
         << endl;
}

main()
{
    ifstream f( "srch1508.cpp" );
    if ( !f ) {
        cerr << "Couldn't open input file!\n";
        return 1;
    }

    vector<string> v;

    while ( f ) {
        string word;
        f >> word;
        v.push_back( word );
    }
    f.close();
    sort( v.begin(), v.end() );
    test( v, "ifstream" );
    test( v, "Woodchuck" );
    test( v, "Barsoom" );
    test( v, v[ 20 ] );

    return 1;
}
