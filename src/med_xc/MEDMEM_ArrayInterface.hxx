// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef MEDMEM_ARRAYINTERFACE_HXX
#define MEDMEM_ARRAYINTERFACE_HXX

#include "MEDMEM_nArray.hxx"
#include "MEDMEM_InterlacingTraits.hxx"

// L'astuce d'une classe d'interface consiste en 
// 1) La déclaration d'un type qui est celui de la classe d'implémentation
// 2) D'utiliser ce nouveau nom de type comme paramètres de toutes
//     les méthodes de l'interface.
// L'inconvenient est qu'il faut justement passer en argument une instance de
// le classe d'implémentation dans toutes les méthodes et que la classe
// appelante aura aussi à faire ce travail.
//  Ne surtout pas oublier inline sinon l'interface couterait cher à l'appel
//  des méthodes !
namespace MEDMEM {

template < class ARRAY_ELEMENT_TYPE,
           class INTERLACE_TAG,
           class GAUSS_TAG,
           class CHECKING_POLICY=IndexCheckPolicy>
           //NoIndexCheckPolicy>
class MEDMEM_EXPORT MEDMEM_ArrayInterface  {

public:

  // Les type ElementType et Array sont a définir aussi dans les classes utilisatrice
  // par une déclaration du type : typedef typename ArrayInterface::Array Array;

  typedef  ARRAY_ELEMENT_TYPE  ElementType;
  typedef  INTERLACE_TAG       Interlacing;
  typedef  GAUSS_TAG           GaussPresence;
  typedef  typename MEDMEM_InterlacingTraits<Interlacing,GaussPresence>::Type InterlacingPolicy;
  typedef  CHECKING_POLICY     CheckingPolicy;
  typedef  MEDMEM_Array<ElementType,InterlacingPolicy,CheckingPolicy> Array;

  static inline int getNbGauss(int i, const Array & array)  {
    return array.getNbGauss(i);
  };

  static inline ElementType * getPtr( Array & array)  {
    return array.getPtr();
  };

  static inline void setPtr( ElementType * arrayptr,  Array & array, 
                             bool shallowCopy=false,
                             bool ownershipOfValues=false )  {
    array.setPtr(arrayptr,shallowCopy,ownershipOfValues);
  };

  static inline const ElementType * getRow(int i, const Array & array ) {
    return array.getRow(i);
  }

  static inline void setRow(int i, const ElementType & value, const Array & array ) {
    return array.setRow(i,value);
  }

  static inline const ElementType * getColumn(int j, const Array & array ) {
    return array.getColumn(j);
  }

  static inline void setColumn(int j, const ElementType & value, const Array & array ) {
    return array.setColumn(j,value);
  }

  static inline const ElementType & getIJ(int i, int j, const Array & array) {
    return array.getIJ(i,j);
  }

  static inline const ElementType & getIJK(int i, int j, int k, const Array & array) {
    return array.getIJK(i,j,k);
  }

  static inline void setIJ(int i, int j, const ElementType & value, Array & array) {
    array.setIJ(i,j,value);
  }

  static inline void setIJK(int i, int j, int k, const ElementType & value, Array & array) {
    array.setIJK(i,j,k,value);
  }

};

} //END NAMESPACE
#endif
