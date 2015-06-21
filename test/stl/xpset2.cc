//xpset.cpp
#include "../src/pset.h"
#include "../src/pdeque.h"
#include <iostream>

//Intento FALLIDO de obtener el indices [i] del pdeque correspondiente
//a un puntero mediante el uso de un pset

int main(void)
  {
    std::string val[]= {"xdeque","xlist2","xlist","xlist_int","xmap","xmap_key","xpdeque","xplist","xpoli_deque","xpset","xpuntero","xpvector","xset_key","xvector1","xvector2","xvector3","xvector_char","xvector_double","xvector_int"};

    pdeque<std::string> dqs;
    for(size_t i=0;i< sizeof(val)/sizeof(val[0]);i++)
      dqs.push_back(&val[i]);
    for(pdeque<std::string>::const_iterator i=dqs.begin();i!=dqs.end();i++)
      std::cout << "str= " << **i << " orden: " << (i-dqs.begin()) << std::endl;
    pset<std::string> set_s;
    for(pdeque<std::string>::const_iterator i=dqs.begin();i!=dqs.end();i++)
      set_s.insert(&(**i));
    // FALLA ESTO:
//     pset<std::string>::const_iterator j= set_s.find(&val[8]);
//     pset<std::string>::const_iterator j0= set_s.begin();
//     std::cout << "orden: " << (j-j0) << std::endl;
    return 0;
  }
