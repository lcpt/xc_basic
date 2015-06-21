/*
 *  TREE1.H
 *
 *  This file defines the public interface for a generic
 *  binary tree data structure.  This is a relatively simple
 *  interface, with just three functions.
 *
 */

typedef struct _tag_tree {
    void *data;
    struct _tag_tree *left_node;
    struct _tag_tree *right_node;
} BINARY_TREE;

/*
 * This typedef defines the comparison function.  It
 * should return a negative number if the first arg
 * is less than the second arg.  The add_node() routine
 * uses this function to add data to the tree.
 */
typedef int (COMPARE_FN)( void *, void * );
typedef void (ITERATE_FN)( void * );

BINARY_TREE *create_new_node( void *p );
void add_node( BINARY_TREE *root, void *p, COMPARE_FN *cmp );
void traverse( BINARY_TREE *root, ITERATE_FN *iter_fn );
