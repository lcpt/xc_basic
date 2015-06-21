/*
 * TREETST3.C
 *
 * This program replaces binary tree interface package with
 * a handcoded alternative.  In this program, the integers
 * are stored in the tree nodes, comparisons are done inline,
 * and printout is done inside the traversal routine.
 * Compile this program using a command line similar to one
 * of these:
 *
 *  bcc -w TREETST3.C
 *
 *  cl /W4 TREETST3.C
 *
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * This node data structure holds the integer directly,
 * instead of using a void *.
 */

typedef struct _tag_tree {
    int data;
    struct _tag_tree *left_node;
    struct _tag_tree *right_node;
} BINARY_TREE;

/*
 * Initialize a node, and store the data simulatneously.
 */

BINARY_TREE *create_new_node( int i )
{
    BINARY_TREE *node = malloc( sizeof( BINARY_TREE ) );
    node->left_node = 0;
    node->right_node = 0;
    node->data = i;
    return node;
}

/*
 * The routine to add a node just walks down the tree until
 * we find an empty node that the new integer can fit into.
 */

void add_node( BINARY_TREE *root, int data )
{
    BINARY_TREE *node = root;
    BINARY_TREE *new_node = create_new_node( data );
    for ( ; ; ) {
        BINARY_TREE **next_node_ptr;
        if ( data < node->data )
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
 * The traversal routine is just used to print out the
 * data in the tree.
 */

void traverse( BINARY_TREE *root )
{
    if ( root == 0 )
        return;
    traverse( root->left_node );
    printf( "%d ", root->data );
    traverse( root->right_node );
}

/*
 * The numbers to be added to the tree.
 */

int numbers[] = { 5, 83, -40, -100, 88, 3, 3, -1 };

/*
 * This short program adds a set of integers to a customized
 * tree.  It should be much more efficient than the generic
 * equivalent.
 */

main()
{
    BINARY_TREE *root;
    int i;

    root = create_new_node( numbers[ 0 ] );
    for ( i = 1; i < sizeof( numbers ) / sizeof( int ) ; i++ )
        add_node( root, numbers[ i ] );
    traverse( root );
    return 0;
}

