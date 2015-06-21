//xlist_int

#include <iostream>
#include <list>
#include <iterator>

void muestra_lista(std::list<int> l,char *nombre)
  {
    std::cout << "Lista " << nombre << ": ";
    copy(l.begin(),l.end(),std::ostream_iterator<int>(std::cout,","));
    std::cout << " tamaño= "<< l.size() << std::endl;
  }

int main(void)
  {
    std::list<int> l;

    l.insert(l.end(),1);
    l.insert(l.end(),2);
    l.insert(l.end(),3);
    l.insert(l.end(),4);
    muestra_lista(l,"l");
    return 1;
  }
