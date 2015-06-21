//
// LIST0604.CPP
//
// This example program is used in Chapter 6 to
// perform some *extremely* crude benchmarks on classes
// vector<T>, deque<T>, list<T>.  This routine just inserts
// a load of doubles into a container, then checks to see
// how long the entire operation took.
//
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED

#include <deque.h>
#include <vector.h>
#include <list.h>
#include <iostream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <time.h>

//
// This template function performs the actual test.
// The insertion point is determined by iterator j.
// I fiddle with j to ensure that all insertions are
// done in the very middle of the container.
//

template<class T>
void test( T& a )
{
    time_t t1 = time( 0 );
    T::iterator j = a.begin();
    for ( int i = 0 ; i < 8000 ; i++ ) {
        if ( ( i % 256 ) == 0 )
            cout << '.';
        j = a.insert( j, i );
        if ( i % 2 )
            j++;
    }
    cout << "\n";
    cout << "Size = " << a.size() << endl;
    cout << "Elapsed time: "
         << time( 0 ) - t1
         << " seconds"
         << endl;
    a.erase( a.begin(), a.end() );
}

//
// Main just sets up the containers, then calls
// the template function to test them out.
//

main()
{
    cout << "Testing vector<T> ";
    vector<double> *a = new vector<double>;
    test( *a );
    delete a;

    cout << "Testing deque<T>  ";
    deque<double> *b = new deque<double>;
    test( *b );
    delete b;

    cout << "Testing list<T>  ";
    list<double > *c = new list<double>;
    test( *c );
    delete c;

    return 1;
}

