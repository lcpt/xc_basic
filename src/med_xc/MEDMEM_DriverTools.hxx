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

#ifndef DRIVERTOOLS_HXX
#define DRIVERTOOLS_HXX

#include "MEDMEM.hxx"

#include "MEDMEM_define.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_FieldForward.hxx"
#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_GaussLocalization.hxx"

#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <iostream>
#include <iomanip>
#include <algorithm>

// Way of storing values with gauss points by CASTEM: full/no interlace.
// Remove code switched by this symbol as soon as the way is found out,
// from here and from gibi driver
#define CASTEM_FULL_INTERLACE


namespace MEDMEM {
class MESH;
class CONNECTIVITY;
class COORDINATE;
class GROUP;
class FAMILY;
class FIELD_;
class _intermediateMED;

// ==============================================================================
struct MEDMEM_EXPORT _noeud
{
  mutable int number; // negative if node is merged
  std::vector<double> coord;
};

// ==============================================================================
typedef pair<int,int> _link; // a pair of node numbers

// ==============================================================================
struct MEDMEM_EXPORT _maille
{
  typedef std::map<int,_noeud>::iterator TNoeud; //iter;
  std::vector< TNoeud >      sommets;
  MED_EN::medGeometryElement geometricType;
  mutable bool               reverse; // to reverse sommets of a face
  mutable int*               sortedNodeIDs; // for comparison and merge
  //mutable list<unsigned>   groupes; // the GROUPs maille belongs to, used to create families

  _maille(MED_EN::medGeometryElement type=MED_EN::MED_NONE, size_t nelem=0)
    : geometricType(type),reverse(false),sortedNodeIDs(0),_ordre(0) { sommets.reserve(nelem); }

  _maille(const _maille& ma);
  void init() const { if ( sortedNodeIDs ) delete [] sortedNodeIDs; sortedNodeIDs = 0; }
  ~_maille() { init(); }

  int dimension() const // retourne la dimension de la maille
  { return geometricType/100; }

  int dimensionWithPoly() const // retourne la dimension de la maille
  { return geometricType >= MED_EN::MED_POLYGON ? dimension()-2 : dimension(); }

  const int* getSortedNodes() const; // creates if needed and return sortedNodeIDs
  bool operator < (const _maille& ma) const;
  MED_EN::medEntityMesh getEntity(const int meshDimension) const throw (MEDEXCEPTION);
  _link link(int i) const;

  //bool isMerged(int i) const { return sommets[i]->second.number < 0; }
  int nodeNum(int i) const { return abs( sommets[i]->second.number ); }
  int nodeID (int i) const { return sommets[i]->first; } // key in points map

  unsigned ordre() const { return abs( _ordre ); }
  bool isMerged() const { return _ordre < 0; }
  void setMergedOrdre(unsigned o) const { _ordre = -o; }
  void setOrdre(int o) const { _ordre = o; }

private:
  mutable int _ordre; // l'ordre est fixé après insertion dans le set, et ne change ni l'état, ni l'ordre -> mutable
  _maille& operator=(const _maille& ma);
};

// ==============================================================================
struct MEDMEM_EXPORT _mailleIteratorCompare // pour ordonner le set d'iterateurs sur mailles
{
  // this operator is used for ordering mailles thinin a group, which happens when
  // right numbers are already given to _maille's, so we can use _maille::ordre
  // for comparison instead of a heavy _maille::operator <
  bool operator () (std::set<_maille>::iterator i1, std::set<_maille>::iterator i2)
  {
    //return *i1<*i2;
    return i1->ordre() < i2->ordre();
  }
};

// ==============================================================================
struct MEDMEM_EXPORT _groupe
{
  typedef std::set<_maille>::iterator            TMaille;
  typedef std::vector< TMaille >::const_iterator TMailleIter;
  std::string            nom;
  std::vector< TMaille > mailles; // iterateurs sur les mailles composant le groupe
  std::vector<int>       groupes; // indices des sous-groupes composant le groupe
  std::map<unsigned,int> relocMap; // map _maille::ordre() -> index in GROUP, built by getGroups()
  GROUP*                 medGroup;

  std::vector<std::string> refNames; /* names of groups referring this one;
                                        refNames is resized according to nb of references
                                        while reading a group (pile 1) and it is filled with
                                        names while reading long names (pile 27); each named
                                        reference is converted into a copy of the medGroup
                                        (issue 0021311)
                                      */
  const _maille& maille(int index) { return *mailles[index]; }
  bool empty() const { return mailles.empty() && groupes.empty(); }
#ifdef WNT
  int  size()  const { return (mailles.size()>relocMap.size())?mailles.size():relocMap.size(); }
#else
  int  size()  const { return std::max( mailles.size(), relocMap.size() ); }
#endif
  _groupe():medGroup(0) {}
};

// ==============================================================================
struct MEDMEM_EXPORT _fieldBase {
  // a field contains several subcomponents each referring to its own support and
  // having several named components
  // ----------------------------------------------------------------------------
  struct _sub_data // a subcomponent
  // --------------------------------
  {
    int                      _supp_id;    // group index within _intermediateMED::groupes
    std::vector<std::string> _comp_names; // component names
    std::vector<int>         _nb_gauss;   // nb values per element in a component

    void setData( int nb_comp, int supp_id )
    { _supp_id = supp_id - 1; _comp_names.resize(nb_comp); _nb_gauss.resize(nb_comp,1); }
    int nbComponents() const { return _comp_names.size(); }
    std::string & compName( int i_comp ) { return _comp_names[ i_comp ]; }
    bool isValidNbGauss() const { return *std::max_element( _nb_gauss.begin(), _nb_gauss.end() ) ==
                                    *std::min_element( _nb_gauss.begin(), _nb_gauss.end() ); }
#ifdef WNT
    int nbGauss() const { return (1>_nb_gauss[0])?1:_nb_gauss[0]; }
#else
    int nbGauss() const { return std::max( 1, _nb_gauss[0] ); }
#endif
    bool hasGauss() const { return nbGauss() > 1; }
  };
  // ----------------------------------------------------------------------------
  std::vector< _sub_data > _sub;
  int                      _group_id; // group index within _intermediateMED::groupes
  // if _group_id == -1 then each subcomponent makes a separate MEDMEM::FIELD, else all subcomponents
  // are converted into one MEDMEM::FIELD. The latter is possible only if nb of components in all subs
  // is the same and supports of subcomponents do not overlap
  MED_EN::med_type_champ   _type;
  std::string              _name;
  std::string              _description;// field description

  _fieldBase( MED_EN::med_type_champ theType, int nb_sub )
    : _group_id(-1),_type(theType) { _sub.resize( nb_sub ); }
  virtual std::list<std::pair< FIELD_*, int> > getField(std::vector<_groupe>& groupes) const = 0;
  void getGroupIds( std::set<int> & ids, bool all ) const; // return ids of main and/or sub-groups
  bool hasCommonSupport() const { return _group_id >= 0; } // true if there is one support for all subs
  bool hasSameComponentsBySupport() const;
  virtual void dump(std::ostream&) const;
  virtual ~_fieldBase() {}
};

// ==============================================================================
template< class T > class _field: public _fieldBase
{
  std::vector< std::vector< T > > comp_values;
 public:
  _field< T > ( MED_EN::med_type_champ theType, int nb_sub, int total_nb_comp )
    : _fieldBase( theType, nb_sub ) { comp_values.reserve( total_nb_comp ); }
  std::vector< T >& addComponent( int nb_values ); // return a vector ready to fill in
  std::list<std::pair< FIELD_*, int> > getField(std::vector<_groupe>& groupes) const;
  virtual void dump(std::ostream&) const;
};

// ==============================================================================
/*!
 * \if developper
 * Iterator on set of _maille's of given dimension
 * \endif
 */
class MEDMEM_EXPORT _maillageByDimIterator
{
public:
  // if (convertPoly) then treat poly as 2d and 3d, else as 4d and 5d (=medGeometryElement/100)
  _maillageByDimIterator( const _intermediateMED & medi,
                          const int                dim=-1, // dim=-1 - for all dimensions
                          const bool               convertPoly = false )
  { myImed = & medi; init( dim, convertPoly ); }

  // if (convertPoly) then treat poly as 2d and 3d, else as 4d and 5d (=medGeometryElement/100)
  void init(const int  dim=-1, // dim=-1 - for all dimensions
            const bool convertPoly = false );

  //!< return next set of _maille's of required dimension
  const std::set<_maille > * nextType() {
    while ( myIt != myEnd )
      if ( !myIt->second.empty() && ( myDim < 0 || dim(false) == myDim ))
        return & (myIt++)->second;
      else
        ++myIt;
    return 0;
  }
  //!< return dimension of mailles returned by the last or further next()
  int dim(const bool last=true) const {
    iterator it = myIt;
    if ( last ) --it;
    return myConvertPoly ?
      it->second.begin()->dimensionWithPoly() :
      it->second.begin()->dimension();
  }
  //!< return type of mailles returned by the last next()
  MED_EN::medGeometryElement type() const { iterator it = myIt; return (--it)->first; }

  //!< return number of mailles taking into account merged ones
  int sizeWithoutMerged() const {
    iterator it = myIt;
    removed::const_iterator tNb = nbRemovedByType->find( (--it)->first );
    return it->second.size() - ( tNb == nbRemovedByType->end() ? 0 : tNb->second );
  }
private:
  typedef std::map<MED_EN::medGeometryElement, int >                removed;
  typedef std::map<MED_EN::medGeometryElement, std::set<_maille > > TMaillageByType;
  typedef TMaillageByType::const_iterator                                 iterator;

  const _intermediateMED* myImed;
  iterator myIt, myEnd;
  int myDim, myConvertPoly;
  const removed * nbRemovedByType;
};

// ==============================================================================
/*!
 * \if developper
 * Intermediate structure used by drivers to store data read from the other format file.
 * The structure provides functions that transform the stored data to the MED format :
 * getCoordinate(), getConnectivity(), getGroups().
 * The elements inserted in maillage and points are automaticaly ordered.
 * Renumbering are performed by getConnectivity & getGroups before writing the MED structures.
 * Read the conception ducumentation for more details.
 * \endif
 */
struct MEDMEM_EXPORT _intermediateMED
{
  typedef std::map<MED_EN::medGeometryElement, std::set<_maille > > TMaillageByType;
  typedef std::map<MED_EN::medGeometryElement, int >                TNbByType;
  typedef std::map< const _maille*, std::vector<int> >              TPolyherdalNbFaceNodes;

  TNbByType                nbRemovedByType; // nb mailles removed by merge
  std::vector<_groupe>     groupes;
  //std::vector<GROUP *>     medGroupes;
  std::map< int, _noeud >  points;
  std::list< _fieldBase* > fields;
  bool                     hasMixedCells; // true if there are groups with mixed entity types
  TPolyherdalNbFaceNodes   polyherdalNbFaceNodes; // nb of nodes in faces for each polyhedron

  inline _groupe::TMaille insert(const _maille& ma);

  int getMeshDimension() const;
  void mergeNodesAndElements(double tolerance); // optionally merge nodes and elements
  CONNECTIVITY * getConnectivity(); // creates MED connectivity from the intermediate structure
  COORDINATE * getCoordinate(const string & coordinateSystem="CARTESIAN"); // makes MED coordinate
//   void getFamilies(std::vector<FAMILY *> & _famCell, std::vector<FAMILY *> & _famFace,
//                    std::vector<FAMILY *> & _famEdge, std::vector<FAMILY *> & _famNode,
//                    MESH * _ptrMesh);
  void getGroups(std::vector<GROUP *> & _groupCell, std::vector<GROUP *> & _groupFace,
                 std::vector<GROUP *> & _groupEdge, std::vector<GROUP *> & _groupNode,
                 MESH * _ptrMesh);
  //GROUP * getGroup( int i );

  void getFields(std::list< FIELD_* >& fields);

  // used by previous functions to renumber points & mesh.
  bool myGroupsTreated;
  void treatGroupes(); // detect groupes of mixed dimension
  void numerotationMaillage();
  bool numerotationPoints(); // return true if renumeration done
  int nbMerged(int geoType) const; //!< nb nodes removed by merge

  _intermediateMED()
  { myNodesNumerated = myMaillesNumerated = myGroupsTreated = false; currentTypeMailles = 0; }
  ~_intermediateMED();

private:

  bool myNodesNumerated, myMaillesNumerated;
  // mailles groupped by geom type; use insert() for filling in and
  // _maillageByDimIterator for exploring it
  //std::set<_maille> maillage;
  TMaillageByType              maillageByType;
  TMaillageByType::value_type* currentTypeMailles; // for fast insertion
  friend class _maillageByDimIterator;
};
//-----------------------------------------------------------------------
_groupe::TMaille _intermediateMED::insert(const _maille& ma)
{
  if ( !currentTypeMailles || currentTypeMailles->first != ma.geometricType )
    currentTypeMailles = & *maillageByType.insert
      ( make_pair( ma.geometricType, std::set<_maille >())).first;

  _groupe::TMaille res = currentTypeMailles->second.insert( ma ).first;

  ((_maille&)ma).init(); // this method was added for the sake of this call which is needed to
  // remove comparison key (sortedNodeIDs) from a temporary _maille ma

  return res;
}

// ==============================================================================

std::ostream& operator << (std::ostream& , const _maille& );
std::ostream& operator << (std::ostream& , const _groupe& );
std::ostream& operator << (std::ostream& , const _noeud& );
std::ostream& operator << (std::ostream& , const _intermediateMED& );
std::ostream& operator << (std::ostream& , const _fieldBase* );

// ===========================================================
//                 field template implementation           //
// ===========================================================

template <class T>
  std::vector< T >& _field< T >::addComponent( int nb_values )
{
  comp_values.push_back( std::vector< T >() );
  std::vector< T >& res = comp_values.back();
  res.resize( nb_values );
  return res;
}

//=======================================================================
//function : getField
//purpose  : return list of pairs< field, supporting_group_id >
//=======================================================================

template <class T>
std::list<std::pair< FIELD_*, int> > _field< T >::getField(std::vector<_groupe> & groupes) const
{
  const char* LOC = "_field< T >::getField()";

  std::list<std::pair< FIELD_*, int> > res;

  // gauss array data
  int nbtypegeo = 0;
  vector<int> nbelgeoc(2,0), nbgaussgeo(2,0);

  int i_comp_tot = 0, nb_fields = 0;
  std::set<int> supp_id_set; // to create a new field when support repeats if hasCommonSupport()
  std::vector< _sub_data >::const_iterator sub_data, sub_end = _sub.end();

  _groupe*  grp = 0;
  GROUP* medGrp = 0;
  if ( hasCommonSupport() ) // several subs are combined into one field
  {
    grp    = & groupes[ _group_id ];
    medGrp = grp->medGroup;
    if ( !grp || grp->empty() || !medGrp || !medGrp->getNumberOfTypes())
      return res;

    // Make gauss array data
    nbtypegeo = medGrp->getNumberOfTypes();
    nbelgeoc  .resize( nbtypegeo + 1, 0 );
    nbgaussgeo.resize( nbtypegeo + 1, 0 );
    const int *                nbElemByType = medGrp->getNumberOfElements();
    const MED_EN::medGeometryElement* types = medGrp->getTypes();
    for (int iType = 0; iType < nbtypegeo; ++iType) {
      // nb elem by type
      nbelgeoc  [ iType+1 ] = nbelgeoc[ iType ] + nbElemByType[ iType ];
      // nb gauss by type; find a sub for a geo type
      for ( sub_data = _sub.begin(); sub_data != sub_end; ++sub_data ) {
        _groupe & sub_grp = groupes[ sub_data->_supp_id ];
        if ( !sub_grp.empty() && sub_grp.mailles[0]->geometricType == types[ iType ])
          break;
      }
      ASSERT_MED( sub_data != sub_end );
      nbgaussgeo[ iType+1 ] = sub_data->nbGauss();
    }
  }
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array TArrayNoGauss;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,Gauss>::Array   TArrayGauss;
  FIELD< T, FullInterlace > * f = 0;
  TArrayNoGauss * arrayNoGauss = 0;
  TArrayGauss   * arrayGauss = 0;

  // loop on subs of this field
  int i_sub = 1;
  for ( sub_data = _sub.begin(); sub_data != sub_end; ++sub_data, ++i_sub )
  {
    // nb values in a field
    if ( !hasCommonSupport() ) {
      grp    = & groupes[ sub_data->_supp_id ];
      medGrp = grp->medGroup;
    }
    int nb_val = grp->size();

    // check validity of a sub_data
    bool validSub = true;
    if ( !nb_val ) {
      INFOS_MED("Skip field <" << _name << ">: invalid supporting group "
            << (hasCommonSupport() ? _group_id : sub_data->_supp_id )
            << " of " << i_sub << "-th subcomponent" );
      validSub = false;
    }
    if ( !sub_data->isValidNbGauss() ) {
      INFOS_MED("Skip field <" << _name << ">: different nb of gauss points in components ");
      validSub = false;
    }
    if ( !validSub ) {
      if ( hasCommonSupport() ) {
        if ( !res.empty() ) {
          if(f)
            f->removeReference();
          res.clear();
        }
        return res;
      }
      i_comp_tot += sub_data->nbComponents();
      continue;
    }

    // Create a field

    if ( !f || !hasCommonSupport() || !supp_id_set.insert( sub_data->_supp_id ).second )
    {
      ++nb_fields;
      supp_id_set.clear();
      arrayNoGauss = 0;
      arrayGauss = 0;

      f = new FIELD< T, FullInterlace >();

      f->setNumberOfComponents( sub_data->nbComponents() );
      f->setComponentsNames( & sub_data->_comp_names[ 0 ] );
      f->setNumberOfValues ( nb_val );
      f->setName( _name );
      f->setDescription( _description );
      vector<string> str( sub_data->nbComponents() );
      f->setComponentsDescriptions( &str[0] );
      f->setMEDComponentsUnits( &str[0] );
      if ( !hasCommonSupport() && nb_fields > 1 )
      {
        f->setName( MEDMEM::STRING(_name) << "_Sub_" << nb_fields );
        INFOS_MED("Warning: field |" <<_name<<"| is incompatible with MED format (has "
                  "sub-fields of different nature), so we map its sub-field #"<< nb_fields <<
                  " into a separate field |"<<f->getName() << "|");
      }
      res.push_back( make_pair( f , hasCommonSupport() ? _group_id : sub_data->_supp_id ));
      MESSAGE_MED(" MAKE " << nb_fields << "-th field <" << _name << "> on group_id " << _group_id );

      // make an array
      if ( !sub_data->hasGauss() ) {
        arrayNoGauss = new TArrayNoGauss( sub_data->nbComponents(), nb_val );
        f->setArray( arrayNoGauss );
      }
      else {
        if ( !hasCommonSupport() ) {
          nbtypegeo = 1;
          nbelgeoc  [1] = nb_val;
          nbgaussgeo[1] = sub_data->nbGauss();
        }
        arrayGauss = new TArrayGauss(sub_data->nbComponents(), nb_val,
                                     nbtypegeo, & nbelgeoc[0], & nbgaussgeo[0]);
        f->setArray( arrayGauss );

        // PAL11040 "GIBI driver for Castem fields with Gauss point values"
        const MED_EN::medGeometryElement* types = medGrp->getTypes();
        for (int iGeom = 0; iGeom < nbtypegeo; ++iGeom) {
          ostringstream name;
          name << "Gauss_" << nbgaussgeo[iGeom+1] << "points_on" << types[iGeom] << "geom";
          GAUSS_LOCALIZATION_* loc = GAUSS_LOCALIZATION_::makeDefaultLocalization
            (name.str(), types[iGeom], nbgaussgeo[iGeom+1]);
          f->setGaussLocalization( types[iGeom], loc );
        }
      }
    }

    // Set values

    // get nb elements in a group
    _groupe & sub_grp = groupes[ sub_data->_supp_id ];
    int nb_supp_elems = sub_grp.mailles.size();
    int nb_gauss      = sub_data->nbGauss();
    MESSAGE_MED("insert sub data, group_id: " << sub_data->_supp_id <<
            ", nb values: "               << comp_values[ i_comp_tot ].size() <<
            ", relocMap size: "           << sub_grp.relocMap.size() <<
            ", nb mailles: "              << nb_supp_elems);

#ifdef CASTEM_FULL_INTERLACE
    const int gauss_step = 1;
    const int elem_step = nb_gauss;
#else
    const int gauss_step = nb_supp_elems;
    const int elem_step = 1;
#endif
    int i; // elem index
    // loop on components of a sub
    for ( int i_comp = 1; i_comp <= sub_data->nbComponents(); ++i_comp )
    {
      // store values
      const std::vector< T > & values = comp_values[ i_comp_tot++ ];
      bool oneValue = ( values.size() == 1 );
      ASSERT_MED( oneValue || (int)values.size() == nb_supp_elems * nb_gauss );
      for ( int k = 0; k < nb_supp_elems; ++k )
      {
        const T& val = oneValue ? values[ 0 ] : values[ k * elem_step ];
        const _maille& ma = sub_grp.maille( k );
        if ( medGrp->isOnAllElements() ) {
          i = ma.ordre();
        }
        else {
          std::map<unsigned,int>::const_iterator ordre_i = grp->relocMap.find( ma.ordre() );
          if ( ordre_i == grp->relocMap.end() )
            throw MEDEXCEPTION (LOCALIZED(STRING(LOC) << ", cant find elem index. "
                                          << k << "-th elem: " << ma));
          i = ordre_i->second;
        }
        if ( arrayNoGauss ) {
          arrayNoGauss->setIJ( i, i_comp, val );
        }
        else {
          const T* pVal = & val;
          for ( int iGauss = 1; iGauss <= nb_gauss; ++iGauss, pVal += gauss_step )
            arrayGauss->setIJK( i, i_comp, iGauss, *pVal);
        }
      }
    }
  } // loop on subs of the field

  return res;
}


// ==============================================================================
template <class T> void _field< T >::dump(std::ostream& os) const
// ==============================================================================
{
  _fieldBase::dump(os);
  os << endl;
  for ( int i = 0 ; i < (int)comp_values.size(); ++i )
  {
    os << "    " << i+1 << "-th component, nb values: " << comp_values[ i ].size() << endl;
  }
}

} // namespace MEDMEM

#endif /* DRIVERTOOLS_HXX */
