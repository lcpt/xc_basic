//
// TEST0903.CPP
//
// This program demonstrates the problem with using
// templates to handle new pointer types, such as
// __far or __huge pointers.  The program below
// compiles properly under large model, but generates
// an error when compiled in small model.  The compiler
// won't generalize a template function for "T *" to
// include "T _far *".
//
// Note that this program is designed for 16 bit segmented
// memory models.
//

#include <iostream.h>

template<class T>
void dump( T *p )
{
    cout << "Value to dump = "
         << *p
         << endl;
}

main()
{
    char *letter = "a";
    int num = 1;
    int *number = &num;
    int __far *farnumber = &num;

    dump( letter );
    dump( number );
    dump( farnumber );
    return 1;
}
