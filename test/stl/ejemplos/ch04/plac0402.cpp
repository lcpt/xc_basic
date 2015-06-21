//
// PLAC0402.CPP
//
// This short program shows how the placement syntax
// can be used with the new operator to create objects
// anywhere in memory.  This allows the programmer
// to control precisely where and how objects are
// created, instead of leaving things up to the
// heap manager.  This program creates an object of
// type foo in an integer array on the stack.  It then
// deletes the same object using an explicit call to
// the destructor.  The program prints out the contents
// of the integer array to verify that the ctor and
// dtor have actually been invoked.
//

#include <iostream.h>

class foo {
        int a, b, c;
    public :
        foo() : a(101), b(202), c(303) {}
        ~foo(){ a = 404; b = 505; c = 606; }
};

//
// This version of new will be called when I create
// an object of type voo using the placement syntax.
//
inline void* operator new( size_t, void* p ) { return p; }

main()
{
    int x[ 10 ];
//
// Create an object using the memory in x
//
    foo *p = new( x ) foo; //new using placement syntax
    for ( int i = 0 ; i < 10 ; i++ )
        cout << x[ i ] << ' ';
    cout << endl;
//
// Now destroy the same object
//
    p->foo::~foo();
    for ( i = 0 ; i < 10 ; i++ )
        cout << x[ i ] << ' ';
    cout << endl;
    return 1;
}
