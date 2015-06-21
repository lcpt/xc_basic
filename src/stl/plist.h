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
//plist.h
#ifndef PLIST_H
#define PLIST_H

#include "puntero.h"
#include <list>

template <class N>
class plist : public std::list<puntero<N> >
  {
  public:
    typedef typename std::list<puntero<N> >::size_type sz_type;

    plist(void): std::list<puntero<N> >() {}
    plist(sz_type n,const puntero<N> &value = puntero<N>())
      :std::list<puntero<N> >(n,value) {}
    plist(const plist<N> &otro)
      :std::list<puntero<N> >(otro) {}
    plist<N> &plist::operator =(const plist<N> &otro)
      {
        std::list<puntero<N> >::operator=(otro);
        return *this;
      }
    friend bool operator==(const plist<N> &pv1,const plist<N> &pv2)
      {
        return ((const std::list<puntero<N> > &) pv1 == (const std::list<puntero<N> > &)pv2);
      }      
  };

template <class N>
std::ostream &operator << (std::ostream &stream,const plist<N> &pl)
  {
    typename plist<N>::const_iterator i;
    size_t j= 0,sz = pl.size();
    for (i= pl.begin();i != pl.end();i++)
      {
	stream << (const N &)*i;
	j++;
	if (j < sz) stream << ',';
      }
    return stream;
  }
  
#endif

