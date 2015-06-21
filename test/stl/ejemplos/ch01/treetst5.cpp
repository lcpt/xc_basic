//
// TREETST5.CPP
//
// This program tests class BinaryTree<T>. It does so by
// instantiating a BinarTree<int>, and another BinaryTree<char *>.
// It then performs the same operations done with the other
// test programs in this chapter, namely adding a list of data
// then displying it using the Traverse() function.
//
//  Build this program using command lines similar to this:
//
//  bcc -w TREETST5.CPP
//
//  cl /W4 TREETST5.CPP
//
//


#include "tree3.h"

int numbers[] = { 5, 12, 4, 4, -1, 10 };
char *strings[] = { "Burma", "Thailand", "Laos", "Viet Nam", "Cambodia" };

main()
{
    BinaryTree<int> tree1( numbers[ 0 ] );
    for ( int i = 1 ; i < sizeof( numbers ) / sizeof( int ) ; i++ )
        tree1.AddNode( numbers[ i ] );
    tree1.Traverse();
    BinaryTree<char *> tree2( strings[ 0 ] );
    for ( i = 1 ; i < sizeof( strings ) / sizeof( char * ) ; i++ )
        tree2.AddNode( strings[ i ] );
    tree2.Traverse();
    return 0;
}
