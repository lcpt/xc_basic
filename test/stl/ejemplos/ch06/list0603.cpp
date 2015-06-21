//
// LIST0603.CPP
//
// This example program is used in Chapter 6 to
// perform some *extremely* crude benchmarks on classes
// vector<T>, deque<T>, list<T>.  I throw in testing of T[]
// to use as a baseline.  This routine just puts
// a bunch of ints into a container, then adds them
// up.  The goal is to see how fast we are able to
// iterate through the container.
//

//
// Borland 4.x workarounds.
//
#define __MINMAX_DEFINED

#include <deque.h>
#include <vector.h>
#include <list.h>
#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <time.h>

const int runs = 25;
long results[ runs ] = { 0 };

//
// This template function performs the actual test.
// To get reasonable run times, you may have to
// tinker with "runs" to get decent multiples of
// a second for each of the three types tested here.
//
template<class T>
void test( T begin, T end )
{
    srand( 1 );
    time_t t1 = time( 0 );
    unsigned int j = rand();
    for ( int i = 0 ; i < runs ; i++ ) {
        for ( T p = begin; p != end; p++ )
            *p = j++;
        long total = 0;
        for ( p = begin; p != end; p++ )
            total += *p;
        cout << '.';
        if ( results [ i ] == 0 )
            results[ i ] = total;
        else if ( results[ i ] != total )
            cout << "Error!\n";
    }
    cout << " elapsed time: "
         << time( 0 ) - t1
         << " seconds"
         << endl;
}
//
// Main just sets up the containers, then calls
// the template function to test them out.
// Note that the allocation of *all* the space
// for the container is done here, so it doesn't
// come into play in the benchmarks.
//
main()
{
    short int *a = new short int[ 32000 ];
    cout << "Testing T[]       ";
    test( a, a + 32000 );
    delete a;

    cout << "Testing vector<T> ";
    vector<short int> *b = new vector<short int>( 32000 );
    test( b->begin(), b->end() );
    delete b;

    cout << "Testing deque<T>  ";
    deque<short int> *c = new deque<short int>( 32000 );
    test( c->begin(), c->end() );
    delete c;

    cout << "Testing list<T>  ";
    list<short int> *d = new list<short int>( 32000 );
    test( d->begin(), d->end() );
    delete d;

    return 1;
}

