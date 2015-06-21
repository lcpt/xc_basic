//MEDMEM_InterlacingPolicy.cxx

#include "MEDMEM_InterlacingPolicy.hxx"

MEDMEM::InterlacingPolicy::InterlacingPolicy(void)
  : _dim(0), _nbelem(0),
   _arraySize(0),
   _interlacing(MED_EN::MED_UNDEFINED_INTERLACE),
  _gaussPresence(false) {}

MEDMEM::InterlacingPolicy::InterlacingPolicy(int nbelem, int dim, int arraySize, int interlacing)
  : _dim(dim),_nbelem(nbelem), _arraySize(arraySize),_interlacing(interlacing),_gaussPresence(false) {}

//! @brief Constructeur par recopie
MEDMEM::InterlacingPolicy::InterlacingPolicy(const InterlacingPolicy & intpol, bool shallowcopy)
 :_dim(intpol._dim), _nbelem(intpol._nbelem), _arraySize(intpol._arraySize), _interlacing(intpol._interlacing), _gaussPresence(intpol._gaussPresence) {}

MEDMEM::InterlacingPolicy &MEDMEM::InterlacingPolicy::operator=(const InterlacingPolicy & intpol)
  {
    if ( this == &intpol ) return *this;
    const char* LOC = "InterlacingPolicy operator =";
    BEGIN_OF_MED(LOC);

    _dim           = intpol._dim;
    _nbelem        = intpol._nbelem; //ne prend pas en compte les points de Gauss
    _arraySize     = intpol._arraySize;
    _interlacing   = intpol._interlacing;
    _gaussPresence = intpol._gaussPresence;

    return *this;
  }

MEDMEM::FullInterlaceNoGaussPolicy::FullInterlaceNoGaussPolicy()
  : InterlacingPolicy() {}

MEDMEM::FullInterlaceNoGaussPolicy::FullInterlaceNoGaussPolicy(int nbelem, int dim)
  : InterlacingPolicy(nbelem, dim, dim*nbelem,MED_EN::MED_FULL_INTERLACE) {}

MEDMEM::FullInterlaceNoGaussPolicy::FullInterlaceNoGaussPolicy(const FullInterlaceNoGaussPolicy & policy, bool shallowcopie)
  : InterlacingPolicy(policy) {}

MEDMEM::FullInterlaceGaussPolicy::FullInterlaceGaussPolicy(void)
  :GSInterlacingPolicy()
    { GSInterlacingPolicy::_gaussPresence=true; }

MEDMEM::FullInterlaceGaussPolicy::FullInterlaceGaussPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc, const int * const nbgaussgeo)
    : GSInterlacingPolicy(nbelem, dim, nbtypegeo, nbelgeoc,MED_EN::MED_FULL_INTERLACE)
  {
    GSInterlacingPolicy::_gaussPresence=true;

    _nbelegeoc.set(_nbtypegeo+1,nbelgeoc);
    _nbgaussgeo.set(_nbtypegeo+1,nbgaussgeo);
    _G.set(nbelem+1);
    // _G[0]       = 1;
    _ESE.set(nbelem+1);
    _ESE[0]       = -1;
    int cumul   = 0;
    int elemno  = 0;

    // Construction of _G
    for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
      for (int  i=0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) ; i++ ) {
        _G[ elemno ] = cumul + i*nbgaussgeo[ntyp]*dim + 1;
        elemno++;
        _ESE[ elemno ] = nbgaussgeo[ntyp];
      };
      cumul += (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) * nbgaussgeo[ntyp] * dim;
#ifdef ARRAY_DEBUG
      std::cout << "Valeur de cumul " << cumul << std::endl;
#endif
  };
    _G[ elemno ] = cumul+1;
    _arraySize   = _G[ elemno ] -1 ;

#ifdef ARRAY_DEBUG
    for (int i =0; i< nbelem+1; i++ )
      std::cout << "Valeur de _G["<<i<<"] = "<<_G[i] << std::endl;
#endif
  }


MEDMEM::FullInterlaceGaussPolicy::FullInterlaceGaussPolicy(const FullInterlaceGaussPolicy & policy, bool shallowcopie)
  : GSInterlacingPolicy(policy,shallowcopie)
 {
    //Seuls les tableaux de grande taille sont recopiés superficiellement
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);
 }

MEDMEM::FullInterlaceGaussPolicy &MEDMEM::FullInterlaceGaussPolicy::operator=(const FullInterlaceGaussPolicy & policy)
  {
    const char LOC[] = "FullInterlaceGaussPolicy operator =";
    BEGIN_OF_MED(LOC);

    if( this == &policy) return *this;

    //Seuls les tableaux de grande taille sont recopiés superficiellement
    GSInterlacingPolicy::operator=(policy);

    // Tableaux toujours recopiés par recopie profonde
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);
    return *this;
  }

MEDMEM::NoInterlaceGaussPolicy::NoInterlaceGaussPolicy(void)
  :GSInterlacingPolicy(),_cumul(0)
  { GSInterlacingPolicy::_gaussPresence=true; }

MEDMEM::NoInterlaceGaussPolicy::NoInterlaceGaussPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc, const int * const nbgaussgeo)
  : GSInterlacingPolicy(nbelem, dim, nbtypegeo,nbelgeoc,MED_EN::MED_NO_INTERLACE)
   {

     GInterlacingPolicy::_gaussPresence=true;

    _nbelegeoc.set(_nbtypegeo+1,nbelgeoc);
    _nbgaussgeo.set(_nbtypegeo+1,nbgaussgeo);
    _G.set(nbelem+1);
    //_G[0]       = 1;
    _ESE.set(nbelem+1);
    _ESE[0] = -1;
    int elemno = 0;

    _cumul = 0;
    for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
      for (int  i=0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) ; i++ ) {
        _G[ elemno ] = _cumul + i*nbgaussgeo[ntyp] + 1;
        elemno++;
        _ESE[ elemno ] = nbgaussgeo[ntyp];
      };
      _cumul += (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) * nbgaussgeo[ntyp];
#ifdef ARRAY_DEBUG
      std::cout << "Valeur de _cumul " << _cumul << std::endl;
#endif
    };

    _G[ elemno ] = _cumul+1;
    _arraySize   = ( _G[ elemno ] -1 ) * dim ;

#ifdef ARRAY_DEBUG
    for (int i =0; i< nbelem+1; i++ )
      std::cout << "Valeur de _G["<<i<<"] = "<<_G[i] << std::endl;
#endif
  }


MEDMEM::NoInterlaceGaussPolicy::NoInterlaceGaussPolicy(const NoInterlaceGaussPolicy & policy, bool shallowcopie)
  : GSInterlacingPolicy(policy,shallowcopie),_cumul(policy._cumul)
  {
    // Tableaux toujours recopiés par recopie profonde
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);
  }

MEDMEM::NoInterlaceGaussPolicy &MEDMEM::NoInterlaceGaussPolicy::operator=(const NoInterlaceGaussPolicy & policy)
  {
    if ( this == &policy) return *this;

    const char LOC[]= "NoInterlaceGaussPolicy operator =";
    BEGIN_OF_MED(LOC);
    GSInterlacingPolicy::operator=(policy);

    this->_cumul = policy._cumul;

    // Tableaux toujours recopiés par recopie profonde
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);

    return *this;
  }


MEDMEM::NoInterlaceByTypeGaussPolicy::NoInterlaceByTypeGaussPolicy(void)
  :GTInterlacingPolicy()
  { GTInterlacingPolicy::_gaussPresence=true; }

MEDMEM::NoInterlaceByTypeGaussPolicy::NoInterlaceByTypeGaussPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc, const int * const nbgaussgeo)
  : GTInterlacingPolicy(nbelem, dim, nbtypegeo, nbelgeoc,MED_EN::MED_NO_INTERLACE_BY_TYPE)
 {

    GTInterlacingPolicy::_gaussPresence=true;

    _nbelegeoc.set(_nbtypegeo+1,nbelgeoc);
    _nbgaussgeo.set(_nbtypegeo+1,nbgaussgeo);
    _G.set(_nbtypegeo+1);
    _T.set(nbelem+1);
    int elemno = 1;
    int cumul = 0;

    for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
      int nbelcurtype = nbelgeoc[ntyp]-nbelgeoc[ntyp-1];
      for (int i=0; i < nbelcurtype; i++ ) {
        _T[ elemno ] = ntyp;
        elemno++;
      };
      _G[ ntyp ] = cumul;
      cumul += nbelcurtype * _dim * nbgaussgeo[ntyp];
#ifdef ARRAY_DEBUG
      std::cout << "Valeur de cumul " << cumul << std::endl;
#endif
    };

    _arraySize = cumul;

#ifdef ARRAY_DEBUG
    for (int i =0; i< nbelem+1; i++ )
      std::cout << "Valeur de _T["<<i<<"] = "<<_T[i] << std::endl;
#endif
  }


MEDMEM::NoInterlaceByTypeGaussPolicy::NoInterlaceByTypeGaussPolicy(const NoInterlaceByTypeGaussPolicy & policy, bool shallowcopie)
  : GTInterlacingPolicy(policy,shallowcopie)
  {
    // Tableaux toujours recopiés par recopie profonde
    _nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);
  }

MEDMEM::NoInterlaceByTypeGaussPolicy & MEDMEM::NoInterlaceByTypeGaussPolicy::operator=(const NoInterlaceByTypeGaussPolicy & policy)
  {
    if ( this == &policy) return *this;
    const char LOC[] = "NoInterlaceGaussPolicy operator =";
    BEGIN_OF_MED(LOC);
    GTInterlacingPolicy::operator=(policy);
    // Tableaux toujours recopiés par recopie profonde
    this->_nbgaussgeo.set(_nbtypegeo+1,policy._nbgaussgeo);
    return *this;
  }

MEDMEM::NoInterlaceByTypeNoGaussPolicy::NoInterlaceByTypeNoGaussPolicy()
  :GTInterlacingPolicy() {}

MEDMEM::NoInterlaceByTypeNoGaussPolicy::NoInterlaceByTypeNoGaussPolicy(int nbelem, int dim)
  : GTInterlacingPolicy(nbelem,dim)
  {
    // constructor is incoorect for this type of interlace
    throw MEDEXCEPTION(LOCALIZED("Wrong constructor of NoInterlaceByTypeNoGaussPolicy "));
  }

MEDMEM::NoInterlaceByTypeNoGaussPolicy::NoInterlaceByTypeNoGaussPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc)
  : GTInterlacingPolicy(nbelem, dim, nbtypegeo,nbelgeoc,MED_EN::MED_NO_INTERLACE_BY_TYPE)
  {
    _nbelegeoc.set(_nbtypegeo+1,nbelgeoc);
    _G.set(nbtypegeo+1);
    _T.set(nbelem+1);
    int elemno = 1;
    int cumul = 0;

    for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
      int nbelcurtype = nbelgeoc[ntyp]-nbelgeoc[ntyp-1];
      for (int i=0; i < nbelcurtype; i++ ) {
        _T[ elemno ] = ntyp;
        elemno++;
      };
      _G[ ntyp ] = cumul;
      cumul += nbelcurtype*_dim;
#ifdef ARRAY_DEBUG
      std::cout << "Valeur de cumul " << cumul << std::endl;
#endif
    };

    _arraySize = cumul;

#ifdef ARRAY_DEBUG
    for (int i =0; i< nbelem+1; i++ )
      std::cout << "Valeur de _T["<<i<<"] = "<<_T[i] << std::endl;
#endif
  }

MEDMEM::NoInterlaceByTypeNoGaussPolicy::NoInterlaceByTypeNoGaussPolicy(const NoInterlaceByTypeNoGaussPolicy & policy, bool shallowcopie)
  : GTInterlacingPolicy(policy,shallowcopie) {}


MEDMEM::NoInterlaceByTypeNoGaussPolicy &MEDMEM::NoInterlaceByTypeNoGaussPolicy::operator=(const NoInterlaceByTypeNoGaussPolicy & policy)
  {
    if ( this == &policy) return *this;

    const char LOC[] = "NoInterlaceNoGaussPolicy operator =";
    BEGIN_OF_MED(LOC);
    GTInterlacingPolicy::operator=(policy);
    return *this;
  }

MEDMEM::GInterlacingPolicy::GInterlacingPolicy(void)
  :InterlacingPolicy(),_nbtypegeo(-1) {}

MEDMEM::GInterlacingPolicy::GInterlacingPolicy(int nbelem, int dim)
  : InterlacingPolicy(nbelem, dim) {}

MEDMEM::GInterlacingPolicy::GInterlacingPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc, int interlacing)
  : InterlacingPolicy(nbelem, dim, -1, interlacing),_nbtypegeo(nbtypegeo) {}

MEDMEM::GInterlacingPolicy::GInterlacingPolicy(const GInterlacingPolicy & policy, bool shallowcopie)
    : InterlacingPolicy(policy),_nbtypegeo(policy._nbtypegeo)
  {
    //Seuls les tableaux de grande taille sont recopiés superficiellement
    if(shallowcopie)
      { _G.set(policy._G); }
    else
      { _G.set(_nbtypegeo+1,policy._G); }
    // Tableaux toujours recopiés par recopie profonde
    _nbelegeoc.set(_nbtypegeo+1,policy._nbelegeoc);
  }

MEDMEM::GInterlacingPolicy & MEDMEM::GInterlacingPolicy::operator=(const GInterlacingPolicy & policy)
  {
    if (this == &policy) return *this;

    const char* LOC = "GInterlacingPolicy operator =";
    BEGIN_OF_MED(LOC);
    InterlacingPolicy::operator=(policy);
    _G.set(policy._G);

    // Tableaux toujours recopiés par recopie profonde
    _nbtypegeo=policy._nbtypegeo;
    _nbelegeoc.set(_nbtypegeo+1,policy._nbelegeoc);
    return *this;
  }

MEDMEM::GTInterlacingPolicy::GTInterlacingPolicy()
  :GInterlacingPolicy() {}

MEDMEM::GTInterlacingPolicy::GTInterlacingPolicy(int nbelem, int dim)
  : GInterlacingPolicy(nbelem,dim)
  {
    // constructor is incoorect for this type of interlace
    throw MEDEXCEPTION(LOCALIZED("Wrong constructor of GTInterlacingPolicy "));
  }

MEDMEM::GTInterlacingPolicy::GTInterlacingPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc, int interlacing)
  : GInterlacingPolicy(nbelem, dim, nbtypegeo,nbelgeoc,interlacing) {}

MEDMEM::GTInterlacingPolicy::GTInterlacingPolicy(const GTInterlacingPolicy & policy, bool shallowcopie)
    : GInterlacingPolicy(policy,shallowcopie)
  {
    //Seuls les tableaux de grande taille sont recopiés superficiellement
    if(shallowcopie)
      { _T.set(policy._T); }
    else
      { _T.set(_nbelem+1,policy._T); }
  }

MEDMEM::GTInterlacingPolicy &MEDMEM::GTInterlacingPolicy::operator=(const GTInterlacingPolicy & policy)
  {
    if ( this == &policy) return *this;

    const char LOC[] = "NoInterlaceNoGaussPolicy operator =";
    BEGIN_OF_MED(LOC);
    GInterlacingPolicy::operator=(policy);
    _T.set(policy._T);
    return *this;
  }

MEDMEM::GSInterlacingPolicy::GSInterlacingPolicy(void)
  :GInterlacingPolicy()
    { GInterlacingPolicy::_gaussPresence=true; }

MEDMEM::GSInterlacingPolicy::GSInterlacingPolicy(int nbelem, int dim, int nbtypegeo, const int * const nbelgeoc,int interlacing)
  : GInterlacingPolicy(nbelem, dim, nbtypegeo, nbelgeoc,interlacing) {}

MEDMEM::GSInterlacingPolicy::GSInterlacingPolicy(const GSInterlacingPolicy & policy, bool shallowcopie)
  : GInterlacingPolicy(policy,shallowcopie)
 {
   //Seuls les tableaux de grande taille sont recopiés superficiellement
    if(shallowcopie)
      { _ESE.set(policy._ESE); }
    else
      { _ESE.set(_nbelem+1,policy._ESE); }
 }

MEDMEM::GSInterlacingPolicy &MEDMEM::GSInterlacingPolicy::operator=(const GSInterlacingPolicy & policy)
  {
    const char LOC[] = "GSInterlacingPolicy operator =";
    BEGIN_OF_MED(LOC);

    if( this == &policy) return *this;

    //Seuls les tableaux de grande taille sont recopiés superficiellement
    GInterlacingPolicy::operator=(policy);
    _ESE.set(policy._ESE);

    return *this;
  }
