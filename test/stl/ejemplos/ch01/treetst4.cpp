//
// TREETST4.CPP
//
// This program tests class BinaryTree. It does so by deriving
// class IntTree, which should represent a binary tree holding
// integers as its data types.  It then adds a list of integers
// to the tree, and finally prints out the integers in order.
// The derived class has just four functions defined.  The
// constructor creates a new node and stores data in it, and
// a casting operator is used to get the data out of a node.
// The virtual functions Compare() and Display() are used by
// the base class to manipulate the tree.
//
//  bcc -w TREETST4.CPP TREE2.CPP
//
//  cl /W4 TREETST4.CPP TREE2.CPP
//
//

#include <iostream.h>
#include <string.h>

#include <iostream.h>
#include "tree2.h"

//
// This is the definition of the derived class.  It
// is used to store integers in a binary tree.  The
// base class knows how to insert items into the tree,
// and the derive class knows how to compare and display
// nodes.  It takes both of them together to produce a
// useful code.
//
class IntTree : public BinaryTree {
    protected :
        int data;
    public :
        IntTree( int i  ){ data = i; }
        operator int(){ return data; }
        virtual int Compare( BinaryTree * );
        virtual void Display(){ cout << data << ' '; }
};

//
// During the insertion process, the base class
// has to compare nodes in order to determine whether to
// turn right or left from a node under test.
// This function takes care of the comparison.
//
int IntTree::Compare( BinaryTree *other_node )
{
    return data - ( int ) other_node ;
}

//
// The ints to be added to the tree.
//

int numbers[] = { 10, 15, -5, 2, 1, 100 };

//
// This short program adds a set of integers to a generic binary
// tree.  After the integers have been added, we print the
// contents of the tree out in order.
//

main()
{
    IntTree *root = new IntTree( numbers[ 0 ] );
    for ( int i = 1; i < sizeof( numbers ) / sizeof( int ) ; i++ )
        root->AddNode( new IntTree( numbers[ i ] ) );
    root->Traverse();
    return 0;
}
