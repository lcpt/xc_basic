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
//pvector.h
#ifndef PVECTOR_H
#define PVECTOR_H

#include "puntero.h"
#include <vector>
#include <iostream>

template <class N>
class pvector : public std::vector< puntero<N> >
  {
  public:
    typedef typename pvector<N>::const_iterator c_iterator;
    typedef std::vector< puntero<N> >  v_pN;
    typedef typename pvector<N>::size_type sz_type;
    pvector(void): v_pN() {}
    pvector(sz_type n,const puntero<N> &value = puntero<N>())
      :v_pN(n,value) {}
    pvector(const pvector<N> &otro): v_pN(otro) {}
    pvector<N> &operator =(const pvector<N> &otro)
      {
        std::vector<puntero<N> >::operator=(otro);
        return *this;
      }
    N *get_ptr(size_t i) const
      { return (*this)[i].get_ptr(); }
    N *get_const_ptr(size_t i) const
      { return (*this)[i].get_const_ptr(); }
    int Check(void) const //Verdadero si apuntadores no nulos.
      {
        c_iterator i;
        for(i=begin();i!=end();i++)
          if(*i == NULL) return 0;
        return 1;
      }
    friend bool operator==(const pvector<N> &pv1,const pvector<N> &pv2)
      {
        return ((const std::vector<puntero<N> > &) pv1 == (const std::vector<puntero<N> > &)pv2);
      }
  };

template <class N>
std::ostream &operator << (std::ostream &stream,const pvector<N> &pv)
  {
    typename pvector<N>::const_iterator i;
    for (i= pv.begin();i != pv.end();i++)
      {
	stream << (const N &)*i;
	if (i < (pv.end()-1)) stream << ',';
      }
    return stream;
  }

#endif

