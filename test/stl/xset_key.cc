//xset_key.cc

#include "../src/set_key.h"
#include <string>
#include <iostream>

struct prueba
  {
    size_t indice;
    std::string str;

    typedef size_t tipo_indice;
  
    prueba(const size_t &i,const std::string &s="")
      : indice(i), str(s) {}
    const size_t &Indice(void) const
      { return indice; }
  };

std::ostream &operator<<(std::ostream &os,const prueba &p)
  {
    os << p.indice << ' ' << p.str;
    return os;
  } 



int main(void)
  {

    set_key<prueba> set;

    set.insert(prueba(1,"hola"));
    set.insert(prueba(2,"adiós"));

    for(set_key<prueba>::const_iterator i= set.begin();i!=set.end();i++)
      std::cout << *i << std::endl;

    std::cout << "cambia:" << std::endl;
    set.insert(prueba(1,"holo"));
    set.insert(prueba(3,"hl"));

    for(set_key<prueba>::const_iterator i= set.begin();i!=set.end();i++)
      std::cout << *i << std::endl;

    std::cout << "busca:" << std::endl;
    std::cout << *(set.busca(2)) << std::endl;
    std::cout << *(set.busca(1)) << std::endl;

    std::cout << "operador[]:" << std::endl;
    std::cout << set[1] << std::endl;
    std::cout << set[3] << std::endl;
    
    return 0;
  }
