//----------------------------------------------------------------------------
//  biblioteca xc_basic; funciones y clases de uso general
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  Este software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//MapSto.h

#ifndef MAPSTO_H
#define MAPSTO_H

#include <iostream.h>
#include "streams/imanip.h"
#include "map.h"


//Mapa con algun m'etodo util y otros para compatibilidad con codigo viejo.
template<class INDICE,class T>
class MapSto:public map<INDICE,T,less<INDICE> >
  {
    typedef map<INDICE,T,less<INDICE> > map_Ts;
  public:
    MapSto(void): map_Ts() {}
    const T &Get(INDICE index)
      { return map_Ts::operator[](index); }
    const T *GetPtr(INDICE index) const
      { return &((*find(index)).second); }
    const T *Busca(INDICE id) const
      {
        const_iterator i= find(id);
        if(i!=end()) return &((*i).second);
        return NULL;
      }
    void Load(istream &stream)
      {
        stream >> ignore_to('{');
        T n;
        INDICE uid;
        char c= stream.get();
        if(c == '}')
          return;
        else
          stream.putback(c);
        while(stream.ipfx(0))
          {
            stream >> uid >> n;
            (*this)[uid]= n;
            if(stream.ipfx(0)) c= stream.get();
            if(c!='}')
              stream.putback(c);
            else
              break;
          }
      }
    void Save(ostream &os) const
      {
        if(size()<1) return;
        const_iterator i= begin();
        os << (*i).first << ' ' << (*i).second;
        i++;
        for (;i!=end();i++)
          os << endl << (*i).first << ' ' << (*i).second;
      }
    friend ostream &operator<<(ostream &stream,const MapSto<INDICE,T> &ns)
      {
        ns.Save(stream);
        return stream;
      }
    friend istream &operator>>(istream &stream,MapSto<INDICE,T> &tn)
      {
        tn.Load(stream);
        return stream;
      }
  };

#endif
