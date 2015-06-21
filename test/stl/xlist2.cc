//xlist2
//Ejemplo de uso de list
#include <iostream>
#include <list>
#include <iterator>

void muestra_lista(std::list<char *> l,char *nombre)
  {
    std::cout << "Lista " << nombre << ": ";
    copy(l.begin(),l.end(),std::ostream_iterator<char *>(std::cout,","));
    std::cout << " tamaño= "<< l.size() << std::endl;
  }

int main(void)
  {
    std::list<char *> l;

    l.insert(l.end(),"Madrid");
    l.insert(l.end(),"Barcelona");
    l.insert(l.end(),"Bilbao");
    l.insert(l.end(),"Valencia");
    muestra_lista(l,"l");
    return 1;
  }
