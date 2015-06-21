#include "xc_basic/src/texto/LabelContainer.h"
#include <iostream>

int main(void)
  {
    LabelContainer c,d;
    std::cout << "Núm etiquetas: " << c.getNumEtiquetas() << std::endl;
    c.addEtiqueta("hola");
    c.addEtiqueta("hola");
    c.addEtiqueta("adios");
    std::cout << "Núm etiquetas: " << c.getNumEtiquetas() << std::endl;
    std::cout << "c= " << c << std::endl;
    d.addEtiqueta("hola");
    d.addEtiqueta("hello");
    d.addEtiqueta("bye");
    d.addEtiqueta("hi!");
    std::cout << "Núm etiquetas: " << d.getNumEtiquetas() << std::endl;
    std::cout << "d= " << d << std::endl;
    std::cout << "d.hasEtiqueta(hi!)= " << d.hasEtiqueta("hi!") << std::endl;
    d.removeEtiqueta("hi!");
    std::cout << "Núm etiquetas: " << d.getNumEtiquetas() << std::endl;
    std::cout << "d= " << d << std::endl;
    std::cout << "d.hasEtiqueta(hello)= " << d.hasEtiqueta("hello") << std::endl;
    std::cout << "d.hasEtiqueta(hi!)= " << d.hasEtiqueta("hi!") << std::endl;
    std::cout << "Núm total etiquetas: " << c.getDiccionario().getNumEtiquetas() << std::endl;
    std::cout << "total etiquetas: " << c.getDiccionario() << std::endl;
    return 0;
  }
