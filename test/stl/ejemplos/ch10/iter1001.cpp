//
// ITER1001.CPP
//
// This program creates a simple input
// iterator class, then uses it as the
// input to the copy() function.  It uses
// a conventional ostream_iterator for
// the output.
//

#include <iostream.h>
#include <string.h>

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-


#include "algo.h"
#include "iterator.h"

//
// struct input takes a pointer to
// a character string as input to
// its constructor.  When dereferenced,
// it returns the individual characters
// in the string.  The increment operator
// advances it to the next character in
// the string.  Note that this type
// uses the default copy constructor
// that the compiler generates.
//

struct input {
    char *current;
    input( char *p ){ current = p; }
    operator*(){ return *current; }
    input operator++(int) {
        input temp( *this );
        current++;
        return temp;
    }
    bool operator==(const input& y) const {
        return current == y.current;
    }
    input end() {
        char *p = current + strlen( current );
        return input( p );
    }
};

main()
{
    ostream_iterator<char> out(cout);
    input in( "This is a test" );

    copy( in, in.end(), out );
    return 1;
}

