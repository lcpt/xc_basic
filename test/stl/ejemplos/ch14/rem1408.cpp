/*
 * REM1408.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the remove algorithm.  The program creates a simple
 * list of names, then calls remove() to remove every
 * name that contains the word "Bill".  Note that
 * remove() compacts the data in the vector, but leaves
 * some unused objects at the end of the container. It
 * takes a call to erase() to actually free up the extra
 * objects.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <string.h>
#include <vector.h>
#include <algo.h>

bool no_bill( char *s )
{
    if ( strstr( s, "Bill" ) )
        return 1;
    else
        return 0;
}

template<class Stream, class OutputIterator>
void output( Stream &s,
             OutputIterator begin,
             OutputIterator end )
{
    while ( begin != end ) {
        s << *begin++;
        if ( begin != end )
            cout << ", ";
    }
    cout << endl;
}

main()
{
    vector<char *> a;
    a.push_back( "Bill Gates" );
    a.push_back( "Bill Clinton" );
    a.push_back( "George Bush" );
    a.push_back( "Abe Lincoln" );
    a.push_back( "Michael Spindler" );

    cout << "Initial vector:\n";
    output( cout, a.begin(), a.end() );
    char **last = remove_if( a.begin(), a.end(), no_bill );
    cout << "\nVector after remove():\n";
    output( cout, a.begin(), a.end() );
    a.erase( last, a.end() );
    cout << "\nVector after erase:\n";
    output( cout, a.begin(), a.end() );
    return 1;
}
