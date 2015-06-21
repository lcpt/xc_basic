//
// DEQU0504.CPP
//
// This example program is used in Chapter 5 to
// perform some *extremely* crude benchmarks on classes
// vector<T> and deque<T>.  I throw in testing of T[]
// to use as a baseline.  This routine just puts
// a bunch of ints into an array, then adds them
// up.  The goal is to see how fast we are able to
// iterate through the array.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED

#include <deque.h>
#include <vector.h>
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
    int i = rand();
    for ( int j = 0 ; j < runs ; j++ ) {
        for ( T p = begin; p != end; p++ )
            *p = i++;
        long total = 0;
        for ( p = begin; p != end; p++ )
            total += *p;
        cout << '.';
        if ( results [ j ] == 0 )
            results[ j ] = total;
        else if ( results[ j ] != total )
            cout << "Error!\n";
    }
    long delta = time( 0 ) - t1;
    cout << " elapsed time: "
         << delta
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

    cout << "Testing vector<T> ";
    vector<short int> b( 32000 );
    test( b.begin(), b.end() );

    cout << "Testing deque<T>  ";
    deque<short int> c( 32000 );
    test( c.begin(), c.end() );

    return 1;
}

