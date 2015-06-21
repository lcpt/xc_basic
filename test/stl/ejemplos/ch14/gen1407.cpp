/*
 * GEN1407.CPP
 *
 * This program is used in Chapter 14 to demonstrate
 * the generate algorithm.  It fills the first 20 elements of
 * an integer array with the first 20 fibonacci numbers.
 * Remember that f(1) = 1, f(2) = 1, and
 * f(n) = f(n-1) + f(n-2).
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <algo.h>

struct fibber {
    int n_minus_1;
    int n_minus_2;
    fibber() {
        n_minus_1 = 0;
        n_minus_2 = 1;
    }
    int operator()() {
        int ret_val = n_minus_1 + n_minus_2;
        n_minus_2 = n_minus_1;
        n_minus_1 = ret_val;
        return ret_val;
    }
};

main()
{
    int fibs[ 20 ];
    generate_n( fibs, 20, fibber() );
    copy( fibs, fibs + 20 , ostream_iterator<int>( cout, " " ) );
    return 1;
}
