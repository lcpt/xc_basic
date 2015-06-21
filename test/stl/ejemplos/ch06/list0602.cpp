//
// LIST0602.CPP
//
// This example program is used in Chapter 6 to demonstrate
// the internals of the memory allocation system used in
// list<T>.  It creates a couple of list<double> containers,
// and dumps their memory statistics after various operations.
//

//
// Borland 4.x workarounds.
//
#define __MINMAX_DEFINED
#include <list.h>

//
// Note that list<T> doesn't allow access to the
// data members that control memory allocation.
// So that we can get at them, I derive a new class
// here that has some public functions that let
// us look at buffer pools and the free list.
//
template<class T>
class public_list : public list<T> {
    public :
        public_list() : list<T>() {}
        size_t buffer_pool_free();
        size_t buffer_pool_total();
        size_t buffer_size(){ return list<T>::buffer_size(); }
        long free_count();
        list<T>::link_type get_free_list(){ return free_list; }
};

//
// This function is used to calculate the total number
// of nodes in the buffer pool.  It simply iterates
// through the list of buffers, adding to the total
// count of nodes along the way.  Note that these
// nodes may be in use or may be in the free list, we
// don't know at this point.
//
template<class T>
size_t public_list<T>::buffer_pool_total()
{
    size_t total = 0;
    buffer_pointer list = buffer_list;
    while ( list ) {
        total += buffer_size();
        list = (buffer_pointer)( list->next_buffer );
    }
    return total;
}

//
// This function returns the total number of nodes
// available for immediate allocation from the buffer
// pool.
//
template<class T>
size_t public_list<T>::buffer_pool_free()
{
    if ( next_avail == 0 || next_avail == last )
        return 0;
    else
        return last - next_avail;
}

//
// This function returns the total number of nodes
// in the free list.  We do it the hard way, by
// counting every single one.
//
template<class T>
long public_list<T>::free_count()
{
    long count = 0;
    link_type free_guy = free_list;
    while ( free_guy ) {
        count++;
        free_guy = (link_type) free_guy->next;
    }
    return count;
}

//
// This function dumps out the memory stats for
// a given list<T> object.  The caller gets to
// add a short message to the start of the printout.
//
template<class T>
void dump_memory( char *message, public_list<T> &a )
{
    cout << message << " ";
    cout << "size= " << a.size() << " ";
    cout << "buffer pool (total/free)= "
         << a.buffer_pool_total()
         << "/"
         << a.buffer_pool_free()
         << "  free_count= "
         << a.free_count()
         << endl;
}

//
// This program prints out the memory stats for
// the list<double> class at several points during
// the lifetime of the objects.
//
main()
{
    public_list<double> a;
    public_list<double> b;
    public_list<char> c;

    dump_memory( "a, constructed  ", a );
    a.push_front( -2 );
    dump_memory( "a.push_front()  ", a );
    a.push_back( -3 );
    dump_memory( "a.push_back()   ", a );
    a.pop_back();
    dump_memory( "a.pop_back()    ", a );
    a.push_front( -4 );
    dump_memory( "a.push_front()  ", a );
    for ( long i = 0 ; i < 5000 ; i++ ) {
        if ( !( i % 1024 ) )
            cout << '\r' << i;
        a.insert( a.begin(), i );
    }
    cout << '\r';
    dump_memory( "a.insert()*5000 ", a );
    a.erase( a.begin(), a.end() );
    dump_memory( "a.erase()       ", a );
    dump_memory( "b, constructed  ", b );
    dump_memory( "c, constructed  ", c );
    return 1;
}

