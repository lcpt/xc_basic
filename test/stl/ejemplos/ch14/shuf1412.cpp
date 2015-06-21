/*
 * SHUF1412.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the random_shuffle algorithm.  It does so using by
 * creating a deck of cards, then performing a single
 * shuffle.
 *
 * Note that this program uses the __long_random()
 * function, which is defined in random.cpp.  To
 * keep things simple, this program actually includes
 * random.cpp as an include file.  You probably won't
 * want to do this in a real programming project.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <vector.h>
#include <algo.h>
#include <random.cpp>

struct card {
    int value;
    card( int v = 0 ) : value( v ){;}
};

ostream & operator<<( ostream &s, card c )
{
    static count = 0;
    s << "A23456789TJQK"[ c.value % 13 ];
    s << "DCHS"[ c.value / 13 ];
    if ( ( ++count % 13 ) == 0 )
        s << endl;
    else
        s << " ";
    return s;
}

main()
{
    vector<card> a;
    for ( int i = 0 ; i < 52 ; i++ )
        a.push_back( card( i )  );
    cout << "Before shuffling: " << endl;;
    copy( a.begin(), a.end(), ostream_iterator<card>( cout ) );
    cout << endl;

    random_shuffle( a.begin(), a.end() );
    cout << "After shuffling: " << endl;;
    copy( a.begin(), a.end(), ostream_iterator<card>( cout ) );
    cout << endl;

    return 1;
}
