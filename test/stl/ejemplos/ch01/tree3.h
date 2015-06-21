/*
 *  TREE3.H
 *
 *  This header file contains the template class definition
 *  for class BinaryTree<T>.  Class T is the single parameter
 *  used in the class definition.  Note that this declaration
 *  looks quite a bit like that in TREE2.H.  One major difference
 *  is that the header file contains all of the code defined for
 *  the member functions in addition to the declaration.
 *
 */

#include <iostream.h>
#include <string.h>

template <class T> class BinaryTree
{
    public :
        BinaryTree<T>( const T &t );
        void AddNode( const T & );
        void Traverse();
    protected :
        T data;
        BinaryTree<T> *left_node;
        BinaryTree<T> *right_node;
        int LessThan( const T &t );
};


//
// The LessThan function is used when inserting new
// data into the tree.  Normally this will expand
// into a simple inline comparison, making this
// very efficient for built-in types.  This class defines
// a slightly different version for string comparisons,
// as we normally don't wnat to compare them just by
// pointer values.
//
template <class T>
inline int BinaryTree<T>::LessThan( const T &t )
{
    return data < t;
}

typedef char *X;

inline int BinaryTree<char *>::LessThan( const X &t )
{
    return strcmp( data, t ) < 0;
}

template <class T>
inline BinaryTree<T>::BinaryTree( const T &t )
{
    data = t;
    left_node = 0;
    right_node = 0;
}

//
// Unlike the traversal function in TREE2.CPP, this function
// doesn't have to call a virtual function to display the
// data.
//

template <class T> void BinaryTree<T>::Traverse()
{
    if ( left_node )
        left_node->Traverse();
    cout << data << " ";
    if ( right_node )
        right_node->Traverse();
}

//
// The AddNode() member function is nearly identical to that
// in TREE2.CPP.  In this case however, the comparison between
// the two data items is called using the LessThan() member
// function.  LessThan() should normally expand to an inline
// function performing a direct comparison.  For built-in
// data types this should be very efficient.
//
template <class T> void BinaryTree<T>::AddNode( const T &data )
{
    BinaryTree<T> *node = this;
    BinaryTree<T> *new_node = new BinaryTree<T>( data );
    for ( ; ; ) {
        BinaryTree<T> **next_node_ptr;
        if ( node->LessThan( data )  )
            next_node_ptr = &node->right_node;
        else
            next_node_ptr = &node->left_node;
        if ( *next_node_ptr == 0 ) {
            *next_node_ptr = new_node;
            return;
        } else
            node = *next_node_ptr;
    }
}


