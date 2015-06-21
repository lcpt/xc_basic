/*
 * MIN1902.CPP
 *
 * This program is used in Chapter 19 to demonstrate the min_element
 * algorithm. This program selects the minimum integer from a
 * selection read from cin.
 *
 * One interesting thing to note is that the istream_iterator
 * can be used in this algorithm. The iterator returned from
 * min_element() points to a valid copy of the minimum value,
 * even though the input stream has presumably all been read in.
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <algo.h>


int main()
{
    cout << "Enter a sequence of integers,\n"
         << "terminated by an EOF: ";
    cout << flush;

    istream_iterator<int,ptrdiff_t> ii( cin );
    istream_iterator<int,ptrdiff_t> ieof;
    istream_iterator<int,ptrdiff_t> result;

    result = min_element( ii, ieof );
    if ( result == ieof )
        cout << "Empty input sequence!\n";
    else
        cout << "Minimum value = "
             << *result
             << endl;
    return 0;
}

