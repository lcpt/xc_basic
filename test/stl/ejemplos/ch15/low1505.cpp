/*
 * LOW1505.CPP
 *
 * This program is used in Chapter 15 to demonstrate
 * the lower_bound algorithm.  A vector of strings
 * is created and then sorted.  Once that valid input
 * sequence has been determined, lower_bound() is used
 * to find the correct insertion point for three
 * new strings.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <algo.h>
#include <vector.h>

main()
{
    vector<string> a;
    a.push_back( "Microsoft" );
    a.push_back( "Apple" );
    a.push_back( "IBM" );
    a.push_back( "Next" );
    a.push_back( "Sun" );
    a.push_back( "Symantec" );
    a.push_back( "HP" );
    a.push_back( "Watcom" );

    sort( a.begin(), a.end() );
    cout << "Input sequence : "
         << "\n";
    copy( a.begin(),
          a.end(),
          ostream_iterator<string>( cout, " " ) );
    cout << endl;

    string *p = lower_bound( a.begin(),
                             a.end(),
                             "Oracle" );
    cout << "lower_bound( \"Oracle\" ) returned "
         << ( p - a.begin() )
         << endl;
    a.insert( p, "Oracle" );

    p = lower_bound( a.begin(),
                     a.end(),
                     "Novell" );
    cout << "lower_bound( \"Novell\" ) returned "
         << ( p - a.begin() )
         << endl;
    a.insert( p, "Novell" );

    p = lower_bound( a.begin(),
                     a.end(),
                     "Lotus" );
    cout << "lower_bound( \"Lotus\" ) returned "
         << ( p - a.begin() )
         << endl;
    a.insert( p, "Lotus" );

    cout << "Final sequence : "
         << "\n";
    copy( a.begin(),
          a.end(),
          ostream_iterator<string>( cout, " " ) );
    cout << endl;

    return 1;
}
