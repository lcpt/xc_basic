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

#ifndef MEDMEM_ARRAY_HXX
#define MEDMEM_ARRAY_HXX

#include "MEDMEM.hxx"

#include "MEDMEM_InterlacingPolicy.hxx"
#include "MEDMEM_IndexCheckingPolicy.hxx"

#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"

namespace MEDMEM {

class MEDMEM_EXPORT MEDMEM_Array_ {
public:
  //virtual void dummy() {};
  virtual bool getGaussPresence() const { return false; }
  virtual MED_EN::medModeSwitch getInterlacingType() const {return MED_EN::MED_UNDEFINED_INTERLACE;}
  virtual ~MEDMEM_Array_() {}; //Indispensable pour détruire le vrai objet pointé
};

template < class ARRAY_ELEMENT_TYPE,
           class INTERLACING_POLICY=FullInterlaceNoGaussPolicy,
           class CHECKING_POLICY=IndexCheckPolicy >
class MEDMEM_Array : public INTERLACING_POLICY, public CHECKING_POLICY, public MEDMEM_Array_ {

public :

  typedef ARRAY_ELEMENT_TYPE  ElementType;
  typedef INTERLACING_POLICY  InterlacingPolicy;
  typedef CHECKING_POLICY     CheckingPolicy;

public  :
  MEDMEM_Array():_array( ( ElementType *) NULL)  {}; //Interdit le constructeur par défaut, peut pas à  cause du FIELD

  ~MEDMEM_Array() {
    // PointerOf s'occupe de la desallocation.
  };

  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé ( ...NoGaussPolicy)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array(int dim, int nbelem) : InterlacingPolicy(nbelem,dim)
  {
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbelem);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",dim);
    _array.set(InterlacingPolicy::_arraySize);
  };

  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé NoInterlaceByTypeNoGaussPolicy(...)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array(int dim, int nbelem,
                      int nbtypegeo, const int * const nbelgeoc)
    : InterlacingPolicy(nbelem, dim, nbtypegeo, nbelgeoc)
  {
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbelem);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",dim);
    _array.set(InterlacingPolicy::_arraySize);
  };

  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé ( ...NoGaussPolicy)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array( ElementType * values, int dim, int nbelem,
                       bool shallowCopy=false,
                       bool ownershipOfValues=false) : InterlacingPolicy(nbelem,dim)
  {
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbelem);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",dim);
    if(shallowCopy)

      if(ownershipOfValues)
        _array.setShallowAndOwnership((const ElementType *)values);
      else
        _array.set((const ElementType*)values);

    else // Cas par défaut
      _array.set(InterlacingPolicy::_arraySize,values);

  }

  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé NoInterlaceByTypeNoGaussPolicy(...)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array( ElementType * values, int dim, int nbelem,
                       int nbtypegeo, const int * const  nbelgeoc,
                       bool shallowCopy=false,
                       bool ownershipOfValues=false) 
    : InterlacingPolicy(nbelem, dim, nbtypegeo, nbelgeoc)
  {
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbelem);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",dim);
    if(shallowCopy)

      if(ownershipOfValues)
        _array.setShallowAndOwnership((const ElementType *)values);
      else
        _array.set((const ElementType*)values);

    else // Cas par défaut
      _array.set(InterlacingPolicy::_arraySize,values);

  }

  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé ( ...GaussPolicy)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array(int dim, int nbelem, int nbtypegeo,
                      const int * const  nbelgeoc, const int * const nbgaussgeo)
    : InterlacingPolicy(nbelem, dim, nbtypegeo, nbelgeoc, nbgaussgeo)
  {
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbelem);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",dim);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbtypegeo);
    _array.set(InterlacingPolicy::_arraySize);
  };


  // Le mot clé inline permettra d'instancier le constructeur uniquement
  // s'il est appelé ( ...GaussPolicy)
  // Rem : Le constructeur de la policy demandée est appelé
  inline MEDMEM_Array(ElementType * values, int dim, int nbelem, int nbtypegeo,
                      const int * const  nbelgeoc, const int * const  nbgaussgeo,
                      bool shallowCopy=false,
                      bool ownershipOfValues=false)
    : InterlacingPolicy(nbelem, dim, nbtypegeo, nbelgeoc, nbgaussgeo)
  {
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbelem);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",dim);
    CHECKING_POLICY::checkMoreThanZero("MEDMEM_Array",nbtypegeo);

    if(shallowCopy)

      if(ownershipOfValues)
        _array.setShallowAndOwnership((const ElementType *)values);
      else
        _array.set((const ElementType*)values);

    else
      _array.set(InterlacingPolicy::_arraySize,values);

  };

  // Constructeur de recopie pour un MEDMEM_Array avec les mêmes
  // paramètres template qu'à la construction
  inline MEDMEM_Array(const MEDMEM_Array & array, bool shallowCopy=false)
    :InterlacingPolicy(array,shallowCopy)
  {
    if (shallowCopy)
      this->_array.set(array._array); // Le propriétaire reste le ARRAY initial
    else
      this->_array.set(InterlacingPolicy::_arraySize,array._array);
  }


  // L'utilisation d'une copie superficielle pour l'opérateur d'affectation
  // ne me parait pas être une bonne ideé : Compatibilité ancienne version MEDARRAY?
  inline MEDMEM_Array<ElementType,InterlacingPolicy,CheckingPolicy> &
         operator=( const MEDMEM_Array & array) {
    if ( this == &array) return *this;
  const char* LOC = "MEDMEM_Array  operator =";
  BEGIN_OF_MED(LOC);
    InterlacingPolicy::operator=(array); //Appel des classes de base ?

    this->_array.set(array._array); // Le propriétaire reste le ARRAY initial

    return *this;
  }

  MED_EN::medModeSwitch getInterlacingType() const {
    return InterlacingPolicy::getInterlacingType();
  }

  bool getGaussPresence() const {
    return InterlacingPolicy::getGaussPresence();
  }

  ElementType * getPtr() {
    return  _array;
  }

  const ElementType * getPtr() const {
    return  _array;
  }

  void setPtr(ElementType * values, bool shallowCopy=false,
              bool ownershipOfValues=false) {

    if(shallowCopy)

      if(ownershipOfValues)
        _array.setShallowAndOwnership((const ElementType *)values);
      else
        _array.set((const ElementType*)values);

    else
      _array.set(InterlacingPolicy::_arraySize,values);
  }

  inline const ElementType * getRow(int i) const {
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    // Empêche l'utilisation de getRow en mode MED_NO_INTERLACE
    // Ne devrait pas dépendre de la politique check
    CHECKING_POLICY::checkEquality("MEDMEM_Array (Interlace test)",
                                   MED_EN::MED_NO_INTERLACE,
                                   InterlacingPolicy::_interlacing );
    return &(_array[ InterlacingPolicy::getIndex(i,1) ]);

  }

  void setRow(int i,const ElementType * const value) {
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    // setRow fonctionne
    // dans les deux modes d'entrelacement.

    // int index = -1;
    for (int j =1; j <= InterlacingPolicy::getDim(); j++) {
      for (int k = 1 ; k <= InterlacingPolicy::getNbGauss(i); k++) {
        _array[InterlacingPolicy::getIndex(i,j,k)] = value[InterlacingPolicy::getIndex(1,j,k)];
        //index++;
        //_array[InterlacingPolicy::getIndex(i,j,k)] = value[index];
      }
    }
  }

  inline const ElementType * getColumn(int j) const {
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    CHECKING_POLICY::checkEquality("MEDMEM_Array (Interlace test)",
                                   MED_EN::MED_FULL_INTERLACE, InterlacingPolicy::_interlacing );
    return &(_array[ InterlacingPolicy::getIndex(1,j) ]);
  }

  void setColumn(int j, const ElementType * const value) {
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    // setColumn fonctionne
    // dans les deux modes d'entrelacement.

    int index = -1;
    for (int i=1; i <= InterlacingPolicy::getNbElem(); i++) {
      for (int k = 1 ; k <= InterlacingPolicy::getNbGauss(i); k++) {
        //_array[InterlacingPolicy::getIndex(i,j,k)] = value[InterlacingPolicy::getIndex(i,1,k)];
        index++;
        _array[InterlacingPolicy::getIndex(i,j,k)] = value[index];
      }
    }
  }


  inline const ElementType & getIJ(int i, int j) const  {
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    return _array[ InterlacingPolicy::getIndex(i,j) ];
  }

  inline const ElementType & getIJK(int i, int j, int k ) const {
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::getNbGauss(i),k);

    return _array[ InterlacingPolicy::getIndex(i,j,k) ];
  };

  inline const ElementType & getIJByType(int i, int j, int t) const  {
    if ( getInterlacingType() != MED_EN::MED_NO_INTERLACE_BY_TYPE )
      throw MEDEXCEPTION(LOCALIZED(STRING("Wrong interlacing type ") << getInterlacingType()));
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::getNbGeoType(),t);
    if ( InterlacingPolicy::getGaussPresence() )
      return _array[ ((NoInterlaceByTypeGaussPolicy*)this)->getIndexByType(i,j,t) ];
    else
      return _array[ ((NoInterlaceByTypeNoGaussPolicy*)this)->getIndexByType(i,j,t) ];
  }

  inline const ElementType & getIJKByType(int i, int j, int k, int t) const {
    if ( getInterlacingType() != MED_EN::MED_NO_INTERLACE_BY_TYPE )
      throw MEDEXCEPTION(LOCALIZED(STRING("Wrong interlacing type ") << getInterlacingType()));
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::getNbGeoType(),t);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);

    if ( InterlacingPolicy::getGaussPresence() ) {
        // not compilable on Debian40
//       CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",
//                             1,((NoInterlaceByTypeGaussPolicy*)this)->getNbGaussByType(t),k);
      int kmax = ((NoInterlaceByTypeGaussPolicy*)this)->getNbGaussByType(t);
      if ( k < 1 || k > kmax )
        throw MEDEXCEPTION(LOCALIZED(STRING("MEDMEM_Array::getIJKByType(), ")
                                     << " k : " << k << " not in rang [1," << kmax <<"]"));
      return _array[ ((NoInterlaceByTypeGaussPolicy*)this)->getIndexByType(i,j,k,t) ];
    }
    else {
      CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::getNbGauss(i),k);
      return _array[ ((NoInterlaceByTypeNoGaussPolicy*)this)->getIndexByType(i,j,k,t) ];
    }
  };

  inline void setIJ(int i, int j, const ElementType & value) {   //autre signature avec
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);

    _array[ InterlacingPolicy::getIndex(i,j) ] = value;                      // retour ElementType & ?
  };

  inline void setIJByType(int i, int j, int t, const ElementType & value) {   //autre signature avec
    if ( getInterlacingType() != MED_EN::MED_NO_INTERLACE_BY_TYPE )
      throw MEDEXCEPTION(LOCALIZED(STRING("Wrong interlacing type ") << getInterlacingType()));
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::getNbGeoType(),t);

    if ( InterlacingPolicy::getGaussPresence() )
      _array[ ((NoInterlaceByTypeGaussPolicy*)this)->getIndexByType(i,j,t) ] = value;
    else
      _array[ ((NoInterlaceByTypeNoGaussPolicy*)this)->getIndexByType(i,j,t) ] = value;
  };

  inline void setIJK(int i, int j, int k, const ElementType & value)
    {   //autre signature avec
      CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
      CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
      CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::getNbGauss(i),k);

      _array[ InterlacingPolicy::getIndex(i,j,k) ] = value;                      // retour ElementType & ?
    };

  inline void setIJKByType(int i, int j, int k, int t, const ElementType & value) {   //autre signature avec
    if ( getInterlacingType() != MED_EN::MED_NO_INTERLACE_BY_TYPE )
      throw MEDEXCEPTION(LOCALIZED(STRING("Wrong interlacing type ") << getInterlacingType()));
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_nbelem,i);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::_dim,j);
    CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::getNbGeoType(),t);

    if ( InterlacingPolicy::getGaussPresence() ) {
        // not compilable on Debian40
//       CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",
//                             1,((NoInterlaceByTypeGaussPolicy*)this)->getNbGaussByType(t),k);
      int kmax = ((NoInterlaceByTypeGaussPolicy*)this)->getNbGaussByType(t);
      if ( k < 1 || k > kmax )
        throw MEDEXCEPTION(LOCALIZED(STRING("MEDMEM_Array::getIJKByType(), ")
                                     << " k : " << k << " not in rang [1," << kmax <<"]"));
      _array[ ((NoInterlaceByTypeGaussPolicy*)this)->getIndexByType(i,j,k,t) ] = value;
    }
    else {
      CHECKING_POLICY::checkInInclusiveRange("MEDMEM_Array",1,InterlacingPolicy::getNbGauss(i),k);
      _array[ ((NoInterlaceByTypeNoGaussPolicy*)this)->getIndexByType(i,j,k,t) ] = value;
    }
  };

  bool operator == (const MEDMEM_Array & array ) const {

    if ( this == &array ) return true;

    int size = array.getArraySize();
    if ( size != this->getArraySize() ) return false;

    ARRAY_ELEMENT_TYPE * arrayPtr =
      const_cast<MEDMEM_Array &>(array).getPtr();
    for (int i=0; i < size; ++i)
      if (_array[i] != arrayPtr[i]) return false;

    return true;
  }

  friend ostream & operator<<(ostream & os, const MEDMEM_Array & array) {

    for (int i=1;i<=array.getNbElem();++i) {
      for (int j=1; j<=array.getDim();++j)
        for (int k=1;k<=array.getNbGauss(i);++k)
          os << "Value [" << i << "," << j << "," << k << "] = " << array.getIJK(i,j,k) << ", ";
      os << endl;
    }
    return os;
  }

private:

  PointerOf<ElementType> _array;
};

} //END NAMESPACE
#endif
