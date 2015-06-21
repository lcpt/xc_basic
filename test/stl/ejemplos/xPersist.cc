//xPersist.cc

#include "../persist.h" //Se coloca la primera y se compila con -frtti
#include "../PstSto.h"
#include "matemat/geom/Pos3d.h"
#include "matemat/geom/Pos2d.h"
#include "matemat/geom/Segmento.h"

CargaT<Pos3d,istream,ostream> cp3d(&cin,&cout);
CargaT<Pos2d,istream,ostream> cp2d(&cin,&cout);
CargaT<Segmento,istream,ostream> cseg(&cin,&cout);

int main(void)
  {
    DEFINE_PST_CLASS(Pos3d,&cp3d);
    DEFINE_PST_CLASS(Pos2d,&cp2d);
    DEFINE_PST_CLASS(Segmento,&cseg);
    cout << class_info() << endl;
    PstStore<istream> pst(cin);
    Pos3d *p3= (Pos3d *)pst.Read();
    cout << *p3 << endl;
    delete p3;
    Pos2d *p2= (Pos2d *)pst.Read();
    cout << *p2 << endl;
    Recta *s= (Recta *)pst.Read();
    cout << (typeid(*s) == typeid(Recta)) << endl;
    cout << (typeid(*s) == typeid(Segmento)) << endl;
    const type_info& tir = typeid(*s);
    cout << "s es un puntero a " << tir.name() << endl;
    if(Recta *q= dynamic_cast<Recta *>(s))
      {
        cout << "Longitud= " << q->Longitud() << endl;
        cout << "BND= " << q->Bnd() << endl;
        cout << "CDG= " << q->Cdg() << endl;
      }
    delete s;
    const Pos3d p4;
    pst.Write(typeid(Pos3d),&p4);
    cout << endl;
    return 0;
  }


