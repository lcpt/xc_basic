//
// SORT1206.CPP
//
// This program uses the sort() algorithm to sort an array
// of C strings (char pointers.)  If we use the default
// comparison operator provided by sort(), the array
// gets sorted based on the value of the pointers, which
// isn't useful!  So this program provides a comparison object
// which actually compares the strings pointed to by the
// object, giving a much more useful comparison.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <string.h>

#include <algo.h>
#include <function.h>
#include <projectn.h>

char *test[]={ "Jay", "Dave", "oprah", "Ricky", "GERALDO" };
const int NELEM = sizeof( test ) / sizeof( char * );

struct cmp {
    bool operator()( char *a, char *b )
    {
        return stricmp( a, b ) < 0;
    }
};

main()
{
     sort( test, test + NELEM, cmp() );
     cout << "After sorting: ";
     copy( test,
           test + NELEM,
           ostream_iterator<char*>( cout, " " ) );
     cout << endl;

     return 1;
}
