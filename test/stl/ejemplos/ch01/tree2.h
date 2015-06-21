/*
 *  TREE2.H
 *
 *  This file defines the base class used to develop binary
 *  trees.  To make effective use of the trees, you must
 *  derive a new class from this type.  The derived class
 *  should define the data members to be stored in the tree,
 *  as well as the two pure virtual functions from this class.
 *
 */

class BinaryTree {
    protected :
        BinaryTree *left;
        BinaryTree *right;
    public :
        BinaryTree(){ left = 0; right = 0; }
        void AddNode( BinaryTree *new_node );
        void Traverse();
        virtual int Compare( BinaryTree * ) = 0;
        virtual void Display() = 0;
};


