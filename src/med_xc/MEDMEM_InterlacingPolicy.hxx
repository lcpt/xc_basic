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

#ifndef MEDMEM_INTERLACING_HXX
#define MEDMEM_INTERLACING_HXX

#include "MEDMEM.hxx"

#include <iostream>
#include "MEDMEM_Utilities.hxx"

#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"

namespace MEDMEM {

class MEDMEM_EXPORT InterlacingPolicy
  {
  protected:
    ~InterlacingPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique
  public :
    InterlacingPolicy(void);
    InterlacingPolicy(int nbelem, int dim, int arraySize=0, int interlacing=MED_EN::MED_UNDEFINED_INTERLACE);
    InterlacingPolicy(const InterlacingPolicy & intpol, bool shallowcopy = true);
    InterlacingPolicy & operator=(const InterlacingPolicy & intpol);

    inline int getDim()       const { return _dim; }
    inline int getNbElem()    const { return _nbelem; }
    inline int getArraySize() const { return _arraySize; }
    inline MED_EN::medModeSwitch getInterlacingType() const {return _interlacing;}
    inline bool getGaussPresence() const { return _gaussPresence;}
    virtual int getNbGauss(int i) const = 0;

    int _dim;
    int _nbelem;
    int _arraySize;
    MED_EN::medModeSwitch _interlacing;
    bool _gaussPresence;
  };


class MEDMEM_EXPORT FullInterlaceNoGaussPolicy : public  InterlacingPolicy
  {
  protected:
    ~FullInterlaceNoGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique
  public :
    FullInterlaceNoGaussPolicy();
    FullInterlaceNoGaussPolicy(int nbelem, int dim);
    FullInterlaceNoGaussPolicy(const FullInterlaceNoGaussPolicy & policy, bool shallowcopie=true);
 
    inline int getIndex(int i,int j) const
      { return (i-1)*_dim + j-1; }

    inline int getIndex(int i,int j,int k) const
      { return (i-1)*_dim + j-1; }

    inline int getNbGauss(int i) const { return 1; }
  };

class MEDMEM_EXPORT NoInterlaceNoGaussPolicy : public InterlacingPolicy
  {
  protected:
    ~NoInterlaceNoGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique
  public:

  NoInterlaceNoGaussPolicy():InterlacingPolicy() {}
  NoInterlaceNoGaussPolicy(int nbelem, int dim) : 
    InterlacingPolicy(nbelem, dim, dim*nbelem,MED_EN::MED_NO_INTERLACE) {}

  NoInterlaceNoGaussPolicy(const NoInterlaceNoGaussPolicy & policy,
                              bool shallowcopie=true)
    : InterlacingPolicy(policy) {}

  inline int getIndex(int i,int j) const {
    return (j-1)*_nbelem + i-1;
  }

  inline int getIndex(int i,int j,int k) const {
    return (j-1)*_nbelem + i-1;
  }

  inline int getNbGauss(int i) const { return 1; }

};

class MEDMEM_EXPORT GInterlacingPolicy: public InterlacingPolicy
  {
  protected:
    PointerOf<int> _G; //!< where type begin
    int _nbtypegeo;
    PointerOf<int> _nbelegeoc;
  public :
    GInterlacingPolicy(void);
    GInterlacingPolicy(int nbelem, int dim);
    GInterlacingPolicy(int nbelem, int dim, int nbtypegeo,const int * const nbelgeoc, int interlacing);
    GInterlacingPolicy(const GInterlacingPolicy &policy, bool shallowcopie=true);
    GInterlacingPolicy &operator=(const GInterlacingPolicy &policy);

    inline int getNbGeoType() const {return _nbtypegeo;}
    inline const int * const getNbElemGeoC() const {return _nbelegeoc;}
  };
class MEDMEM_EXPORT GTInterlacingPolicy : public GInterlacingPolicy
  {
  protected:
    ~GTInterlacingPolicy(void) {} //pour éviter qu'un utilisateur cast la class array en politique
    PointerOf<int> _T; //!< type of element
  public :
    GTInterlacingPolicy(void);
    GTInterlacingPolicy(int nbelem, int dim);
    GTInterlacingPolicy(int nbelem, int dim, int nbtypegeo,const int * const nbelgeoc, int interlacing);
    GTInterlacingPolicy(const GTInterlacingPolicy &policy, bool shallowcopie=true);

    GTInterlacingPolicy &operator=(const GTInterlacingPolicy &policy);
  };

class MEDMEM_EXPORT NoInterlaceByTypeNoGaussPolicy : public GTInterlacingPolicy
  {
  protected:
    ~NoInterlaceByTypeNoGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique
  public :
    NoInterlaceByTypeNoGaussPolicy(void);
    NoInterlaceByTypeNoGaussPolicy(int nbelem, int dim);
    NoInterlaceByTypeNoGaussPolicy(int nbelem, int dim, int nbtypegeo,const int * const nbelgeoc);
    NoInterlaceByTypeNoGaussPolicy(const NoInterlaceByTypeNoGaussPolicy & policy, bool shallowcopie=true);

    NoInterlaceByTypeNoGaussPolicy & operator=(const NoInterlaceByTypeNoGaussPolicy & policy);

    inline int getIndex(int t) const
      { return _G[t]; }
    inline int getIndex(int i,int j) const
      {
        int t = _T[i];
        return getIndexByType( i-(_nbelegeoc[t-1]-_nbelegeoc[0]), j, t );
      }

    inline int getIndex(int i,int j,int k) const
      { return getIndex(i,j); }

    inline int getIndexByType(int i,int j,int t) const
      { return _G[t] + i-1 + (j-1)*(_nbelegeoc[t]-_nbelegeoc[t-1]); }

    inline int getIndexByType(int i,int j,int k,int t) const 
      { return getIndexByType( i, j, t ); }

    inline int getLengthOfType(int t) const
      { return (_nbelegeoc[t]-_nbelegeoc[t-1]) * _dim; }

    inline int getNbGauss(int i) const { return 1; }
    inline int getNbGeoType() const {return _nbtypegeo;}
    inline const int * const getNbElemGeoC() const {return _nbelegeoc;}
  };

class MEDMEM_EXPORT GSInterlacingPolicy: public GInterlacingPolicy
  {
  protected:
    ~GSInterlacingPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique
  public:
    MEDMEM::PointerOf<int> _ESE;

    GSInterlacingPolicy(void);
    GSInterlacingPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc, int interlacing);
    GSInterlacingPolicy(const GSInterlacingPolicy & policy, bool shallowcopie=true);
    GSInterlacingPolicy & operator=(const GSInterlacingPolicy & policy);
  };

class MEDMEM_EXPORT FullInterlaceGaussPolicy : public GSInterlacingPolicy
  {
  protected:
    ~FullInterlaceGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique
  public:
    PointerOf<int> _nbgaussgeo;

    FullInterlaceGaussPolicy(void);
    FullInterlaceGaussPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc, const int * const nbgaussgeo);

    FullInterlaceGaussPolicy(const FullInterlaceGaussPolicy & policy, bool shallowcopie=true);
    FullInterlaceGaussPolicy & operator=(const FullInterlaceGaussPolicy & policy);

    inline int getIndex(int i,int j ) const
      { return _G[i-1]-1 + (j-1); }

    inline int getIndex(int i,int j, int k ) const
      {
        //std::cout << "Index : " << _G[i-1]-1 + _dim *(k-1) + (j-1) << std::endl;
        return _G[i-1]-1 +  (k-1)*_dim + (j-1);
      }

    inline int getNbGauss(int i) const { return _ESE[i]; }
    inline int getNbGeoType() const {return _nbtypegeo;}
    inline const int * const getNbElemGeoC() const {return _nbelegeoc;}
    inline const int * const getNbGaussGeo() const {return _nbgaussgeo;}
  };

class MEDMEM_EXPORT NoInterlaceGaussPolicy : public GSInterlacingPolicy
  {
  protected:
    ~NoInterlaceGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique
  public:
    PointerOf<int> _nbgaussgeo;
    // _cumul is used in getIndex() to access directly to the first value
    // of a given dimension.
    int _cumul;

    NoInterlaceGaussPolicy(void);
    NoInterlaceGaussPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc, const int * const nbgaussgeo);
    NoInterlaceGaussPolicy(const NoInterlaceGaussPolicy & policy, bool shallowcopie=true);
    NoInterlaceGaussPolicy & operator=(const NoInterlaceGaussPolicy & policy);

    inline int getIndex(int i,int j ) const
      { return _G[i-1]-1 + (j-1)*_cumul; }

    inline int getIndex(int i,int j, int k ) const
      { return _G[i-1]-1 + (j-1)*_cumul + (k-1); }

    inline int getNbGauss(int i) const { return _ESE[i]; }
    inline int getNbGeoType() const {return _nbtypegeo;}

    inline const int * const getNbElemGeoC() const {return _nbelegeoc;}
    inline const int * const getNbGaussGeo() const {return _nbgaussgeo;}
  };

class MEDMEM_EXPORT NoInterlaceByTypeGaussPolicy : public GTInterlacingPolicy
  {
  protected:
    ~NoInterlaceByTypeGaussPolicy() {} //pour éviter qu'un utilisateur cast la class array en politique
    PointerOf<int> _nbgaussgeo;
  public:
    NoInterlaceByTypeGaussPolicy();
    NoInterlaceByTypeGaussPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc, const int * const nbgaussgeo);
    NoInterlaceByTypeGaussPolicy(const NoInterlaceByTypeGaussPolicy & policy,bool shallowcopie=true);
    NoInterlaceByTypeGaussPolicy & operator=(const NoInterlaceByTypeGaussPolicy & policy);

    inline int getIndex(int t) const
      { return _G[t]; }
    inline int getIndex(int i,int j ) const
      {
        int t = _T[i];
        return getIndexByType( i-(_nbelegeoc[t-1]-_nbelegeoc[0]), j, t );
      }

    inline int getIndex(int i,int j, int k ) const
      { return getIndex( i, j ) + (k-1); }

    inline int getIndexByType(int i,int j,int t) const
      { return _G[t] + (i-1 + (j-1)*(_nbelegeoc[t]-_nbelegeoc[t-1])) * _nbgaussgeo[t]; }

    inline int getIndexByType(int i,int j,int k,int t) const
      { return getIndexByType( i,j,t ) + (k-1); }

    inline int getNbGauss(int i) const { return _nbgaussgeo[ _T[i] ]; }
    inline int getNbGaussByType(int t) const { return _nbgaussgeo[ t ]; }
    inline int getNbGeoType() const {return _nbtypegeo;}
    inline const int * const getNbElemGeoC() const {return _nbelegeoc;}
    inline const int * const getNbGaussGeo() const {return _nbgaussgeo;}
    inline int getLengthOfType(int t) const
      { return (_nbelegeoc[t]-_nbelegeoc[t-1]) * _dim * _nbgaussgeo[t]; }
  };

} //END NAMESPACE

#endif
