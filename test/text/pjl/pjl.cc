//pjl.cc
#include <string>
#include <iostream>

const char ESC= 27;
const std::string UEL= "\033%-12345X";
const char crlf[]= {13,10,0};
const std::string CRLF= crlf; //char(13)+char(10);
const std::string PJL= "@PJL";
const std::string CABECERA_PCL= "\033E\033%0BIN;";

inline std::string linea_pjl(const std::string &str)
  { return PJL + ' ' + str + CRLF; }

inline std::string pjl_comentario(const std::string &str)
  { return linea_pjl("COMMENT " + str); }

inline std::string pjl_leng_impresion(const std::string &str)
  { return linea_pjl("ENTER LANGUAGE = " + str); }

inline std::string set_paper(const std::string &str)
  { return linea_pjl("SET PAPER = " + str); }
inline std::string set_orientation(const std::string &str)
  { return linea_pjl("SET ORIENTATION = " + str); }
inline std::string set_media_source(const std::string &str)
  { return linea_pjl("SET MEDIASOURCE = " + str); }
inline std::string set_resolution(const std::string &str)
  { return linea_pjl("SET RESOLUTION = " + str); }
inline std::string set_print_quality(const std::string &str)
  { return linea_pjl("SET PRINTQUALITY = " + str); }

void vuelca(std::istream &is,std::ostream &os)
  {
    char c;
    while(1)
      {
        is.get(c);
        if(is)
          os << c;
        else
          break;
      }
  }

int main(void)
  {
    std::cout << UEL << linea_pjl("");
    std::cout << pjl_comentario("Comienza trabajo HP-GL/2 ");
    std::cout << set_resolution("600 ");
    std::cout << set_paper("A3 ");
    std::cout << set_orientation("LANDSCAPE ");
    std::cout << set_media_source("TRAY3 ");
    std::cout << set_print_quality("HIGH ");
    std::cout << pjl_leng_impresion("PCL ");
    std::cout << CABECERA_PCL;
    vuelca(std::cin,std::cout);
    std::cout << UEL;
    return 0;
  }
