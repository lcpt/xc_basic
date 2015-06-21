/*
 * MAKE1703.CPP
 *
 * This program is used in Chapter 17 to demonstrate
 * the make_heap algorithm.  It keeps track of the
 * number of operations performed while making the
 * heap, to help demonstrate that the complexity of
 * make_heap() is linear.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <stdlib.h>

#include <algo.h>
#include <heap.h>
#include <vector.h>


struct bob {
    int x;
    static int comp_count;
    static int ctor_count;
    static int copy_count;
    static int assign_count;

    bob() : x( 0 ){ ctor_count++; }
    bob( int i ) : x( i ){ ctor_count++; }
    bob( const bob &a ) : x( a.x ){ copy_count++; }
    void operator=( const bob& a ){ assign_count++; x = a.x; }
    bool operator<( const bob& a ) {
        comp_count++;
        return x < a.x;
    }
};

int bob::comp_count = 0;
int bob::ctor_count = 0;
int bob::copy_count = 0;
int bob::assign_count = 0;

int main()
{
    srand( time( 0 ) );
    vector< bob > a;
    const int k = 1000;

    for ( int i = 0 ; i < k; i++ )
        a.push_back( bob( rand() ) );
    make_heap( a.begin(), a.end() );
    cout << "Nodes      : " << k            << endl;
    cout << "Comparisons: " << bob::comp_count   << endl;
    cout << "Ctors      : " << bob::ctor_count   << endl;
    cout << "Copies     : " << bob::copy_count   << endl;
    cout << "Assigns    : " << bob::assign_count << endl;
    return 1;
}

