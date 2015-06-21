//
// MSET05.CPP
//
// This program is used in Chapter 8 to demonstrate two
// of the lookup functions that are used with multiset<>.
// I create a set containing many duplicate entries, then
// look up a single entry, then a range of entries.
//

// Borland specific hacks
#define __MINMAX_DEFINED
#pragma option -vi-

#include <iostream.h>
#include <cstring.h>

#include "multiset.h"

struct product {
    string Company;
    string Name;
    product(char *company, char *name )
        : Company( company ),
          Name( name ){}
    bool operator<( const product& a ) const {
        return Company < a.Company;
    }
};

typedef multiset< product, less<product> > Products;
typedef pair< Products::iterator, Products::iterator > ProductsRange;

main()
{
    Products p;

    p.insert( product( "Microsoft", "Visual C++" ) );
    p.insert( product( "Borland", "Borland C++" ) );
    p.insert( product( "Microsoft", "Word for Windows" ) );
    p.insert( product( "Borland", "dBase" ) );
    p.insert( product( "Symantec", "Norton Desktop" ) );
    p.insert( product( "AT&T", "Plan 9" ) );
    p.insert( product( "Microsoft", "Excel" ) );
    p.insert( product( "Microsoft", "Access" ) );
    cout << "Set size = " << p.size() << endl;
    Products::iterator i;
    i = p.lower_bound( product( "Microsoft", "" ) );
    cout << "First entry matching 'Microsoft' = "
         << (*i).Company
         << ", "
         << (*i).Name
         << endl;
    i = p.lower_bound( product( "Vaporware, Inc.", "" ) );
    cout << "First entry matching 'Vaporware, Inc.' = ";
    if ( i == p.end() )
        cout << "None" << endl;
    else
        cout << (*i).Company
             << ", "
             << (*i).Name
             << endl;
    ProductsRange result = p.equal_range( product( "Microsoft", "" ) );
    for ( i = result.first ; i != result.second; i++ )
        cout << (*i).Company
             << ", "
             << (*i).Name
             << endl;
    return 1;
}
