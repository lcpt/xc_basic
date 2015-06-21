//Primera prueba de la biblioteca STL;

#include <list>
#include <algorithm>
#include <iostream>
#include <iterator>

void muestra_lista(std::list<int> l,char *nombre)
  {
    std::cout << "Lista " << nombre << ": ";
    copy(l.begin(),l.end(),std::ostream_iterator<int>(std::cout,","));
    std::cout << " tamaño= "<< l.size() << std::endl;
  }
void suma2(int &i)
  {
    i++;i++;
  }
bool es3(int i)
  {
    return i == 3;
  }
bool es5(int i)
  {
    return i == 5;
  }
int main(void)
  {
    std::list<int> a(5);
    std::list<int> b;
    b.push_back(2);
    b.push_front(1);
    b.push_back(3);
    muestra_lista(a,"a");
    muestra_lista(b,"b");
    std::cout << "Efecto de a= b" << std::endl;
    a= b;
    muestra_lista(a,"a");
    muestra_lista(b,"b");
    std::cout << "Elemento inicial: " << *b.begin() << std::endl;
    std::cout << "Elemento final: " << *--b.end() << std::endl;
    std::cout << "Efecto de insert: " << std::endl;
    std::list<int>::iterator iterador= b.begin();
    b.insert(++iterador,7);
    muestra_lista(b,"b");
    std::cout << "Efecto de reverse: " << std::endl;
    b.reverse();
    muestra_lista(b,"b");
    std::cout << "Efecto de sort: " << std::endl;
    b.sort();
    muestra_lista(b,"b");
    std::cout << "*max_element: " << *max_element(b.begin(),b.end()) << std::endl;
    std::cout << "Efecto de fill_n: " << std::endl;
    fill_n(++b.begin(),b.size(),3);
    muestra_lista(b,"b");    
    std::cout << "Efecto de iter_swap: " << std::endl;
    iter_swap(b.begin(),--b.end());
    muestra_lista(b,"b");
    std::cout << "Efecto de for_each: " << std::endl;
    for_each(b.begin(),b.end(),suma2);
    muestra_lista(b,"b");
    std::cout << "Efecto de find_if: " << std::endl;
    std::list<int>::iterator i;
    i= find_if(b.begin(),b.end(),es3);
    std::cout << *i << std::endl;
    std::cout << "Efecto de count_if: " << std::endl;
//     long n;
//     count_if(b.begin(),b.end(),es5,n);
//     std::cout << n << std::endl;
//     std::cout << "Efecto de count: " << std::endl;
//     n= 0; //Inicializmos el contador;
//     count(b.begin(),b.end(),5,n);
//     std::cout << n << std::endl;
    return 0;
  } 
