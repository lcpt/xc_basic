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
//set_key.h
//Contenedor que ordena los objetos contenidos por uno
//de sus campos. La clase de los objetos a contener debe
//definir:
//
//El tipo "tipo_indice" que será la clase a la que pertenecen los índices.
//La función Indice() que debe devolver un valor de tipo "tipo_indice" que
//se empleará como índice para almacenar el objeto.
//
//El contenedor no admite índices repetidos.

#ifndef SET_KEY_H
#define SET_KEY_H

#include <cassert>
#include <set>

template <class T>
struct menor
  {
    bool operator()(const T &s1, const T &s2) const
     {
       return s1.Indice() < s2.Indice();
     }
  };

template <class T>
class set_key: protected std::set<T,menor<T> >
  {
  public:
    typedef std::set<T,menor<T> > set_t;
    typedef typename set_t::iterator iterator;
    typedef typename set_t::const_iterator const_iterator;
    typedef typename T::tipo_indice tipo_indice;

    const_iterator begin(void) const
      { return set_t::begin(); }
    const_iterator end(void) const
      { return set_t::end(); }
    iterator begin(void)
      { return set_t::begin(); }
    iterator end(void)
      { return set_t::end(); }
    void insert(const T &t)
      {
        iterator i= find(t);
	if(i!=end()) //Si existe lo borra.
          set_t::erase(i);
	set_t::insert(t);
      }
    inline iterator busca(const tipo_indice &idx)
      { return find(T(idx)); }
    inline const_iterator busca(const tipo_indice &idx) const
      { return find(T(idx)); }
    const T &operator[](const tipo_indice &idx) const
      {
        const_iterator retval= busca(idx);
        assert(retval!=end());
        return *retval;
      }
    inline void clear(void)
      { set_t::clear(); }
  };

#endif
