/*
 *  TREE2.CPP
 *
 *  This file contains the code that implements the binary
 *  tree generic base class defined in TREE2.H.
 *
 */

#include "tree2.h"

/*
 * The routine to add a node to the tree simply works its way
 * down the branches of the tree until it finds an open leaf
 * position where it can add the new node.  To determine which
 * path to take down the tree requires the use of the Compare()
 * virtual function, which is defined in the derived class.
 */

void BinaryTree::AddNode( BinaryTree *new_node )
{
    BinaryTree *node = this;
    for ( ; ; ) {
        BinaryTree **next_node_ptr;
        if ( node->Compare( new_node ) < 0 )
            next_node_ptr = &node->left;
        else
            next_node_ptr = &node->right;
        if ( *next_node_ptr == 0 ) {
            *next_node_ptr = new_node;
            return;
        } else
            node = *next_node_ptr;
    }
}

/*
 * When traversing the tree we call the virtual function
 * Display().  This function is defined in the base class.
 */

void BinaryTree::Traverse()
{
    if ( left )
        left->Traverse();
    Display();
    if ( right )
        right->Traverse();
}


