//
// SET0802.CPP
//
// This program is used in Chapter 8 to demonstrate the
// access functions used in set<Key,Compare>.  I use a
// constructor that initializes the set, then print
// out some of its metrics, as well as all of its elements.
//



#include <iostream.h>
#include "set.h"

main()
  {
    int init[] = { 4, 10, 1, 3, 22, 1, 100, -100 };
    set<int, less<int> > a( init, init + 8 );

    cout << "a contains "
         << a.size()
         << " elements"
         << endl;
#ifndef __GNUG__
    cout << "The maximum size is "
         << a.max_size()
         << endl;
#endif

    cout << "a is "
         << ( a.empty() ? "" : "not " )
         << "empty"
         << endl;

    cout << "a contains : ";
    for ( set<int, less<int> >::iterator i = a.begin() ;
          i != a.end() ;
          i++ )
        cout << *i << " ";
    cout << endl;

    cout << "a reversed contains: ";
    set<int, less<int> >::reverse_iterator j;
    j = a.rbegin();
    while ( j != a.rend() )
        cout << *j++ << " ";
    cout << endl;

    return 1;
  }
