//
// PQUE0705.CPP
//
// This program is used in Chapter 7 to demonstrate
// the use of function objects with a priority queue.
// Since the queue is built to contain objects of a user
// defined type, we can't use a built in function
// such as less<T>.  Instead, I have to define a class
// dedicated to performing the comparison between
// two objects of type foo.
//

//
// Borland 4.x workarounds.
//

#define __MINMAX_DEFINED
#pragma option -vi-

#include <vector.h>
#include <stack.h>
#include <function.h>
#include <iostream.h>

struct foo {
    char *name;
    int age;
    foo( char *n = "", int a = 0 ){ name = n; age = a; }
};

//
// The function object used to compare two objects
// of type foo will be created from this class.
// class test_foo doesn't do anything useful except
// compare objects of type foo.
//
class younger_foo {
  public :
    int operator()( const foo &a, const foo &b )
    {
        return a.age > b.age;
    }
};

main()
{
    priority_queue< vector<foo>, younger_foo > a;

    a.push( foo( "Mark", 38 ) );
    a.push( foo( "Marc", 25 ) );
    a.push( foo( "Bill", 47 ) );
    a.push( foo( "Andy", 13 ) );
    a.push( foo( "Newt", 44 ) );
    while ( !a.empty() ) {
        cout << a.top().name << endl;
        a.pop();
    }
    cout << endl;
    return 1;
}
