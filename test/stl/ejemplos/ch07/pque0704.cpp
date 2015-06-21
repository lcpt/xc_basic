//
// PQUE0704.CPP
//
// This program is used in Chapter 7 to demonstrate
// the use of a priority queue.  I create a prioirity
// queue based on deque<char>, then place the contents
// of this file (PQUE0704.CPP) into the queue.  I pull
// the characters out one at a time to demonstrate
// the fact that they will be removed in sorted order.
//
// Note that I use greater<T> instead of the conventional
// less<T>, so that the queue stores items smallest
// first instead of largest first.
//

//
// Borland 4.x workarounds.
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <deque.h>
#include <stack.h>
#include <function.h>
#include <iostream.h>
#include <fstream.h>

main()
{
    priority_queue< deque<char>, greater<char> > a;
    ifstream infile( "pque0704.cpp" );

    while ( infile ) {
        char c;
        infile >> c;
        if ( c != '\n' && infile )
            a.push( c );
    }
    while ( !a.empty() ) {
        cout << a.top();
        a.pop();
    }
    cout << endl;
    return 1;
}
