//
// PTR1213.CPP
//
// This program is used in Chapter 12 to demonstrate
// the use of the ptr_fun() function adaptor.  ptr_fun()
// converts an existing function pointer (not a function
// object) to a function object that can be used with
// any of the classes in the STL.  In this case, we use
// stricmp() to compare for equality of character strings.
// The first time find_if() is called, you get a demo of
// how *not* to do it.  Compare two pointers for equality
// just isn't going to work.  Instead, you have to compare
// the actual strings, which is what happens
// the second time.  Note also that stricmp() returns 0
// on a match, so we have to negate it with a not1() to
// return the boolean expected by the STL.  Wow!
//

//
// Borland 4.x workarounds
//

#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <string.h>

#include <list.h>
#include <algo.h>

main()
{
    list<char *> vec;
    vec.push_back( "IDG" );
    vec.push_back( "M&T" );
    vec.push_back( "SAMS" );
    vec.push_back( "Microsoft" );
//
// This is the *wrong* way to compare strings.
//
    list<char *>::iterator p =
        find_if( vec.begin(),
                 vec.end(),
                 bind2nd( equal_to<char*>(), "m&t" ) );
    int i = 0;
    distance( vec.begin(), p, i );
    cout << "Offset = "
         << i
         << ".  Matched data = "
         << ( p == vec.end() ? "<end>" : *p )
         << endl;
//
// This is the right way to compare strings.
//
    p = find_if( vec.begin(),
                 vec.end(),
                 not1( bind2nd( ptr_fun( stricmp ), "m&t" ) ) );
    i = 0;
    distance( vec.begin(), p, i );
    cout << "Offset = "
         << i
         << ".  Matched data = "
         << ( p == vec.end() ? "<end>" : *p )
         << endl;

    return 1;
}
