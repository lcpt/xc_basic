/*
 * IN1802.CPP
 *
 * This program is used in Chapter 18 to demonstrate
 * the inner_product algorithm.  It uses function
 * objects to perform an operation that is somewhat
 * different from the default version of inner_product.
 * The default inner product function will summ the
 * products of the two ranges, something like this:
 *
 *  while ( first1 != last1 )
 *      sum = sum + *first1++ * *first2++;
 *
 * This program uses function objects to turn that on
 * its head.  It instead implements:
 *
 *  while ( first1 != last1 )
 *      sum = sum = *first1++ / *first2++;
 */

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>
#include <algo.h>

double s1[] = { 1, 2, 3, 4, 5 };
double s2[] = { 5, 4, 3, 2, 1 };

int main()
{
    double d = inner_product( s1,
                              s1 + 5,
                              s2,
                              0.0,
                              minus<double>(),
                              divides<double>() );
    cout << "inner_product = "
         << d
         << endl;
    return 0;
}

