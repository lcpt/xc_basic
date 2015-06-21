//
//  DEQU0501.CPP
//
//  This demo program is used in Chapter 5 to compare the
//  number of constructor calls that are made when
//  filling a deque vs. the number when filling a
//  vector.
//

//
// Borland 4.5 workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <vector.h>
#include <deque.h>
#include <iostream.h>
#include <iomanip.h>
#include <strstrea.h>
#include <string.h>

class foo {
    public :
        int a[ 64 ];
        foo( int i = -1 ){ ctor_count++; }
        foo( const foo &rhs ){ copy_count++; }
        void operator=( const foo & ){ assign_count++; }
        ~foo(){ dtor_count++; }
        static long ctor_count;
        static long copy_count;
        static long dtor_count;
        static long assign_count;
};

long foo::ctor_count = 0;
long foo::copy_count = 0;
long foo::dtor_count = 0;
long foo::assign_count = 0;

template<class T1, class T2>
void exercise( T1 &container, T2 * )
{
    for ( int j = 0 ; j < 511; j++ )
        container.push_back( T2( j ) );
    container.erase( container.begin(), container.end() );
}

main()
{
    cout << "Testing vector<foo>\n";
    vector<foo> a;
    exercise( a, (foo *) 0 );
    cout << "Ctor count:   " << foo::ctor_count   << "\n";
    cout << "Copy count:   " << foo::copy_count   << "\n";
    cout << "Assign count: " << foo::assign_count << "\n";
    cout << "Dtor count:   " << foo::dtor_count   << "\n";

    foo::ctor_count = 0;
    foo::copy_count = 0;
    foo::assign_count = 0;
    foo::dtor_count = 0;

    cout << "\nTesting deque<foo>\n";
    deque<foo> b;
    exercise( b, (foo *) 0 );
    cout << "Ctor count:   " << foo::ctor_count   << "\n";
    cout << "Copy count:   " << foo::copy_count   << "\n";
    cout << "Assign count: " << foo::assign_count << "\n";
    cout << "Dtor count:   " << foo::dtor_count   << "\n";
    return 1;
}

