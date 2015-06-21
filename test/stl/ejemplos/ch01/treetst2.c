/*
 * TREETST2.C
 *
 * This program tests the binary tree interface package.  It
 * does so by adding a list of integers to the tree, then
 * printing out the tree contents in order.  Compile this
 * program using a command line similar to one of these:
 *
 *  bcc -w TREETST2.C TREE1.C
 *
 *  cl /W4 TREETST2.C TREE1.C
 *
 */

#include <stdio.h>
#include "tree1.h"

/*
 * The numbers to be added to the tree.
 */

int numbers[] = { 5, 83, -40, -100, 88, 3, 3, -1 };

/*
 * The comparison function used by the routine that adds nodes
 * to the tree.
 */

int intcmp( int *a, int *b )
{
    return *a - *b;
}

/*
 * This is the function called by the tree iteration routine.
 * It simply prints the contens of each node.
 */

void intprn( const int *p )
{
    printf( "%d ", *p );
}

/*
 * This short program adds a set of integers to a generic binary
 * tree.  Note that we have to define both a comparion function
 * for the call to add_node(), and an output function for the call
 * to traverse().
 */

main()
{
    BINARY_TREE *root;
    int i;

    root = create_new_node( numbers );
    for ( i = 1; i < sizeof( numbers ) / sizeof( int ) ; i++ )
        add_node( root, numbers + i, (COMPARE_FN *) intcmp );
    traverse( root, (ITERATE_FN*) intprn );
    return 0;
}

