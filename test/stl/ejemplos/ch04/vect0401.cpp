//
// VECT0401.CPP
//
// This example program is used in Chapter 4 to test
// some of the features of vector<T>.  It creates
// a vector<T> object, then performs some insertions
// and erasures, dumping the contents of the vector
// after each step.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <vector.h>
#include <iostream.h>
#include <iomanip.h>

template<class T>
void dump_array( char *title, vector<T> &data );

//
// This test program creates a vector of doubles.  It
// inserts three doubles using the conventional
// vector<T>::push_back() function.  It then modifies one
// of the saved doubles using operator[], erases the
// same object, then inserts a new object at the start
// of the vector.
//
main()
{
    vector<double> a;

    dump_array( "At creation", a );
    a.push_back( 1.0 );
    dump_array( "Added 1 object", a );
    a.push_back( 2.0 );
    a.push_back( 3.0 );
    dump_array( "Added 2 more objects", a );
    a[ 1 ] = 2.5;
    dump_array( "Modified a[1]", a );
    a.erase( &a[ 1 ] );
    dump_array( "Erased a[1]", a );
    a.insert( a.begin(), 0.5 );
    dump_array( "Inserted at a[0]", a );
    a.insert( a.begin() + 2, 2.6 );
    dump_array( "Inserted at a[2]", a );
    while ( a.size() > 0 ) {
        a.pop_back();
        dump_array( "After pop_back()", a );
    }
    cout << "\n";
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


