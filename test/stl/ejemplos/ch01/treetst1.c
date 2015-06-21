/*
 * TREETST1.C
 *
 * This program tests the binary tree interface package.  It
 * does so by adding a list of strings to the tree, then
 * printing out the tree contents in order.  Compile this
 * program using a command line similar to one of these:
 *
 *  bcc -w TREETST1.C TREE1.C
 *
 *  cl /W4 TREETST1.C TREE1.C
 *
 */

#include <stdio.h>
#include <string.h>
#include "tree1.h"

/*
 * The strings to be added to the tree.
 */

char *strings[] = {
    "Arvid",
    "Thirl",
    "Loki",
    "Athena",
    "Nimrod",
    "Zima",
    "Kirin",
    0
};

/*
 * This is the function called by the tree iteration routine.
 * It simply prints the string value stored in the node.
 */

void strprn( const char *s )
{
    printf( "%s ", s );
}

/*
 * This short program adds a set of strings to a generic binary
 * tree.  Note that we don't have to define a comparsion function
 * for the call to add_node(), because the RTL function strcmp()
 * does the job without modifications.  After the strings have
 * been added, we print the contents of the tree out in order.
 */

main()
{
    BINARY_TREE *root;
    char **p;

    root = create_new_node( strings[ 0 ] );
    for ( p = strings + 1; *p != 0 ; p++ )
        add_node( root, *p, (COMPARE_FN *) strcmp );
    traverse( root, (ITERATE_FN *) strprn );
    return 0;
}

