//
// HUGE0904.CPP
//
// This program is used in Chapter 9 to demonstrate
// the steps you need to take to use a non-default
// allocator class for a given container.
//
// This program defines Allocator to be huge_allocator
// *before* including vector.h.  This means that vector.h
// will use the huge allocator instead of the default
// allocator.  vector.h automatically undefines Allocator,
// which means the next include of list.h will use the
// default allocator.
//

//
// Borland 4.x workarounds
//
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>

#include <hugalloc.h>
#define Allocator huge_allocator
#include <vector.h>

#include <list.h>

main()
{
    vector<int> a;
    cout << "a.max_size() = "
         << a.max_size()
         << endl;

    list<int> b;
    cout << "b.max_size() = "
         << b.max_size()
         << endl;
    return 1;
}

