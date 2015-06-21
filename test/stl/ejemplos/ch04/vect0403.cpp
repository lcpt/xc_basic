//
// VECT0403.CPP
//
// This example program is used in Chapter 4 to test
// some of the features of vector<T>.  It creates
// a vector<T> object with some predefined objects,
// then performs a single insertion, which will force
// a reallocation of memory.

//
// Borland 4.x workarounds.
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <vector.h>
#include <iostream.h>
#include <iomanip.h>

template<class T>
void dump_array( char *title, vector<T> &data );

main()
{
    char init[] = "ABDE";
    vector<char> a( init, init + 4 );

    dump_array( "At creation", a );
    a.insert( a.begin() + 2, 'c' );
    dump_array( "After insertion", a );
    a.pop_back();
    dump_array( "After pop_back", a );
    a.erase( a.begin() + 1, a.end() - 1 );
    dump_array( "After erase", a );
    return 1;
}

//
// This routine is called to print out the contents of
// a vector.  It was created as a template function so
// that it would be easy to test different types of
// vectors without a rewrite.
//

template<class T>
void dump_array( char *title, vector<T> &data )
{
    static int first_time = 1;
    if ( first_time ) {
        first_time = 0;
        cout << "         Event          Begin    End    "
             << "Size  Capacity          Data\n"
             << "---------------------  ------- -------  "
             << "----- --------  --------------------\n";
    };
    cout << setw(21) << title
         << "  " << (void *) data.begin()
         << "  " << (void *) data.end()
         << "  " << setw( 6 ) << data.size()
         << "  " << setw( 6 ) << data.capacity()
         << "  ";
    for ( vector<T>::iterator i = data.begin();
          i != data.end();
          i++ )
        cout << setw( 4 ) << *i;
    cout << endl;
}


