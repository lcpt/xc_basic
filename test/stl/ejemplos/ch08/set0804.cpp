//
// SET0804.CPP
//
// This program is used in Chapter 8 to demonstrate the
// use of set<> with user defined data types.  I create
// a user defined type, class employee.  I then have
// to create a comparison class that can be used to
// order objects of this type.
//


#include <iostream.h>
#include <cstring.h>
#include "set.h"

class employee {
    public :
        string name;
        long id_number;
        employee( char *n, long id )
          :  name( n ), id_number( id ){}
};

class less_employee {
    public :
    bool operator()(const employee& x, const employee& y) const
    {
        return x.name < y.name;
    }
};

main()
{
    set<employee,less_employee > a;

    a.insert( employee( "Bill Gates", 123 ) );
    a.insert( employee( "Phillipe Kahn", 456 ) );
    a.insert( employee( "Bill Clinton", 789 ) );
    set<employee, less_employee>::iterator i;
    i = a.find( employee( "Bill Gates", 0 ) );
    if ( i == a.end() )
        cout << "Bill Gates not found!" << endl;
    else
        cout << "find() returned: "
             << (*i).name
             << ", "
             << (*i).id_number
             << endl;
    return 0;
}
