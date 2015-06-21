//
// MMAP0808.CPP
//
// This program is used in Chapter 8 to demonstrate the
// use of the multimap<> container.  It does this by
// scanning this text file, and adding each appearance
// of a word to the container, along with the line
// number.  After the entire contents of the file have
// been read, I go through the container and print the
// line numbers that each word appears on.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include "multimap.h"

#include <cstring.h>
#include <iostream.h>
#include <strstream.h>
#include <fstream.h>
#include <ctype.h>
#include <cstring.h>


main()
{
    ifstream input( "mmap0808.cpp" );
    if ( !input ) {
        cerr << "Failed to open file!\n";
        return 1;
    }
    string word;
    int line;
    multimap< string, int, less<string> > words;

    for( line = 1; input ; line++ ) {
        char buf[ 129 ];
        input.getline( buf, 128 );
//
// I throw away all of the punctuation chars
// in each of the words, leaving just alphas.
//
        for ( char *p = buf ; *p != '\0'; p++ )
            if ( !isalnum( *p ) )
                *p = ' ';
        istrstream i( buf );
        while (i) {
            i >> word;
            if ( word != "" )
                words.insert( pair<const string,int>( word, line ) );
        }
    }
    input.close();
//
// This is where I print out the results.  I have to
// print all the appearances of each word.  The
// upper_bound() function tells me where the last
// appearance of each word will be.
//
    multimap< string, int, less<string> >::iterator j;
    multimap< string, int, less<string> >::iterator k;
    for ( j = words.begin() ; j != words.end() ; ) {
        k = words.upper_bound( (*j).first );
        cout << (*j).first << " : ";
        for ( ; j != k ; j++ )
            cout << (*j).second << " ";
        cout << endl;
    }
    return 0;
}

