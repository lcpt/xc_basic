/*
 *  TREE1.C
 *
 *  This file contains the code that implements the binary
 *  tree generic data structure package defined in TREE1.H.
 *
 */

#include <stdlib.h>
#include "tree1.h"

/*
 * Initialize the root node.  After allocating the memory,
 * the left an right pointers are set to null, and the
 * data pointer is initialized to the value passed in.
 */

BINARY_TREE *create_new_node( void * p )
{
    BINARY_TREE *node = malloc( sizeof( BINARY_TREE ) );
    node->data = p;
    node->left_node = 0;
    node->right_node = 0;
    return node;
}

/*
 * The routine to add a node just walks down the tree until we
 * find an empty node that the new data can fit into.  Comparisons
 * are done using the pointer to a function passed in.
 */

void add_node( BINARY_TREE *root, void *p, COMPARE_FN *cmp )
{
    BINARY_TREE *node = root;
    BINARY_TREE *new_node = create_new_node( p );
    for ( ; ; ) {
        BINARY_TREE **next_node_ptr;
        if ( cmp( p, node->data ) < 0 )
            next_node_ptr = &node->left_node;
        else
            next_node_ptr = &node->right_node;
        if ( *next_node_ptr == 0 ) {
            *next_node_ptr = new_node;
            return;
        } else
            node = *next_node_ptr;
    }
}

/*
 * The traversal function calls the user supplied
 * function via a pointer.
 */
void traverse( BINARY_TREE *root, ITERATE_FN *iter_fn )
{
    if ( root == 0 )
        return;
    traverse( root->left_node, iter_fn );
    iter_fn( root->data );
    traverse( root->right_node, iter_fn );
}

