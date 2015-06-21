//
// SET0803.CPP
//
// This program is used in Chapter 8 to demonstrate the
// insertion function for containers of type set<>.
// This program introduces the oddball return type
// of pair<iterator,bool> that is used by some of
// the insertion functions in the associative classes.
//



#include <iostream.h>
#include "set.h"

main()
{
    typedef set<int, less<int> > my_set;
    my_set a;
    int data[] = { 1, 2, 3, 4, 3, 4, 5, 6, 0 };

    for ( int i = 0 ; data[ i ] != 0 ; i++ ) {
        pair< my_set::iterator, bool> x = a.insert( data[ i ] );
        cout << "insertion of "
             << data[ i ]
             << ( x.second ? " worked" : " failed" )
             << endl;
    }
    return 0;
}
