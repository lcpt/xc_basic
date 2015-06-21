//
// BIND1212.CPP
//
// This program is used in Chapter 12 to demonstrate
// the use of bind1st() and bind2nd().  In both cases,
// the function is used to bind an integer to a numeric
// function object, which in turn modifies data in a
// vector.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <vector.h>
#include <function.h>
#include <algo.h>

main()
{
    vector<long> lv;
    for ( int i = 0 ; i < 10 ; i++ )
        lv.push_back( i * i );
    cout << "Before: ";
    copy( lv.begin(),
          lv.end(),
          ostream_iterator<long>( cout, " " ) );
    cout << endl;
    transform( lv.begin(),
               lv.end(),
               lv.begin(),
               bind1st( times<long>(), 100L ) );
    cout << "After multiplying by 100: ";
    copy( lv.begin(),
          lv.end(),
          ostream_iterator<long>( cout, " " ) );
    cout << endl;
    transform( lv.begin(),
               lv.end(),
               lv.begin(),
               bind2nd( minus<long>(), 144L ) );
    cout << "After subtracting 144: ";
    copy( lv.begin(),
          lv.end(),
          ostream_iterator<long>( cout, " " ) );
    cout << endl;
    return 1;
}
