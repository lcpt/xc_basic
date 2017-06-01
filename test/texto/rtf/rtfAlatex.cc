//rtfAlatex.cc

#include <iostream>
#include <fstream>
#include <cctype>
#include <string>
#include <stack>
#include "xc_basic/src/stl/pdeque.h"
#include "xc_basic/src/stl/poli_deque.h"

using namespace std;

class EntidadRTF
  {
  public:
    EntidadRTF(void){}
    virtual EntidadRTF *Copia(void) const= 0;
    virtual void Print(std::ostream &os) const= 0;
    virtual void PrintLatex(std::ostream &os) const= 0;
    virtual int Coincide(const string &str) const
      { return 0; }
    virtual ~EntidadRTF() {}
  };

class SimbCtrl:public EntidadRTF
  {
  protected:
    char cod;

  public:
    SimbCtrl(void): EntidadRTF() {}
    SimbCtrl(const SimbCtrl &otro): EntidadRTF(otro), cod(otro.cod) {}
    virtual EntidadRTF *Copia(void) const
      { return new SimbCtrl(*this); }
    virtual int Coincide (const string &str) const
      { return (cod == str[0]); }
    virtual void Print(std::ostream &os) const
      { os << "simbolo de control: " << cod; }
    virtual void PrintLatex(std::ostream &os) const
      {}
    friend istream &operator>>(istream &is,SimbCtrl &sc)
      {
        sc.cod= char(cin.get());
        return is;
      }
    friend std::ostream &operator<<(std::ostream &os,const SimbCtrl &sc)
      {
        sc.Print(os);
        return os;
      }
  };

class CaracterHex: public SimbCtrl
  {
  private:
    string arg;

  public:
    CaracterHex(void): SimbCtrl(), arg() {}
    CaracterHex(const CaracterHex &otro): SimbCtrl(otro), arg(otro.arg) {}
    virtual EntidadRTF *Copia(void) const
      { return new CaracterHex(*this); }
    virtual int Coincide (const string &str) const
      { return (cod == str[0]); }
    virtual void Print(std::ostream &os) const
      { os << "caracter hexadecimal: " << cod << std::endl
           << "  argumento: " << arg << std::endl; }
    virtual void PrintLatex(std::ostream &os) const
      {
        if(arg == "ba") os << 'º';
        if(arg == "e1") os << 'á';
        if(arg == "e9") os << 'é';
        if(arg == "ed") os << 'í';
        if(arg == "f1") os << 'ñ';
        if(arg == "f3") os << 'ó';
        if(arg == "fa") os << 'ú';
      }
    friend istream &operator>>(istream &is,CaracterHex &sc)
      {
        sc.cod= char(cin.get());
        sc.arg+= char(cin.get());
        sc.arg+= char(cin.get());
        return is;
      }
  };

class CadenaRTF: public EntidadRTF
  {
  protected:
    string nmb;

  public:
    CadenaRTF(void): EntidadRTF(), nmb() {}
    CadenaRTF(char c): EntidadRTF(), nmb()
      { nmb+= c; }
    CadenaRTF(const CadenaRTF &otro)
      : EntidadRTF(otro), nmb(otro.nmb) {}
    virtual EntidadRTF *Copia(void) const
      { return new CadenaRTF(*this); }
    const string &getName(void) const
      { return nmb; }
    inline virtual void Print(std::ostream &os) const
      { os << "cadena: " << getName(); }
    inline virtual void PrintLatex(std::ostream &os) const
      { os << getName(); }  
    friend istream &operator>>(istream &is,CadenaRTF &str)
      {
        if(cin.eof()) return is;
        char c= char(cin.get());
        if(cin.eof()) return is;
        while((c!='\\') && (c!='{') && (c!='}'))
          {
            if(c!=13) str.nmb+=c;
            c= char(cin.get());
            if(cin.eof()) break;
          }
        cin.putback(c);
        return is;
      }
    friend std::ostream &operator<<(std::ostream &os,const CadenaRTF &pc)
      {
        pc.Print(os);
        return os;
      }
  };

class PalabCtrl: public CadenaRTF
  {
  private:
    string arg;

    inline int EsArgumento(char c) const
      {
        if(isdigit(c) || c=='-') return 1;
        return 0;
      }
    int EsDelimitador(char c) const
      {
        if(c==' ') return 1;
        if(EsArgumento(c)) return 1;
        if(!isalnum(c)) return 1;
        return 0;
      }
  public:
    PalabCtrl(void): CadenaRTF(), arg() {}
    PalabCtrl(const PalabCtrl &otro)
      : CadenaRTF(otro), arg(otro.arg) {}
    virtual EntidadRTF *Copia(void) const
      { return new PalabCtrl(*this); }
    const string &GetArgumento(void) const
      { return arg; }
    virtual int Coincide (const string &str) const
      { return (getName() == str); }
    virtual void Print(std::ostream &os) const
      {  
        os << "palabra de control: " << getName()
           << " argumento: " << GetArgumento();
      }
    virtual void PrintLatex(std::ostream &os) const
      {}
    friend istream &operator>>(istream &is,PalabCtrl &pc)
      {
        char c= char(cin.get());
        while(!pc.EsDelimitador(c))
          {
            if(c!=13) pc.nmb+=c;
            c= char(cin.get());
          }
        while(pc.EsArgumento(c))
          {
            if(c!=13) pc.arg+=c;
            c= char(cin.get());
          }
        cin.putback(c);
        return is;
      }
    friend std::ostream &operator<<(std::ostream &os,const PalabCtrl &pc)
      {
        pc.Print(os);
        return os;
      }
  };

//Lista de entidades polimorfas.
class GrupoRTF: public poli_deque<EntidadRTF>
  {
    int BuscaCadena(const string &str) const
      {
	poli_deque<EntidadRTF>::const_iterator i= begin();
        for(;i!=end();i++)
          if((*i)->Coincide(str))
            {
              return 1;
              break;
            }
        return 0;
      }
    int Imprimible(void) const
      {
        if(BuscaCadena("fonttbl")) return 0;
        if(BuscaCadena("*")) return 0;
        if(BuscaCadena("froman")) return 0;
        if(BuscaCadena("fswiss")) return 0;
        if(BuscaCadena("fmodern")) return 0;
        if(BuscaCadena("colortbl")) return 0;
        if(BuscaCadena("nowidctlpar")) return 0;
        if(BuscaCadena("title")) return 0;
        if(BuscaCadena("author")) return 0;
        if(BuscaCadena("operator")) return 0;
        if(BuscaCadena("pntxta")) return 0;
        if(BuscaCadena("pntxtb")) return 0;
        if(BuscaCadena("levelnumbers")) return 0;
        if(BuscaCadena("listname")) return 0;
        return 1;
      }
  public:
    typedef poli_deque<EntidadRTF> polidq_ent;
    GrupoRTF(void): polidq_ent() {}
    GrupoRTF(const GrupoRTF &otro): polidq_ent(otro) {}
    /*
    int In(const EntidadRTF *c) const
      //Devuelve verdadero si encuentra el elemento.
      { return (find(begin(),end(),c)!=end()); }
    */
    void Print(std::ostream &os) const
      {
        std::cout << "Grupo: " << std::endl;
        std::cout << "  Imprimible: " << Imprimible() << std::endl;
        for(polidq_ent::const_iterator i=begin();i!=end();i++)
          { (*(*i)).Print(os); os << std::endl; }
      }
    void PrintLatex(std::ostream &os) const
      {
        if(!Imprimible()) return;
        for(polidq_ent::const_iterator i=begin();i!=end();i++)
          (*(*i)).PrintLatex(os);
      }
  };

typedef stack<GrupoRTF> PilaGrupos;

int main(void)
  {
    ofstream salida("pp.tex");
    PilaGrupos pila;
    size_t grupos_abiertos= 0;
    char c;
    while(!cin.eof())
      {
        c= char(cin.get());
        if(cin.eof()) break;
        if(c==13) continue; //quitamos el avance de linea.
        if(c=='\\')
          {
            char sig= char(cin.peek());
            if(!isalnum(sig))
              {
                if(sig == '\'')
                  {
                    CaracterHex ch;
                    cin >> ch;
                    pila.top().push_back(ch);
                  }
                else
                  {
                    SimbCtrl sc;
                    cin >> sc;
                    pila.top().push_back(sc);
                  }
              }
            else
              {
                PalabCtrl pc;
                cin >> pc;
                pila.top().push_back(pc);
              }
          }
        else
          if(c=='{')
            {
              grupos_abiertos++;
              pila.push(GrupoRTF());
            }
          else
            if(c=='}')
              {
                grupos_abiertos--;
                std::cout << std::endl << "Entrada RTF: " << std::endl;
                pila.top().Print(std::cout);
                std::cout << "Salida latex: " << std::endl;
                pila.top().PrintLatex(std::cout);
                pila.top().PrintLatex(salida);
                pila.pop();
              }
            else
              {
                CadenaRTF str(c);
                cin >> str;
                pila.top().push_back(str);
              }
        if(!grupos_abiertos) break;
      }
    return 0;
  }
