//xmap_key.cc

#include "../src/map_key.h"
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

    map_key<prueba> map;

    map.insert(prueba(1,"hola"));
    map.insert(prueba(2,"adiós"));

    for(map_key<prueba>::const_iterator i= map.begin();i!=map.end();i++)
      std::cout << (*i).second << std::endl;

    std::cout << "cambia:" << std::endl;
    map.insert(prueba(1,"holo"));
    map.insert(prueba(3,"hl"));

    for(map_key<prueba>::const_iterator i= map.begin();i!=map.end();i++)
      std::cout << (*i).second << std::endl;

    std::cout << "busca:" << std::endl;
    std::cout << (*map.busca(2)).second << std::endl;
    std::cout << (*map.busca(1)).second << std::endl;

    std::cout << "operador[]:" << std::endl;
    std::cout << map[1] << std::endl;
    std::cout << map[3] << std::endl;
    
    return 0;
  }
