/*
 * TRAN1404.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the transform algorithm.  It applies an area function
 * to a queue full of circles, and routes the output
 * to cout via an iostream iterator object.
 *
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <deque.h>
#include <algo.h>

struct circle {
    int x_origin;
    int y_origin;
    int radius;
    circle( int x = 0, int y = 0, int r = 1 ) :
        x_origin(x), y_origin(y), radius( r ){}
};

double area( const circle &c )
{
    return 3.1459 * c.radius * c.radius;
}

main()
{
    deque<circle> a;

    a.push_back( circle( 1, 1, 5 ) );
    a.push_back( circle( 2, 2, 2 ) );
    a.push_back( circle( 0, -100, 15 ) );

    cout << "circle areas: ";
    transform( a.begin(),
               a.end(),
               ostream_iterator<double>( cout, " " ),
               area );
    return 1;
}
