//
// MAP0807.CPP
//
// This program is used in Chapter 8 to demonstrate
// the use of associative containers.  In this case,
// the program scans an input file and keeps a count
// of all the strings it reads in.  Normally, you
// might want to do this with a map<string,int,...>,
// but I decided to define a special counter class.
// I get the convenience of having the counters
// initialized with a value of 0.  The first time
// I reference a string entry, it will get created
// and incremented in one operation.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <cstring.h>
#include <iostream.h>
#include <fstream.h>

#include "map.h"

struct counter {
    int value;
    counter() : value(0) {}
    void operator++(int){ value++; }
};

ostream& operator<<(ostream& s, counter& a )
{
    return s << a.value;
}

main()
{
    map<string,counter,less<string> > counts;
    ifstream f( "map0807.cpp" );
    string token;

    for ( ; ; ) {
        f >> token;
        if ( f )
            counts[ token ]++;
        else
            break;
    }
    map<string ,counter,less<string> >::iterator i;
    for ( i = counts.begin(); i != counts.end() ; i++ )
        cout << (*i).first
             << ", "
             << (*i).second
             << endl;
    return 1;
}
