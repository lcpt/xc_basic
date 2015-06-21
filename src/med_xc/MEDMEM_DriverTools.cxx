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

#include "MEDMEM_DriverTools.hxx"
#include "MEDMEM_STRING.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Field.hxx"

#include <iomanip>
#include <algorithm>

using namespace std;
using namespace MED_EN;

#define DUMP_LINES_LIMIT 20

namespace MEDMEM {

// avoid coping sortedNodeIDs
_maille::_maille(const _maille& ma)
  : sommets(ma.sommets), geometricType(ma.geometricType), 
    reverse(ma.reverse), sortedNodeIDs(0), _ordre(ma._ordre)
{
}

// Cet opérateur permet d'ordonner les mailles dans un set suivant l'ordre requis par MED
bool _maille::operator < (const _maille& ma) const
{
  // si le type géométrique differe, la comparaison est basée dessus
  // sinon on se base sur une comparaison des numéros de sommets
  // we compare _mailles of only same geometricType due to maillageByType usage
  size_t l=sommets.size();
  if ( dimension() > 3 ) { // poly
    size_t l2 = ma.sommets.size();
    if ( l != l2 )
      return l < l2;
  }
  const int* v1 = getSortedNodes();
  const int* v2 = ma.getSortedNodes();
  for ( const int* vEnd = v1 + l; v1 < vEnd; ++v1, ++v2 )
    if(*v1 != *v2)
      return *v1 < *v2;
  return false; // cas d'égalité

//   if(geometricType==ma.geometricType)
//   {
//     // construction de deux vecteur temporaire contenant les numeros de sommets
//     // pour faire le tri et les comparaisons
//     size_t l=sommets.size();
//     if ( dimension() > 3 ) { // poly
//       size_t l2 = ma.sommets.size();
//       if ( l != l2 )
//         return l < l2;
//     }
//     std::vector<int> v1(l);
//     std::vector<int> v2(l);
//     for (unsigned int i=0; i!=l; ++i)
//     {
//       v1[i]=nodeNum(i);
//       v2[i]=ma.nodeNum(i);
//     }
//     std::sort(v1.begin(), v1.end());
//     std::sort(v2.begin(), v2.end());
//     for(std::vector<int>::const_iterator i1=v1.begin(), i2=v2.begin(); i1!=v1.end(); ++i1, ++i2)
//       if(*i1 != *i2)
//         return *i1 < *i2;
//     return false; // cas d'égalité
//   }
//   else
//     return geometricType<ma.geometricType;
}

// creates if needed and return sortedNodeIDs
const int* _maille::getSortedNodes() const
{
  if ( !sortedNodeIDs )
  {
    size_t l=sommets.size();
    sortedNodeIDs = new int[ l ];

    for (size_t i=0; i!=l; ++i)
      sortedNodeIDs[i]=nodeID(i);
    std::sort( sortedNodeIDs, sortedNodeIDs + l );
  }
  return sortedNodeIDs;
}

_link _maille::link(int i) const
{
  ASSERT_MED ( i >= 0 && i < (int)sommets.size() );
  int i2 = ( i + 1 == (int)sommets.size() ) ? 0 : i + 1;
  if ( reverse )
    return make_pair( sommets[i2]->first, sommets[i]->first );
  else
    return make_pair( sommets[i]->first, sommets[i2]->first );
}

// retourne l'entité d'une maille en fonction de la dimension du maillage.
MED_EN::medEntityMesh _maille::getEntity(const int meshDimension) const throw (MEDEXCEPTION)
{
  const char * LOC = "_maille::getEntity(const int meshDimension)" ;
//   BEGIN_OF_MED(LOC);

  int mailleDimension = this->dimensionWithPoly();
  medEntityMesh entity;
  if (mailleDimension == meshDimension)
    entity= MED_EN::MED_CELL;
  else
    switch (mailleDimension)
      {
      case 0 :
        entity = MED_EN::MED_NODE;
        break;
      case 1 :
        entity = MED_EN::MED_EDGE;
        break;
      case 2 :
        entity = MED_EN::MED_FACE;
        break;
      default :
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Impossible de determiner l'entite de la maille."));
      }
return entity;

//END_OF_MED(LOC);
}

void _maillageByDimIterator::init(const int dim, const bool convertPoly )
{
  myIt = myImed->maillageByType.begin();
  myEnd = myImed->maillageByType.end();
  myDim = dim;
  myConvertPoly = convertPoly;
  nbRemovedByType = & myImed->nbRemovedByType;
}

std::ostream& operator << (std::ostream& os, const _maille& ma)
{
    os << "maille " << ma.ordre() << " (" << ma.geometricType << ") : < ";
    os << ma.nodeNum(0);
    for( unsigned i=1; i!=ma.sommets.size(); ++i)
        os << ", " << ma.nodeNum( i );
    os << " > sortedNodeIDs: ";
    if ( ma.sortedNodeIDs ) {
      os << "< ";
      for( unsigned i=0; i!=ma.sommets.size(); ++i)
        os << ( i ? ", " : "" ) << ma.sortedNodeIDs[ i ];
      os << " >";
    }
    else {
      os << "NULL";
    }
    if ( ma.isMerged() )
      os << " MERGED ";
    return os;
}

std::ostream& operator << (std::ostream& os, const _groupe& gr)
{
    os << "--- Groupe " << gr.nom << " --- " << std::endl ;
    os << " -> liste des sous-groupes : ";
    for( std::vector<int>::const_iterator i=gr.groupes.begin(); i!=gr.groupes.end(); ++i)
            os << *i << " ";

    os << std::endl << " -> liste des "<< gr.mailles.size() << " mailles : " << std::endl;

    _groupe::TMailleIter i1=gr.mailles.begin();
    int l;
    for(l = 0; l < DUMP_LINES_LIMIT && i1!=gr.mailles.end(); i1++, l++)
            os << setw(3) << l+1 << " " << *(*i1) << std::endl;

    if ( l == DUMP_LINES_LIMIT )
      os << "   ... skip " << gr.mailles.size() - l << " mailles" << endl;

    os << " relocMap, size=" << gr.relocMap.size() << endl;
    map<unsigned,int>::const_iterator it = gr.relocMap.begin();
    for ( l = 0; l < DUMP_LINES_LIMIT && it != gr.relocMap.end(); ++it, ++l )
      os << " (" << it->first << "," << it->second << ")";
    if ( gr.relocMap.size() > 0 )
      os << endl;
    return os;
}

std::ostream& operator << (std::ostream& os, const _noeud& no)
{
    os << "noeud " << no.number << " : < ";
    std::vector<double>::const_iterator i=no.coord.begin();
    os << *i++ ;
    for( ; i!=no.coord.end(); ++i)
        os << ", " << *i;
    os << " >";
    return os;
}

void MEDMEM::_fieldBase::dump(std::ostream& os) const
{
  os << "field " << "<" << _name << ">" << endl <<
    "  nb sub: " << _sub.size() << endl <<
    "  group index: " << _group_id << endl <<
    "  type: " << _type << endl;
  os << "  subcomponents:" << endl;
  vector< _sub_data >::const_iterator sub_data = _sub.begin();
  for ( ; sub_data != _sub.end(); ++sub_data ) {
    os << "    group index: " << sub_data->_supp_id <<
      ", " << sub_data->nbComponents() << " comp names: ";
    for ( int i_comp = 0; i_comp < sub_data->nbComponents(); ++i_comp )
      os << " |" << sub_data->_comp_names[ i_comp ] << "|";
    os << endl;
  }
}

std::ostream& operator << (std::ostream& os, const _fieldBase * f)
{
  f->dump( os );
  return os;
}

std::ostream& operator << (std::ostream& os, const _intermediateMED& mi)
{
  int l;
  _maillageByDimIterator maIt( mi );
  while ( const set<_maille >* maillage = maIt.nextType() )
  {
    os << "Set des " << maillage->size()
       << " mailles of type " << maIt.type() << ": "<< std::endl;
    std::set<_maille>::const_iterator i=maillage->begin();
    for( l = 0; l < DUMP_LINES_LIMIT && i!=maillage->end(); ++i, ++l)
      os << setw(3) << l+1 << " " <<*i << std::endl;
    if ( l == DUMP_LINES_LIMIT )
      os << "   ... skip " << maillage->size() - l << " mailles" << endl;
  }
  os << std::endl << "Vector des " << mi.groupes.size() << " groupes : " << std::endl;
  for (unsigned int k=0; k!=mi.groupes.size(); k++)
    os << std::setw(3) << k << " " << mi.groupes[k] << std::endl;

  os << std::endl << "map des " << mi.points.size() << " noeuds : " << std::endl;
  std::map<int,_noeud>::const_iterator j=mi.points.begin();
  for( l = 0; l < DUMP_LINES_LIMIT && j!=mi.points.end(); ++j, ++l)
    os << j->second << std::endl;
  if ( l == DUMP_LINES_LIMIT )
    os << "   ... skip " << mi.points.size() - l << " noeuds" << endl;

  os << endl << mi.fields.size() << " fields:" << endl;
  std::list<_fieldBase* >::const_iterator fIt = mi.fields.begin();
  for ( l = 0; fIt != mi.fields.end(); ++fIt, ++l )
    os << " - " << l+1 << " " << *fIt << endl;

  return os;
}


//=======================================================================
//function : treatGroupes
//purpose  : detect groupes of mixed dimension and erase groupes that
//           won't be converted
//=======================================================================

void _intermediateMED::treatGroupes()
{
  const char * LOC = "_intermediateMED::treatGroupes() : ";
  BEGIN_OF_MED(LOC);

  if ( myGroupsTreated )
    return;
  myGroupsTreated = true;

  // --------------------
  // erase useless group
  // --------------------

  // decrease hierarchical depth of subgroups
  vector<int>::iterator j;
  for (unsigned int i=0; i!=this->groupes.size(); ++i)
  {
    _groupe& grp = groupes[i];
    //INFOS_MED( i << " " << grp.nom );
    j = grp.groupes.begin();
    while( j!=grp.groupes.end() ) {
      int grpInd = *j-1;
      if ( grpInd < 0 || grpInd >= (int)groupes.size() ) {
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Bad subgroup index: " << grpInd <<
                                     ", in " << i << " groupe.nom=" << grp.nom));
      }
      _groupe & sub_grp = groupes[ grpInd ];
      if ( !sub_grp.groupes.empty() ) {
        MESSAGE_MED("High hierarchical depth of subgroups in group " << i );
        *j = sub_grp.groupes[0]; // replace j with its 1st subgroup
        // push back the rest subs
        for ( int k = 1; k < (int)sub_grp.groupes.size(); ++k )
          grp.groupes.push_back( sub_grp.groupes[ k ]);
        // vector maybe is reallocated: restart iterator
        j = grp.groupes.begin();
      }
      else
        j++;
    }
    // remove empty sub-groupes
    j = grp.groupes.begin();
    while ( j!=grp.groupes.end() ) {
      if ( groupes[*j-1].empty() ) {
        grp.groupes.erase( j );
        j = grp.groupes.begin();
      }
      else
        j++;
    }
  }
  // get indices of groups that are field support -
  // do not erase them and their subgroups
  std::set<int> groups2convert;
  std::list< _fieldBase* >::const_iterator fIt = fields.begin();
  for ( ; fIt != fields.end(); ++fIt )
    (*fIt)->getGroupIds( groups2convert, true );

  // keep named groups and their subgroups
  for (unsigned int i=0; i!=this->groupes.size(); ++i)
  {
    _groupe& grp = groupes[i];
    if ( grp.empty() || grp.nom.empty() )
      continue;
    groups2convert.insert( i );
    for( j = grp.groupes.begin(); j!=grp.groupes.end(); ++j)
      groups2convert.insert( *j-1 );
  }
  // erase groups that are not in groups2convert
  for (unsigned int i=0; i!=this->groupes.size(); ++i)
  {
    if ( groups2convert.find( i ) == groups2convert.end() ) {
      _groupe& grp = groupes[i];
      grp.mailles.clear();
      grp.groupes.clear();
      MESSAGE_MED( "Erase " << i << "-th group " << grp.nom );
    }
  }

  // ---------------------------------------------------
  // define if there are groups with mixed entity types
  // ---------------------------------------------------

  hasMixedCells = false;
  for (unsigned int i=0; i!=this->groupes.size(); ++i)
  {
    _groupe& grp = groupes[i];
    if ( grp.groupes.empty() )
      continue;

    // check if sub-groups have different dimension
    j = grp.groupes.begin();
    int dim = groupes[*j-1].mailles[0]->dimension();
    for( j++; j!=grp.groupes.end(); ++j) {
      int dim2 = groupes[*j-1].mailles[0]->dimension();
      if ( dim != dim2 ) {
        if ( dim == 0 || dim2 == 0 || dim + dim2 == 9 ) {
          // cant create a group of nodes plus anything else,
          // neither a group of polygones + polyhedrons
          grp.mailles.clear();
          grp.groupes.clear();
          MESSAGE_MED( "Erase mixed dim group with nodes:" << i << "-th group " << grp.nom );
          break;
        }
        int d1 = dim  < 4 ? dim  : dim  - 2; // care of poly
        int d2 = dim2 < 4 ? dim2 : dim2 - 2;
        if ( d1 != d2 ) {
          hasMixedCells = true;
          MESSAGE_MED( "Mixed dim group: " << i << "-th " << grp.nom <<
                   "  dim1 = " << dim << " dim2 = " << dim2 );
        }
      }
    }
  }

//   if ( hasMixedCells )
//     INFOS_MED( "There will be groups of mixed dimention" );
  END_OF_MED(LOC);
}

void _intermediateMED::numerotationMaillage()
{
  const char* LOC = "_intermediateMED::numerotationMaillage() : ";
  BEGIN_OF_MED(LOC);
  if ( myMaillesNumerated )
    return;
  myMaillesNumerated = true;

  treatGroupes();

  set<_maille>::const_iterator i, iEnd;

  // numerotation mailles of type MED_POINT1 by node number
  bool hasPointMailles = false;
  if ( const set<_maille> * points = _maillageByDimIterator( *this, 0 ).nextType() ) {
    hasPointMailles = true;
    numerotationPoints();
    for ( i = points->begin(), iEnd = points->end(); i != iEnd; ++i )
      i->setOrdre( i->sommets[0]->second.number ); // is merged if point is merged
  }

  // loop on entities
  for ( int dim = 1; dim <= 3; ++dim )
  {
    int iterDim = hasMixedCells ? -1 : dim;
    const bool skipFirstType = ( hasPointMailles && hasMixedCells );

    // check if any numeration is present
    bool hasNumeration = true;
    _maillageByDimIterator entityMailles( *this, iterDim, true );
    if ( skipFirstType ) entityMailles.nextType();
    while ( const set<_maille> * typeMailles = entityMailles.nextType() ) {
      if ( typeMailles->begin()->ordre() == 0 || typeMailles->rbegin()->ordre() == 0 ) {
        hasNumeration = false;
        break;
      }
    }
    // check if re-numeration is needed
    bool ok = false, renumEntity = false;
    if ( hasNumeration )
    {
      ok = true;
      _maillageByDimIterator entityMailles( *this, iterDim, true );
      if ( skipFirstType ) entityMailles.nextType();

      int prevNbElems = 0;
      while ( const set<_maille> * typeMailles = entityMailles.nextType() )
      {
        unsigned minOrdre = INT_MAX, maxOrdre = 0;
        for ( i = typeMailles->begin(), iEnd = typeMailles->end(); i!=iEnd; ++i) {
          if ( i->ordre() < minOrdre ) minOrdre = i->ordre();
          if ( i->ordre() > maxOrdre ) maxOrdre = i->ordre();
        }
        unsigned typeSize = entityMailles.sizeWithoutMerged();
        if ( typeSize != maxOrdre - minOrdre + 1 )
          ok = false;
        if ( prevNbElems != 0 ) {
          if ( minOrdre == 1 )
            renumEntity = true;
          else if ( prevNbElems+1 != (int)minOrdre )
            ok = false;
        }
        prevNbElems += typeSize;
      }

      if ( ok && renumEntity ) // each geom type was numerated separately
      {
        entityMailles.init( iterDim, true );
        if ( skipFirstType ) entityMailles.nextType();
        prevNbElems = entityMailles.nextType()->size(); // no need to renumber the first type
        while ( const set<_maille> * typeMailles = entityMailles.nextType() ) {
          for ( i = typeMailles->begin(), iEnd = typeMailles->end(); i!=iEnd; ++i)
            i->setOrdre( i->ordre() + prevNbElems );
          prevNbElems += typeMailles->size();
        }
      }
    }
    if ( !ok )
    {
      int i_maille=0;
      entityMailles.init( iterDim, true );
      if ( skipFirstType ) entityMailles.nextType();
      while ( const set<_maille> * typeMailles = entityMailles.nextType() )
        for ( i = typeMailles->begin(), iEnd = typeMailles->end(); i!=iEnd; ++i)
          i->setOrdre( ++i_maille );
    }
  }
  END_OF_MED(LOC);
}

bool _intermediateMED::numerotationPoints()
{
  if ( !myNodesNumerated ) // is negative if numerated by merge
  {
    int i_noeud=0;
    for( map<int,_noeud>::iterator i=points.begin(); i!=points.end(); ++i)
      i->second.number = ++i_noeud ;
    myNodesNumerated = true;
    return true;
  }
  return false;
}

int _intermediateMED::nbMerged(int type) const //!< nb nodes removed by merge
{
  TNbByType::const_iterator typeNb = nbRemovedByType.find( type );
  return ( typeNb == nbRemovedByType.end() ? 0 : typeNb->second );
}


/*!
 * \if developper
 * create a MED COORDINATE from the intermediate structure.
 * \endif
 */
COORDINATE * _intermediateMED::getCoordinate(const string & coordinateSystem)
{
  const medModeSwitch mode= MED_EN::MED_FULL_INTERLACE;
    int spaceDimension=points.begin()->second.coord.size();
    int numberOfNodes=points.size() - nbMerged( MED_EN::MED_POINT1 );

    // creation du tableau des coordonnees en mode MED_FULL_INTERLACE
    double * coord = new double[spaceDimension*numberOfNodes];
    double * xyz = coord;
    for( std::map<int,_noeud>::const_iterator i=points.begin(); i!=points.end(); ++i )
      if ( i->second.number > 0 ) {
        std::copy(i->second.coord.begin(), i->second.coord.end(), xyz );
        xyz += spaceDimension;
      }

    // creation de l'objet COORDINATE
    COORDINATE * coordinate = new COORDINATE(spaceDimension, numberOfNodes, mode);
    coordinate->setCoordinates(mode,coord);
    delete [] coord;
    coordinate->setCoordinatesSystem(coordinateSystem);
    return coordinate;
}


/*!
 * \if developper
 * create a MED CONNECTIVITY from the intermediate structure.
 * \endif
 */
CONNECTIVITY * _intermediateMED::getConnectivity()
  {
    const char LOC[]= "_intermediateMED::getConnectivity() : ";
  BEGIN_OF_MED(LOC);

  int numberOfNodes=points.size() - nbMerged( MED_EN::MED_POINT1 ); // number of nodes in the mesh
  medEntityMesh entity= MED_ALL_ENTITIES;
  CONNECTIVITY *Connectivity = NULL, *Constituent = NULL;

  set<_maille>::const_iterator i, iEnd; // iterateurs sur les mailles

  // find out mesh dimension
  medGeometryElement meshDim= 0;
  _maillageByDimIterator allMailles( *this, -1, true );
  while ( allMailles.nextType() )
    meshDim = allMailles.dim();

  // renumerote les points de 1 a n (pour le cas ou certains points ne sont pas presents dans le maillage d'origine)
  numerotationPoints();

  // loop on entities
  for ( int dim = 0; dim <= 3; ++dim )
  {
    // skip nodes and elements of <dimension_maillage - 2> or less dimension
    // Unfortunately, it is impossible because of MESH::createFamilies() that requires
    // presence of connectivity even for nodes!
    //int dimension_maillage_moin_2=maillage.rbegin()->dimension() - 2;

    // tableau de travail : nombre d'elements pour chaque type geometrique
    vector<int> count;
    count.reserve( maillageByType.size() );
    count.push_back( 1 );

    // tableau de travail : stockage des types geometriques pour UNE entite
    vector<medGeometryElement> types;
    types.reserve( maillageByType.size() );

    // iterator returning mailles of each type of an entity,
    // but if hasMixedCells, we iterate on all types at every dim, since
    // in this case we store POINT1 elems as MED_EN::MED_NODE and
    // elems of all the rest types as MED_EN::MED_CELL
    int iterDim = hasMixedCells ? -1 : dim;
    _maillageByDimIterator entityMailles( *this, iterDim, /*convertPoly=*/true );

    // count nb of types and nb mailles of each type
    int dimension=0;
    if ( dim == 0 ) {
      if ( entityMailles.nextType() && entityMailles.dim() == 0 )
      {
        count.push_back( count.back() + numberOfNodes );
        types.push_back( entityMailles.type() );
      }
    }
    else {
      while ( entityMailles.nextType() )
      {
        //if ( entityMailles.dim() > 3 ) break; // ignore poly

        dimension = entityMailles.dim();
        if ( dimension == 0 ) continue; // if hasMixedCells, iterator returns all types

        count.push_back( count.back() + entityMailles.sizeWithoutMerged() );
        types.push_back( entityMailles.type() );
      }
    }
    int numberOfTypes = types.size(); // nombre de types de l'entite
    if ( numberOfTypes == 0 )
      continue;

    if ( dimension == meshDim ) entity=MED_EN::MED_CELL;
    else if (dimension==2 )     entity=MED_EN::MED_FACE;
    else if (dimension==1 )     entity=MED_EN::MED_EDGE;
    else if (dimension==0 )     entity=MED_EN::MED_NODE;

    Connectivity = new CONNECTIVITY ( numberOfTypes, entity );
    Connectivity->setEntityDimension( dimension );
    Connectivity->setNumberOfNodes  ( numberOfNodes );
    Connectivity->setGeometricTypes ( &types[0], entity);
    Connectivity->setCount          ( &count[0], entity );

    int prevNbElems = 1; // in previous type
    for (int k=0; k!=numberOfTypes; ++k )
    {
      set<_maille> & typeMailles = maillageByType[ types[k] ];
      i = typeMailles.begin(), iEnd = typeMailles.end();
      int nbMailles = count[k+1]-count[k];
      int* connectivity = 0, *index = 0;

      switch ( types[k] )
      {
      case MED_POLYGON:
        {
          // put polygones in order of increasing number
          vector<const _maille*> orderedPoly( nbMailles );
          for ( ; i != iEnd; ++i )
            if ( !i->isMerged() )
              orderedPoly[ i->ordre() - prevNbElems ] = &(*i);

          // make index
          int* polyIndex = index = new int[ nbMailles + 1 ];
          vector<const _maille*>::iterator poly = orderedPoly.begin(), polyEnd = orderedPoly.end();
          for ( *polyIndex++ = 1; polyIndex < index+nbMailles+1; ++poly, ++polyIndex)
            *polyIndex = polyIndex[-1] + (*poly)->sommets.size();

          // make connectivity
          int nbNodes = polyIndex[-1];
          int* conn = connectivity = new int[ nbNodes ];
          for ( poly = orderedPoly.begin(); poly != polyEnd; ++poly) {
            for ( int j = 0, nbNodes = (*poly)->sommets.size(); j < nbNodes; ++j )
              *conn++ = (*poly)->nodeNum( j );
          }
          break;
        }

      case MED_POLYHEDRA:
        {
          if ( typeMailles.size() != polyherdalNbFaceNodes.size() )
            throw MEDEXCEPTION (LOCALIZED(STRING(LOC) << "Missing info on polyhedron faces"));

          typedef TPolyherdalNbFaceNodes::iterator TPolyFaNoIter;
          TPolyFaNoIter polyFaNo, polyFaNoEnd = polyherdalNbFaceNodes.end();

          // put poly's in order of increasing number and count size of connectivity
          vector<TPolyFaNoIter> orderedPolyFaNo( nbMailles );
          int connSize = 0;
          for ( polyFaNo = polyherdalNbFaceNodes.begin(); polyFaNo != polyFaNoEnd; ++polyFaNo )
            if ( !polyFaNo->first->isMerged() )
            {
              orderedPolyFaNo[ polyFaNo->first->ordre() - prevNbElems ] = polyFaNo;
              connSize += polyFaNo->first->sommets.size() + polyFaNo->second.size() - 1;
            }
          vector<TPolyFaNoIter>::iterator pfnIter, pfnEnd = orderedPolyFaNo.end();

          // make index and connectivity
          int* conn = connectivity = new int[ connSize ];
          int* ind  = index        = new int[ nbMailles+1 ];
          *ind++ = 1;
          for ( pfnIter = orderedPolyFaNo.begin(); pfnIter != pfnEnd; ++pfnIter)
          {
            const _maille * poly = (*pfnIter)->first;
            const vector<int> & nbFaceNodes = (*pfnIter)->second;
            int nbNodes = 0;
            for ( unsigned iFace = 0; iFace < nbFaceNodes.size(); ++iFace )
            {
              for ( int j = 0, nbFNodes = nbFaceNodes[iFace]; j < nbFNodes; ++j )
                *conn++ = poly->nodeNum( nbNodes++ );
              *conn++ = -1;
            }
            conn--;
            *ind = ind[-1] + nbNodes;
            ++ind;
          }
          break;
        }

      default: // CLASSIC TYPES

        // copie des sommets dans connectivity et set dans Connectivity
        int nbSommetsParMaille = i->sommets.size();
        int nbSommets = nbMailles * nbSommetsParMaille;
        connectivity = new int[ nbSommets ];
        if ( entity==MED_EN::MED_NODE )
        {
          for (int l=0; l!=nbSommets; ++l)
            connectivity[l] = l+1;
        }
        else
        {
          for ( ; i != iEnd; ++i ) { // loop on elements of geom type
            if ( i->isMerged() )
              continue;
            int* mailleConn = connectivity + nbSommetsParMaille * ( i->ordre() - prevNbElems );
            if ( i->reverse )
              for ( int n=nbSommetsParMaille-1; n!=-1; --n)
                *mailleConn++ = i->nodeNum( n );
            else
              for ( int n=0; n != nbSommetsParMaille; ++n)
                *mailleConn++ = i->nodeNum( n );
          }
          // DO NOT ERASE, maillage will be used while fields construction
          //maillage.erase(j);    ; // dangereux, mais optimise la memoire consommee!
        }
      }

      Connectivity->setNodal (connectivity, entity, types[k], index);
      delete [] connectivity;
      delete [] index; index = 0;

      prevNbElems += nbMailles;
    }

    if ( Constituent )
      Connectivity->setConstituent (Constituent);
    // stocke Connectivity pour utilisation dans setConstituent lors de la boucle suivante
    Constituent = Connectivity;

    if ( entity == MED_EN::MED_CELL )
      break; // necessary if hasMixedCells
  }

  END_OF_MED(LOC);
  return Connectivity;
}


/*!
 * \if developper
 * fill the arguments vector of groups from the intermediate structure.
 * This function must be called before getConnectivity()
 * WARNING: new GROUP on all elements are invalid: numbers are not set! 
 * to make them valid it is necessary to update() them after setting
 * connectivity to mesh
 * \endif
 */
void _intermediateMED::getGroups(vector<GROUP *> & _groupCell,
                                 vector<GROUP *> & _groupFace,
                                 vector<GROUP *> & _groupEdge,
                                 vector<GROUP *> & _groupNode, MESH * _ptrMesh)
{
  const char* LOC = "_intermediateMED::getGroups() : ";
  BEGIN_OF_MED(LOC);

  //medGroupes.resize( groupes.size(), 0 );
  if (maillageByType.size() == 0) {
    INFOS_MED( "Erreur : il n'y a plus de mailles");
    return;
  }

  // get indices of groups that are field support - do not skip them
  std::set<int> support_groups;
  std::list< _fieldBase* >::const_iterator fIt = fields.begin();
  for ( ; fIt != fields.end(); ++fIt )
    (*fIt)->getGroupIds( support_groups, false );

  numerotationMaillage(); // Renumerotation des mailles par entite

  int dimension_maillage=getMeshDimension();

  for (size_t i=0; i!=this->groupes.size(); ++i)
  {
    _groupe& grp = groupes[i];
    if ( grp.medGroup /*medGroupes[ i ]*/ )
      continue; // grp already converted into a MED group

    bool isFieldSupport = ( support_groups.find( i ) != support_groups.end() );
    // si le groupe est vide, ou si le groupe n'est pas nommé : on passe au suivant
    if ( grp.empty() || ( grp.nom.empty() && !isFieldSupport )) {
      if ( !grp.nom.empty() ) {
        INFOS_MED("Skip group " << i << " " << grp.nom);
//       } else {
//         MESSAGE_MED("Skip group " << i << " <" << grp.nom << "> " << ( grp.empty() ? ": empty" : ""));
      }
      continue;
    }

    // Build a set of mailles: sort mailles by type and exclude maille doubling
    bool isSelfIntersect = false;
    typedef set< set<_maille>::iterator, _mailleIteratorCompare > TMailleSet;
    TMailleSet mailleSet;
    if( grp.groupes.size() ) {// le groupe i contient des sous-maillages
      int nb_elem = 0;
      for( vector<int>::iterator j=grp.groupes.begin(); j!=grp.groupes.end(); ++j)
      {
        nb_elem += groupes[*j-1].mailles.size();
        _groupe::TMailleIter maIt=groupes[*j-1].mailles.begin();
        for( ; maIt!=groupes[*j-1].mailles.end(); ++maIt) {
//           TMailleSet::const_iterator ma_it = mailleSet.find( *maIt );
//           if ( ma_it != mailleSet.end() ) {
//             MESSAGE_MED("EQUAL ELEMS: " << *ma_it << " AND " << *maIt);
//           }
//           else
            mailleSet.insert( *maIt );
        }
      }
      if ( nb_elem != (int)mailleSet.size() ) { // Self intersecting compound group
        isSelfIntersect = true;
        INFOS_MED("Self intersecting group: " << i << " <" << grp.nom << ">"
              << ", mailleSet.size = " << mailleSet.size() << ", sum nb elems = " << nb_elem);
        for( vector<int>::iterator j=grp.groupes.begin(); j!=grp.groupes.end(); ++j)
          INFOS_MED(" in sub-group "<<  *j << " <" << groupes[*j-1].nom << "> "
                << groupes[*j-1].mailles.size() << " mailles of type "
                << groupes[*j-1].mailles[0]->geometricType);
        // if a group serve as a support to a field, then the _field is to be converted
        // into several MEDMEM::FIELDs, one per sub-group; if we already skipped some sub-groups,
        // we roll back the loop on groups to create MED groups for skipped ones
        if ( isFieldSupport ) {
          if ( grp.nom.empty() ) // clear group existing for the sake of field only
            grp.groupes.clear();
          std::set<int> sub_grps;
          for ( fIt = fields.begin(); fIt != fields.end(); ++fIt ) {
            _fieldBase * field = (*fIt);
            if ( field->_group_id == (int)i ) {
              field->_group_id = -1; // -> a field by support
              field->getGroupIds( sub_grps, false );
            }
          }
          if ( (int)i > *sub_grps.begin() ) { // roll back
            support_groups.erase( i );
            support_groups.insert( sub_grps.begin(), sub_grps.end() );
            i = *sub_grps.begin() - 1;
            continue;
          }
        }
      }
    }
    else {
      _groupe::TMailleIter maIt=grp.mailles.begin();
      for(; maIt!=grp.mailles.end(); ++maIt)
        mailleSet.insert( *maIt );
      if ( grp.mailles.size() != mailleSet.size() )
        INFOS_MED( "Self intersecting group: " << i << " <" << grp.nom << ">"
               << ", mailleSet.size = " << mailleSet.size()
               << ", nb elems = " << grp.mailles.size());
    }
    // MEDMEM does not allow constituents of <mesh_dim>-2 and less dimension
    // but do not skip node group
    int group_min_dim = (**mailleSet.begin()).dimensionWithPoly();
    int group_max_dim = (**(--mailleSet.end())).dimensionWithPoly();
    if ( group_max_dim != 0 && group_min_dim <= dimension_maillage - 2  ) {
      MESSAGE_MED("Skip group: " << i << " <" << grp.nom << "> - too small dimension: "
              << group_min_dim);
      continue;
    }

    // initialise groupe_entity a l'entite de la premiere maille du groupe
    medEntityMesh groupe_entity = (**mailleSet.rbegin()).getEntity(dimension_maillage);
    if ( hasMixedCells && group_min_dim > 0 )
      groupe_entity = MED_EN::MED_CELL;

    // total nb of elements in mesh of groupe_entity
    int totalNbElements = 0;
    if ( groupe_entity == MED_EN::MED_NODE )
      totalNbElements = points.size() - nbMerged( MED_EN::MED_POINT1 );
    else {
      int entityDim = hasMixedCells ? -1 : group_min_dim;
      _maillageByDimIterator allMailles( *this, entityDim, true );
      while ( allMailles.nextType() )
        if ( allMailles.dim() > 0 )
          totalNbElements += allMailles.sizeWithoutMerged();
    }
    const bool isOnAll = ((int) mailleSet.size() == totalNbElements );

    // if !isOnAll, build a map _maille::ordre() -> index in GROUP.getNumber(MED_ALL_ELEMENTS).
    // It is used while fields building.
    if ( !isOnAll || isSelfIntersect || isFieldSupport ) {
      TMailleSet::iterator maIt = mailleSet.begin();
      for ( int iMa = 0; maIt != mailleSet.end(); maIt++ )
        grp.relocMap.insert( make_pair( (*maIt)->ordre(), ++iMa ));
    }
    //Parcours des mailles (a partir de la deuxieme) pour compter les types geometriques
    int nb_geometric_types=1;
    TMailleSet::iterator j=mailleSet.begin();
    medGeometryElement geometrictype=(**j).geometricType;
    for ( ++j ; j!=mailleSet.end(); ++j )
    {
      //Compte nombre de types geometriques
      if ( (**j).geometricType != geometrictype ) // si on change de type geometrique
      {
        nb_geometric_types++;
        geometrictype=(**j).geometricType;
      }
    }

    MED_EN::medGeometryElement * tab_types_geometriques = new MED_EN::medGeometryElement[nb_geometric_types];
    int * tab_index_types_geometriques = new int[nb_geometric_types+1];
    int * tab_numeros_elements = new int[mailleSet.size()];
    int * tab_nombres_elements = new int[nb_geometric_types];

    //Remplit tableaux entree des methodes set
    int indice_mailles=0/*, maxOrdre = -1*/;
    j=mailleSet.begin();
    geometrictype=(**j).geometricType;
    tab_index_types_geometriques[0]=1;
    int indice_types_geometriques=1;
    tab_types_geometriques[0]=geometrictype;
    //parcours des mailles du groupe
    for (  ; j!=mailleSet.end(); ++j , ++indice_mailles)
    {
      const _maille& ma = **j;
      tab_numeros_elements[indice_mailles]= ma.ordre();
//       if ( maxOrdre < tab_numeros_elements[indice_mailles] )
//         maxOrdre = tab_numeros_elements[indice_mailles];
      if (ma.geometricType != geometrictype)
      {
        tab_index_types_geometriques[indice_types_geometriques]=indice_mailles+1;
        geometrictype=ma.geometricType;
        tab_types_geometriques[indice_types_geometriques]=geometrictype;
        ++indice_types_geometriques;
      }
    }
    tab_index_types_geometriques[indice_types_geometriques]=indice_mailles+1;
    for (int k=0; k != nb_geometric_types; ++k)
    {
      tab_nombres_elements[k] = tab_index_types_geometriques[k+1]-tab_index_types_geometriques[k];
    }
    //INFOS_MED( "MAX ORDRE in grp " << grp.nom << " entity " << groupe_entity << " : " << maxOrdre);

    //Determination type entite du groupe
    vector <GROUP *> *vect_group= NULL;
    switch(groupe_entity )
      {
      case MED_EN::MED_CELL :
        vect_group= & _groupCell;
        break;
      case MED_EN::MED_FACE :
        vect_group= & _groupFace;
        break;
      case MED_EN::MED_EDGE :
        vect_group= & _groupEdge;
        break;
      case MED_EN::MED_NODE :
        vect_group= & _groupNode;
        break;
      }

    //Creation nouveau groupe MED
    GROUP * new_group = new GROUP();
    grp.medGroup = new_group;
    //Appel methodes set
    //new_group->setTotalNumberOfElements(mailleSet.size());
    new_group->setName(grp.nom);
    new_group->setMesh(_ptrMesh);
    if ( _ptrMesh )
      _ptrMesh->removeReference();
    new_group->setNumberOfGeometricType(nb_geometric_types);
    new_group->setGeometricType(tab_types_geometriques);
    new_group->setNumberOfElements(tab_nombres_elements);
    new_group->setNumber(tab_index_types_geometriques,tab_numeros_elements);
    new_group->setEntity(groupe_entity);
    new_group->setAll( isOnAll );

    vect_group->push_back(new_group);

    // Issue 0021311. Use case: a gibi group has references (recorded in pile 1)
    // and several names (pile 27) refer (pile 10) to this group.
    // We create a copy of this group per each named reference
    for ( unsigned iRef = 0 ; iRef < grp.refNames.size(); ++iRef )
      if ( !grp.refNames[ iRef ].empty() )
      {
        vect_group->push_back( new GROUP( *new_group ));
        vect_group->back()->setName( grp.refNames[ iRef ] );
      }

    delete [] tab_types_geometriques;
    delete [] tab_index_types_geometriques;
    delete [] tab_numeros_elements;
    delete [] tab_nombres_elements;
  }

  END_OF_MED(LOC);
}

//=======================================================================
//function : getFamilies
//purpose  : create families like MESH::createFamilies() but preserves
//           the order of elements in GROUPs defined by constituent families
//           order. Call it after getGroups()
//=======================================================================

// void _intermediateMED::getFamilies(std::vector<FAMILY *> & _famCell,
//                                    std::vector<FAMILY *> & _famFace,
//                                    std::vector<FAMILY *> & _famEdge,
//                                    std::vector<FAMILY *> & _famNode, MESH * _ptrMesh)
// {
//   const char * LOC = "_intermediateMED::getFamilies() : ";
//   BEGIN_OF_MED(LOC);

//   int nbElemFam = 0, nbNodeFam = 0;
//   std::map< GROUP*, vector< FAMILY * > > grpFamsMap;
//   int dimension_maillage=maillage.rbegin()->dimension();

//   std::set<_maille>::const_iterator i=maillage.begin(); // iterateurs sur les mailles
//   std::set<_maille>::const_iterator j=maillage.begin();

//   do
//   {
//     // make a family containing mailles shared by the same set of groups
//     std::list<unsigned>&  grList = i->groupes;  // to define the family end
//     int           dimension = i->dimension();        // to define the entity end
//     medGeometryElement type = i->geometricType;
//     medEntityMesh    entity = i->getEntity( dimension_maillage );

//     std::vector<medGeometryElement> tab_types_geometriques;
//     std::vector<int> tab_index_types_geometriques;
//     std::vector<int> tab_nombres_elements;
//     std::vector<int> tab_numeros_elements;

//     int iMa = 1, nbtype = 0;
//     tab_types_geometriques.push_back( type );
//     tab_index_types_geometriques.push_back( iMa );

//     // scan family cells and fill the tab that are needed by the create a MED FAMILY
//     while (i != maillage.end() &&
//            i->groupes == grList &&
//            i->dimension() == dimension)
//     {
//       if (type != i->geometricType) // si changement de type geometrique
//       {
//         tab_index_types_geometriques.push_back(iMa);
//         tab_nombres_elements.push_back(nbtype);
//         nbtype=0;
//         type=i->geometricType;
//         tab_types_geometriques.push_back(type); // stocke le nouveau type geometrique rencontre
//       }
//       ++nbtype;
//       ++iMa;
//       ++i;
//     }
//     tab_index_types_geometriques.push_back(iMa);
//     tab_nombres_elements.push_back(nbtype); // n'a pas été stocké dans la boucle

//     tab_numeros_elements.resize( iMa - 1 );
//     for ( iMa = 0; j != i; j++, iMa++ )
//       tab_numeros_elements[ iMa ] = j->ordre;

//     int id = ( entity == MED_EN::MED_NODE ? ++nbNodeFam : -(++nbElemFam) );

//     ostringstream name;
//     name << "FAM_" << id;

//     // create a empty MED FAMILY and fill it with the tabs we constructed
//     FAMILY* newFam = new FAMILY();
//     newFam->setTotalNumberOfElements( iMa );
//     newFam->setName( name.str() );
//     newFam->setMesh( _ptrMesh );
//     newFam->setNumberOfGeometricType( tab_types_geometriques.size() );
//     newFam->setGeometricType( &tab_types_geometriques[0] ); // we know the tab is not empy
//     newFam->setNumberOfElements( &tab_nombres_elements[0] );
//     newFam->setNumber( &tab_index_types_geometriques[0], &tab_numeros_elements[0] );
//     newFam->setEntity( entity );
//     newFam->setAll( false );
//     newFam->setIdentifier( id );
//     newFam->setNumberOfGroups( grList.size() );

//     // Update links between families and groups
//     if ( ! grList.empty() )
//     {
//       std::string * groupNames = new string[ grList.size() ];
//       std::list<unsigned>::iterator g = grList.begin();
//       for ( int i = 0; g != grList.end(); ++g, ++i ) {
//         GROUP * medGROUP = getGroup( *g );
//         groupNames[ i ] = medGROUP->getName();
//         grpFamsMap[ medGROUP ].push_back( newFam );
//       }
//       newFam->setGroupsNames(groupNames);
//     }
//     // store newFam
//     std::vector<FAMILY*>* families = 0;
//     switch ( entity )
//     {
//     case MED_EN::MED_CELL :
//       families = & _famCell; break;
//     case MED_EN::MED_FACE :
//       families = & _famFace; break;
//     case MED_EN::MED_EDGE :
//       families = & _famEdge; break;
//     case MED_EN::MED_NODE :
//       families = & _famNode; break;
//     }
//     if ( families )
//       families->push_back( newFam );

//   } while ( i != maillage.end() );

//   // update references in groups
//   std::map< GROUP*, vector< FAMILY * > >::iterator gf = grpFamsMap.begin();
//   for ( ; gf != grpFamsMap.end(); ++gf )
//   {
//     gf->first->setNumberOfFamilies( gf->second.size() );
//     gf->first->setFamilies( gf->second );
//   }
//}

//=======================================================================
//function : getGroup
//purpose  :
//=======================================================================

// GROUP * _intermediateMED::getGroup( int i )
// {
//   if ( i <(int) medGroupes.size() )
//     return medGroupes[ i ];
//   throw MEDEXCEPTION
//     (LOCALIZED(STRING("_intermediateMED::getGroup(): WRONG GROUP INDEX: ")
//                << medGroupes.size() << " <= " << i ));
// }

//=======================================================================
//function : getFields
//purpose  :
//=======================================================================

void _intermediateMED::getFields(std::list< FIELD_* >& theFields)
{
  const char * LOC = "_intermediateMED::getFields() : ";
  BEGIN_OF_MED(LOC);

  std::list< _fieldBase* >::const_iterator fIt = fields.begin();
  for ( ; fIt != fields.end(); fIt++ )
  {
    const _fieldBase* fb = *fIt;
    list<pair< FIELD_*, int> >  ff = fb->getField(groupes);
    list<pair< FIELD_*, int> >::iterator f_sup = ff.begin();
    for (int j = 1 ; f_sup != ff.end(); f_sup++, ++j )
    {
      FIELD_    * f = f_sup->first;
      SUPPORT * sup = groupes[ f_sup->second ].medGroup;
      if ( !sup )
        throw MEDEXCEPTION
          (LOCALIZED(STRING(LOC) <<"_intermediateMED::getFields(), NULL field support: "
                     << " group index: " << fb->_group_id));
      int nb_elems = sup->getNumberOfElements( MED_ALL_ELEMENTS );
      if ( nb_elems != f->getNumberOfValues() )
        throw MEDEXCEPTION
          (LOCALIZED(STRING("_intermediateMED::getFields(), field support size (")
                     << nb_elems  << ") != NumberOfValues (" << f->getNumberOfValues()));
      theFields.push_back( f );
      if ( sup->getName().empty() ) {
        ostringstream name;
        name << "GRP_" << f->getName() << "_" << j;
        sup->setName( name.str() );
      }
      f->setSupport( sup );
      //f->setIterationNumber( j );
      f->setOrderNumber( j );
    }
  }
  END_OF_MED(LOC);
}

//=======================================================================
//function : ~_intermediateMED
//purpose  : destructor
//=======================================================================

_intermediateMED::~_intermediateMED()
{
  MESSAGE_MED( "~_intermediateMED()");
  std::list< _fieldBase* >::const_iterator fIt = fields.begin();
  for ( ; fIt != fields.end(); fIt++ )
    delete *fIt;
}

//=======================================================================
//function : getGroupIds
//purpose  : return ids of main and/or sub- groups
//=======================================================================

void _fieldBase::getGroupIds( std::set<int> & ids, bool all ) const
{
  if ( hasCommonSupport() )
    ids.insert( _group_id );
  if ( all || !hasCommonSupport() ) {
    vector< _sub_data >::const_iterator sub = _sub.begin();
    for ( ; sub != _sub.end(); ++sub )
      ids.insert( sub->_supp_id );
  }
}

//=======================================================================
//function : hasSameComponentsBySupport
//purpose  :
//=======================================================================

bool _fieldBase::hasSameComponentsBySupport() const
{
  vector< _sub_data >::const_iterator sub_data = _sub.begin();
  const _sub_data& first_sub_data = *sub_data;
  for ( ++sub_data ; sub_data != _sub.end(); ++sub_data )
  {
    if ( first_sub_data._comp_names != sub_data->_comp_names )
      return false; // diff names of components

    if ( first_sub_data._nb_gauss != sub_data->_nb_gauss )
      return false; // diff nb of gauss points
  }
  return true;
}

//=======================================================================
//
//function : mergeNodes
//
//=======================================================================
namespace {

struct __NOEUD
{
  typedef map<int,_noeud>::iterator Inoeud;
  Inoeud i_noeud;

  __NOEUD() {}
  __NOEUD(Inoeud n): i_noeud(n) {}
  const double & operator[] (int i) const { return i_noeud->second.coord[i]; }
  double         operator[] (int i)       { return i_noeud->second.coord[i]; }
  int dim() const { return i_noeud->second.coord.size(); }
  int& num() { return i_noeud->second.number; }
  int id() const { return i_noeud->first; }
  bool isMerged() const { return i_noeud->second.number < 1; }
};
//-----------------------------------------------------------------------
double DistanceL2(const __NOEUD &A,const __NOEUD &B)
{
  if ( B.isMerged() ) return DBL_MAX;
  const double* cooA = &A[0], *cooB = &B[0], *cooEnd = cooA + A.dim();
  double dist, somme=0;
  while ( cooA < cooEnd ) {
    dist=((*cooA++) - (*cooB++));
    somme+=dist*dist;
  }
  return sqrt(somme);
}
//-----------------------------------------------------------------------
struct __NUAGENOEUD
{
  vector< __NOEUD > nodes;
  __NUAGENOEUD(_intermediateMED& imed);
  __NOEUD & operator [] (int i) { return nodes[i]; }
  int size() const { return nodes.size(); }
};
//-----------------------------------------------------------------------
__NUAGENOEUD::__NUAGENOEUD(_intermediateMED& imed)
{
  nodes.resize( imed.points.size() );
  map<int,_noeud>::iterator i_noeud = imed.points.begin();
  for( int i = 0; i_noeud!=imed.points.end(); ++i_noeud, ++i ) {
    i_noeud->second.number = i+1;
    nodes[ i ] = i_noeud;
  }
}
//-----------------------------------------------------------------------
// mergeNodes
template<int DIM> int mergeNodes(double            tolerance,
                                 _intermediateMED& imed,
                                 vector< int > &   /*newNodeIDs*/)
{
  /*typedef dTree<__NOEUD,__NUAGENOEUD,DIM > DTree;
  __NUAGENOEUD aNUAGENOEUD( imed );
  DTree tree( &aNUAGENOEUD );

//   int maxID = imed.points.rbegin()->first;
//   newNodeIDs.resize( maxID + 1, 0 );

  int num = 0, nbRemoved = 0;
  int nbNodes = aNUAGENOEUD.size();
  for ( int i = 0; i < nbNodes; ++i )
  {
    __NOEUD & node = aNUAGENOEUD[i];
    int & number = node.num();
    if ( number < 0 )
      continue; // already merged
    number = ++num;

    list<int> closeNumbers;
    int nbCoinsident = tree.get_all_close( node, tolerance, closeNumbers );
    if ( nbCoinsident > 1 ) // not only node it-self found
    {
      nbRemoved += nbCoinsident-1; // one of coincident nodes remains
      int id = node.id();
      list<int>::iterator n = closeNumbers.begin(), nEnd = closeNumbers.end();
      while ( n != nEnd ) {
        __NOEUD & coincNode = aNUAGENOEUD[ *n++ ];
        int coincID = coincNode.id();
        if ( coincID != id ) {
          coincNode.num() = -number;
          //newNodeIDs[ coincID ] = id;
        }
      }
    }
  }
  return nbRemoved;*/
  return 0;
}
//-----------------------------------------------------------------------
// wrapper of _maille used after merging nodes to find equal mailles
struct _mailleWrap
{
  const _maille* _ma;

  _mailleWrap(const _maille* ma=0): _ma(ma) { if (_ma) _ma->init(); }
  ~_mailleWrap()                            { if (_ma) _ma->init(); }

  bool operator < (const _mailleWrap& mw) const
  {
    size_t l=_ma->sommets.size();
    const int* v1 = getSortedNodeNums(_ma);
    const int* v2 = getSortedNodeNums(mw._ma);
    for ( const int* vEnd = v1 + l; v1 < vEnd; ++v1, ++v2 )
      if(*v1 != *v2)
        return *v1 < *v2;
    return false; // cas d'égalité
  }
  static const int* getSortedNodeNums(const _maille* ma)
  {
    if ( !ma->sortedNodeIDs ) {
      size_t l=ma->sommets.size();
      ma->sortedNodeIDs = new int[ l ];
      for (size_t i=0; i!=l; ++i)
        ma->sortedNodeIDs[i]=ma->nodeNum(i);
      std::sort( ma->sortedNodeIDs, ma->sortedNodeIDs + l );
    }
    return ma->sortedNodeIDs;
  }
};

} // namespace

//=======================================================================
//function : mergeNodesAndElements
//purpose  : optionally merge nodes and elements
//=======================================================================

void _intermediateMED::mergeNodesAndElements(double tolerance)
{
  //const char * LOC = "_intermediateMED::mergeNodesAndElements(): ";
  vector< int > newNodeIDs; // for i-th node id, id to replace with

  int nbRemovedNodes = 0;
  const int spaceDimension=points.begin()->second.coord.size();
  if ( spaceDimension == 3 )
    nbRemovedNodes = mergeNodes<3>( tolerance, *this, newNodeIDs );
  else if ( spaceDimension == 2 )
    nbRemovedNodes = mergeNodes<2>( tolerance, *this, newNodeIDs );

  myNodesNumerated = true;

  if ( nbRemovedNodes == 0 )
    return;

  // all commented code here was used to keep initial numeration but it was too slow
  //numerotationMaillage();
  treatGroupes();

  nbRemovedByType[ MED_EN::MED_NONE   ] = nbRemovedNodes;
  nbRemovedByType[ MED_EN::MED_POINT1 ] = nbRemovedNodes;

  bool hasPointMailles = false;
  _maillageByDimIterator entityMailles( *this, 0 );
  if ( const set<_maille> * points = entityMailles.nextType() ) {
    hasPointMailles = true;
    set<_maille>::const_iterator i, iEnd = points->end();
    for ( i = points->begin(); i != iEnd; ++i )
      i->setOrdre( i->sommets[0]->second.number ); // is merged if point is merged
  }
  const bool skipFirstType = ( hasPointMailles && hasMixedCells );
  // loop on entities
  for ( int dim = 1; dim <= 3; ++dim )
  {
    int iterDim = hasMixedCells ? -1 : dim;
    //int nbRemovedInEntity = 0;

    // count total nb of mailles in entity
//     int nbMailles = 0;
//     entityMailles.init( iterDim, true );
//     if ( skipFirstType ) entityMailles.nextType(); // merged POINT1's are same as nodes
//     while ( const set<_maille> * typeMailles = entityMailles.nextType() )
//       nbMailles += typeMailles->size();

    // for each maille number, count shift due to removed mailles with lower numbers
    //vector<int> shift( nbMailles+1, 0 );

    // merge and numerate mailles
    int num = 0;
    entityMailles.init( iterDim, true );
    if ( skipFirstType ) entityMailles.nextType(); // merged POINT1's are same as nodes
    while ( const set<_maille> * typeMailles = entityMailles.nextType() )
    {
      int nbRemoved = 0;
      set<_mailleWrap> maNodeNumSorted;
      pair< set<_mailleWrap>::const_iterator, bool > mw_isunique;

      set<_maille>::const_iterator ma = typeMailles->begin(), maEnd = typeMailles->end();
      while ( ma != maEnd )
      {
        const _maille & m = *ma++;
        mw_isunique = maNodeNumSorted.insert( &m );
        if ( mw_isunique.second ) {
          m.setOrdre( ++num );
        }
        else {
          const _maille* equalMa = mw_isunique.first->_ma;
          //unsigned ordreToRemove = m.ordre();
          m.setMergedOrdre( equalMa->ordre() );
          nbRemoved++;
//           while ( ordreToRemove <= nbMailles )
//             shift[ ordreToRemove++ ]++;
        }
      }

      if ( nbRemoved ) {
        nbRemovedByType[ entityMailles.type() ] = nbRemoved;
        //nbRemovedInEntity += nbRemoved;
      }

      // update maille ordre
//       if ( nbRemovedInEntity ) {
//         for ( ma = typeMailles->begin(); ma != maEnd; ++ma ) {
//           unsigned newOrdre = ma->ordre() - shift[ ma->ordre() ];
//           if ( ma->isMerged() )
//             ma->setMergedOrdre( newOrdre );
//           else
//             ma->setOrdre( newOrdre );
//         }
//       }
    }
  }

  myMaillesNumerated = true;

}

//=======================================================================
//function : getMeshDimension
//purpose  : count mesh dimension
//=======================================================================

int _intermediateMED::getMeshDimension() const
{
  int dim = 0;
  _maillageByDimIterator allMailles( *this, -1, true );
  while ( allMailles.nextType() )
    dim = allMailles.dim();
  return dim;
}

}

/////
