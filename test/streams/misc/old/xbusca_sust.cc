//xbusca_sust.cc

#include <fstream.h>
#include "../prep.h"
#include "sistema/Aplicacion.h"
#include "sistema/Sistema.h"

int main(int argc,char **argv)
  {
    Aplicacion app(argc,argv,"i:o:f:");
    const String str_entrada= app.GetOpc("i");
    const String str_salida= app.GetOpc("o");
    const String str_archivos= app.GetOpc("f");
    cout << str_archivos << endl;
    if(str_archivos == "")
      {
        cerr << "Sustituyendo: " << '"' << str_entrada << '"'
             << " por: " << '"' << str_salida << '"' << endl;
        busca_y_sust(cin,str_entrada,cout,str_salida);
      }
    else
      {
        cerr << "Procesando archivos de: " << str_archivos << endl;
        cerr << "Para sustituir: " << '"' << str_entrada << '"'
             << " por: " << '"' << str_salida << '"' << endl;
        ifstream archivos(str_archivos);
        String pth_archivo_entrada;
        String pth_archivo_backup;
        while(archivos.ipfx())
          {
            archivos >> get_token(pth_archivo_entrada,'\n');
            pth_archivo_backup= pth_archivo_entrada + '~';
            cout << "Procesando: " << pth_archivo_entrada << endl;
	    Sistema::Copy(pth_archivo_entrada,pth_archivo_backup);
            ifstream arch_entrada(pth_archivo_backup);
            ofstream arch_salida(pth_archivo_entrada);
	    busca_y_sust(arch_entrada,str_entrada,arch_salida,str_salida);
            arch_salida.close();
            arch_entrada.close();
          }
        archivos.close();
      }
  }
