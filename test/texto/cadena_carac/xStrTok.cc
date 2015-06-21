#include <iostream>
#include "xc_basic/src/texto/StrTok.h"

void prueba_StrTok(const std::string &dato,const char &sep,std::ostream &os)
  {
    StrTok strtk1(dato);
    os << "Dato: " << strtk1 << std::endl;
    os << "Carácter separador: " << sep << std::endl;
    StrTok::dq_campos campos= strtk1.campos(sep);
    os << "Campos obtenidos:" << std::endl;
    size_t conta= 1;
    for(StrTok::dq_campos::const_iterator i=campos.begin();i!=campos.end();i++,conta++)
      os << "Campo " << conta << ": " << *i << std::endl;
  }

int main(void)
  {
    const char sep= '|';
    std::string str1("|CONDUCCION BY-PASS DE ROYO FRIO.|702441684.8577|040400|0");
    prueba_StrTok(str1,sep,std::cout);
    std::cout << std::endl;
    std::string str2("***|Unidad sin descompuesto|1|040400|1");
    prueba_StrTok(str2,sep,std::cout);
    return 0;
  }
