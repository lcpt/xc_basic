#include <iostream>
#include "xc_basic/src/texto/FixedWidthReader.h"


int main(void)
  {
    FixedWidthReader reader(9);
    reader.set_field_pair(0,0,8);
    reader.set_field_pair(1,9,20);
    reader.set_field_pair(2,21,32);
    reader.set_field_pair(3,33,44);
    reader.set_field_pair(4,45,56);
    reader.set_field_pair(5,57,68);
    reader.set_field_pair(6,69,80);
    reader.set_field_pair(7,81,92);
    reader.set_field_pair(8,93,104);
    reader.set_string("       1  0.26409E-13 0.16650E-12-0.29961E-13-0.25381     0.76536    -0.26848     -1.3153     0.99011    ");
    size_t conta= 0;
    for(FixedWidthReader::const_iterator i=reader.campos_begin();i!=reader.campos_end();i++,conta++)
      std::cout << "campo[" << conta << "]= " << *i << std::endl;
    std::cout << reader.get_csv_string() << std::endl;
    std::cout << reader.get_indices_string() << std::endl;
    return 0;
  }
