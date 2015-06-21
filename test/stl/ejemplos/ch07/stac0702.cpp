//
// STAC0702.CPP
//
// This program is used in Chapter 6 to demonstrate
// one method of overcoming that lack of a
// constructor for the stack<Container> container
// adaptor.  When creating a stack based on container
// type deque<char>, I derive a new class called
// my_deque.  my_deque<> is exactly like deque<char>,
// except that its default constructor initializes
// the deque with the correct initial number of
// elements.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <deque.h>
#include <stack.h>
#include <iostream.h>

//
// This derived class is used to force the
// deque<char> container used to build the
// stack to start life with 15 copies of
// the letter 'a'.
//

class my_deque : public deque<char>
{
    public :
        my_deque() : deque<char>( 15, 'a' ){}
};

//
// This short program demonstrates that the
// stack does in fact start out with 15
// predefined elements.
//
main()
{
    stack<my_deque> a;

    a.push( 'X' );
    a.push( 'Y' );
    a.push( 'Z' );
    cout << "a.size() = " << a.size() << endl;
    while ( !a.empty() ) {
        cout << a.top();
        a.pop();
    }
    cout << endl;
    return 1;
}

