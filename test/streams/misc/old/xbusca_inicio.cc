//xbusquedabruta.cc

#include "../imanip.h"

void main(void)
  {
    busca_inicio(cin,"modelo");
    while(cin.ipfx(0))
      cout << (char) cin.get();
    cout << endl;
  }
