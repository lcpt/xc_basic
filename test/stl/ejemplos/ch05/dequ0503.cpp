//
// DEQU0503.CPP
//
// This example program is used in Chapter 5 to demonstrate
// some features of deque<T>::iterator.  It looks at the
// internals of these iterators by creating a derived class
// call public_iterator<T>. This public function has access
// to all of the deque<T>::iterator member data, and provides
// member functions to copy their values.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <deque.h>
#include <iostream.h>
#include <iomanip.h>
#include <strstrea.h>
#include <string.h>

//
// Class public_iterator is used to let us look at
// the contents of a deque<T> iterator.  The iterator
// members are protected, so we derive this new
// class that give us access to all of the protected
// members of the iterator.  To look at the members,
// just copy the iterator into one of these, using
// either the copy constructor or the assigment operator.
//

template<class T>
class public_iterator : public deque<T>::iterator {
  public :
    public_iterator( const deque<T>::iterator &a )
        : deque<T>::iterator( a ) {}
    friend ostream& operator<<( ostream&,
                                public_iterator<T>&);
    deque<T>::pointer *get_node(){ return node; }
};

//
// Once we create a public_iterator object, this routine
// is called to print its members.  There are four components
// to any given iterator.
//

template<class T>
ostream& operator<<(ostream& s, public_iterator<T>& i )
{
    s << "Curr: " << i.current << "  "
      << "1st: "  << i.first << "  "
      << "Last: " << i.last << "  "
      << "Node: " << i.node;
    return s;
}

//
// This template function does a dump of some of the iterator
// data for a container.  It accomplishes this by assigning the
// begin() and end() iterators from the container to iterators
// in my derived class.  I have access to the information from
// the derived class, so gets printed out.
//
template<class T>
void dump_iterators( deque<T>& a, char *label )
{
    cout << "\nIterator dump: "
         << label
         << endl;
    public_iterator<T> start = a.begin();
    public_iterator<T> finish = a.end();
    cout << "Start:  " << start << endl;
    cout << "Finish: " << finish << endl;
    cout << "Number of allocated_blocks : ";
    if ( finish.get_node() == 0 )
        cout << "0";
    else
        cout << (finish.get_node() - start.get_node() + 1);
    cout << endl;
}

//
// This program creates a deque<double> container.
//
main()
{
    deque<double> a;

    dump_iterators( a, "Immediately after ctor" );
    a.push_front( -1 );
    dump_iterators( a, "After one push_back()" );
    for ( int i = 0 ; i < 8000 ; i++ ) {
        if ( ( i % 1024 ) == 0 )
            cout << i;
        a.push_front( i );
        if ( ( i % 1024 ) == 0 )
            cout << '\r';
    }
    dump_iterators( a, "After filling container" );
    a.erase( a.begin(), a.end() );
    dump_iterators( a, "After erasure" );
    return 1;
}

