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
//map_key.h
//Contenedor que ordena los objetos contenidos por uno
//de sus campos. La clase de los objetos a contener debe
//definir:
//
//El tipo "tipo_indice" que será la clase a la que pertenecen los índices.
//La función Indice() que debe devolver un valor de tipo "tipo_indice" que
//se empleará como índice para almacenar el objeto.
//
//El contenedor no admite índices repetidos.

#ifndef MAP_KEY_H
#define MAP_KEY_H

#include <cassert>
#include <map>

template <typename T>
class map_key: protected std::map<typename T::tipo_indice,T>
  {
  public:
    typedef std::map<typename T::tipo_indice,T> map_t;
    typedef typename map_t::iterator iterator;
    typedef typename map_t::const_iterator const_iterator;
    typedef typename map_t::size_type size_type;
    typedef typename T::tipo_indice tipo_indice;

    const_iterator begin(void) const
      { return map_t::begin(); }
    const_iterator end(void) const
      { return map_t::end(); }
    iterator begin(void)
      { return map_t::begin(); }
    iterator end(void)
      { return map_t::end(); }
    void insert(const T &t)
      {
        iterator i= find(t.Indice());
	if(i!=end()) //Si existe lo borra.
          map_t::erase(i);
        typename map_t::value_type tmp(t.Indice(),t); 
	map_t::insert(tmp);
      }
    typename map_t::size_type size(void) const
      { return map_t::size(); }
    bool empty(void) const
      { return map_t::empty(); }
    inline iterator busca(const tipo_indice &idx)
      { return find(idx); }
    inline const_iterator busca(const tipo_indice &idx) const
      { return find(idx); }
    const T &operator[](const tipo_indice &idx) const
      {
        const_iterator retval= busca(idx);
        assert(retval!=end());
        return (*retval).second;
      }
    inline void clear(void)
      { map_t::clear(); }
  };

#endif
