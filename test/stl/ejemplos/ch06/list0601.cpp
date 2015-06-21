//
// LIST0601.CPP
//
// This example program is used in Chapter 6 to demonstrate
// some features of list<T>.  The program reads in a text file,
// strips out all lines that start with a comment, then
// prints the resulting file back out to cout.  Borland defines
// their string class in cstring.h, other compilers may use a
// different file.
//

//
// Borland 4.x workarounds.  Note that when using STL with
// Borland 4.x, you must include the RTL headers
// (fstream and cstring) before list.h.
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <fstream.h>
#include <cstring.h>
#include <list.h>

//
// These comment lines should be stripped from the
// output file.
//

main()
{
    ifstream input( "list0601.cpp" );
    if ( !input ) {
        cerr << "Open file error, list0601.cpp not found!\n";
        return 1;
    }
    list<string> text;

    while ( input ) {
        char buf[ 133 ];
        input.getline( buf, 132, '\n' );
        text.push_back( buf );
    }
    input.close();
    list<string>::iterator i = text.begin();
    while ( i != text.end() ) {
        if ( (*i).substr( 0, 2 ) == "//"  )
            text.erase( i++ );
        else
            i++;
    }
    for ( i = text.begin() ; i != text.end() ; i++ )
        cout << *i << endl;
    return 0;
}

