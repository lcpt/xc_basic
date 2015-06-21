//
// DEQU0502.CPP
//
// This example program is used in Chapter 5 to demonstrate
// some features of deque<T>.  It looks at the internals of
// deque<T> by creating a derived class called public_deque<T>.
// This public function has access to all of the deque<T>
// member data, and provides member functions to copy their
// values.
//

//
// Borland-specific workarounds.
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <deque.h>
#include <iostream.h>
#include <iomanip.h>
#include <strstrea.h>
#include <string.h>

//
// class public_deque<T> does nothing useful on its own.
// It exists solely to allow us to look at the protected
// members of deque<T>.  The various access functions
// defined below provide read-only access.  Note that
// the other constructors supported by deque<T> can all
// be added to this class fairly easily.  A this time,
// only the default ctor is defined.
//

template<class T>
class public_deque : public deque<T> {
  public :
    public_deque() : deque<T>() {}
    deque<T>::map_pointer get_map(){ return map; }
    deque<T>::size_type get_map_size(){ return map_size; }
    deque<T>::size_type get_length(){ return length; }
    deque<T>::size_type get_buffer_size() {
        return buffer_size;
    }
};

//
// This class keeps track of how often its member functions
// are called.  This helps provide an indication of how
// efficiently the class is being used.
//

class foo {
    public :
        static int ctor_count;
        static int copy_count;
        static int assign_count;
        static int dtor_count;
        char a[ 2000 ];
        foo(){ ctor_count++; }
        foo( const foo& ){ copy_count++; }
        void operator=(const foo&){ assign_count++; }
        ~foo(){ dtor_count++; }
};

int foo::ctor_count = 0;
int foo::copy_count = 0;
int foo::assign_count = 0;
int foo::dtor_count = 0;

foo static_foo;

//
// This routine displays some of the member functions
// of the deque<T> object.
//
template<class T>
void dump_public_deque( public_deque<T> & a, char *note )
{
    cout << "\nDump of deque<T> " << note << ":" << endl;
    cout << "map = "          << a.get_map() << "  "
         << "map_size = "     << a.get_map_size() << "  "
         << "deque_length = " << a.get_length() << endl;
}

//
// The body of the test program.
//
main()
{
    public_deque<foo> a;

    cout << "deque<foo>::buffer_size  = "
         << a.get_buffer_size()
         << endl;
    cout << "deque<foo>::max = "
         << a.max_size()
         << endl;
    dump_public_deque( a, "just after creation" );
//
// First I add 32 elements to the deque.
//
    for ( int i = 0 ; i < 32 ; i++ ) {
        cout << i;
        a.push_back( static_foo );
        cout << '\r';
    }
    dump_public_deque( a, "after adding 32 blocks" );
//
// Now we can add to the end and take away from the
// beginning.  We do this more than 1K times so that
// we are sure to reach the end of the 1K map.
//
    for ( i = 0 ; i < 1024 ; i++ ) {
        cout << i;
        a.pop_front();
        a.push_back( static_foo );
        cout << '\r';
    }
    dump_public_deque( a, "after 1024 add/delete cycles" );
//
// Now erase all of the elements in the deque, and see
// what it looks like afterwards.
//
    a.erase( a.begin(), a.end() );
    dump_public_deque( a, "after erasing all elements" );
//
// Finally, dump the class statistics.
//
    cout << endl;
    cout << "ctor count = " << foo::ctor_count << endl;
    cout << "copy count = " << foo::copy_count << endl;
    cout << "assign count = " << foo::assign_count << endl;
    cout << "dtor count = " << foo::dtor_count << endl;
    return 1;
}

