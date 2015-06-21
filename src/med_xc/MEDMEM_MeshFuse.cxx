// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File      : MEDMEM_MeshFuse.cxx
// Created   : Tue Jul  7 18:27:00 2009
// Author    : Edward AGAPOV (eap)

#include "MEDMEM_MeshFuse.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"

using namespace MEDMEM;
using namespace MED_EN;
using namespace std;

#define _NODE_TYPE_ MED_EN::MED_NONE

MeshFuse::MeshFuse():MESHING()
{
}

MeshFuse::~MeshFuse()
{
}

//================================================================================
/*!
 * \brief set global numbers of nodes if MeshFuse has been filled via MESHING
 */
//================================================================================

void MeshFuse::setNodeNumbers( const std::vector<int>& node_glob_numbers )
{
  const char* LOC = "MeshFuse::setNodeNumbers(node_glob_numbers): ";

  if ( !_node_glob_numbers.empty() )
    throw MEDEXCEPTION(STRING(LOC)<<"node numbers has been already set");

  if ( node_glob_numbers.size() != getNumberOfNodes() &&
       node_glob_numbers.size() > 0 && getNumberOfNodes() > 0 )
    throw MEDEXCEPTION
      (STRING(LOC)<<"size of node_glob_numbers must be equal number of nodes in MeshFuse");

  _node_glob_numbers = node_glob_numbers;
}

//================================================================================
/*!
 * \brief Fuse me and other mesh
 *  \param mesh - mesh to concatenate
 *  \param node_glob_numbers - node numbers used to merge coincident meshes entities
 */
//================================================================================

void MeshFuse::concatenate( const MESH* mesh, const vector<int>& node_glob_numbers )
{
  const char* LOC = "MeshFuse::concatenate( mesh, node_glob_numbers ): ";
  if ( !mesh || mesh->getNumberOfNodes() == 0) return;
    
  _mesh = mesh;

  if ( this->getNumberOfNodes() < 1 )
  {
    mesh->getCoordinates( MED_EN::MED_FULL_INTERLACE );// make fields filled in case of GRID
    mesh->getConnectivityptr();

    // just copy mesh
    static_cast<MESH&>(*this) = *mesh;

    _node_glob_numbers = node_glob_numbers;
    return;
  }
  // check feasibility

  if ( mesh->getNumberOfNodes() > 0 && node_glob_numbers.empty() )
    throw MEDEXCEPTION(STRING(LOC)<<"merging without node global numbers not implemented yet");

  if ( mesh->getNumberOfNodes() != node_glob_numbers.size() )
    throw MEDEXCEPTION(STRING(LOC)<<"invalid number of node global numbers");

  if ( mesh->getSpaceDimension() != this->getSpaceDimension() ||
       mesh->getMeshDimension()  != this->getMeshDimension() )
    throw MEDEXCEPTION(STRING(LOC)<<"can't unite meshes with different dimensions so far, sorry");
  
  // init
  _merged_of_type.clear();
  for ( int i = 0; i < NB_INDICES; ++i )
  {
    _nb_index[ i ].clear();
    _nb_index[ i ].resize( MED_ALL_ENTITIES );
  }

  // concatenation

  int final_nb_nodes = makeNewNodeIds( node_glob_numbers );

  expandCoordinates(final_nb_nodes);

  expandConnectivity(final_nb_nodes);

  expandSupports();

  // clear unnecessary data
  _new_elem_ids_of_type.clear();
}

//================================================================================
/*!
 * \brief Return number of nodes in the expanded mesh
 *  \param add_glob_numbers - global ids of nodes to append
 */
//================================================================================

int MeshFuse::makeNewNodeIds(const vector<int>& add_glob_numbers)
{
  // remember merged added nodes and make an array of new ids of added nodes
  vector<int>&       merged = _merged_of_type      [ _NODE_TYPE_ ];
  vector<int>& new_node_ids = _new_elem_ids_of_type[ _NODE_TYPE_ ];
  new_node_ids.resize( add_glob_numbers.size() );

  // extand global node numbers
  _node_glob_numbers.reserve( _node_glob_numbers.size() + add_glob_numbers.size());

  map<int,int> glob_ids; // to map global ids to new local ones
  for ( int i = 0; i < _node_glob_numbers.size(); ++i )
    glob_ids.insert( make_pair( _node_glob_numbers[i], i+1 ));

  int next_loc_id = getNumberOfNodes() + 1;

  for ( int i = 0; i < add_glob_numbers.size(); ++i )
  {
    map<int,int>::iterator  glob_loc =
      glob_ids.insert( make_pair( add_glob_numbers[i], next_loc_id )).first;

    new_node_ids[i] = glob_loc->second;

    if ( new_node_ids[i] == next_loc_id ) // unique global number
    {
      next_loc_id++;
      _node_glob_numbers.push_back( add_glob_numbers[i] );
    }
    else
    {
      merged.push_back( i );
    }
  }
  _nb_index[ INIT_OLD ][ MED_EN::MED_NODE ][ _NODE_TYPE_ ] = getNumberOfNodes();
  _nb_index[ INIT_ADD ][ MED_EN::MED_NODE ][ _NODE_TYPE_ ] = add_glob_numbers.size();
  _nb_index[ RSLT_ADD ][ MED_EN::MED_NODE ][ _NODE_TYPE_ ] = add_glob_numbers.size() - merged.size();

  return next_loc_id - 1;
}

//================================================================================
/*!
 * \brief Update coordinates
 */
//================================================================================

void MeshFuse::expandCoordinates(int final_nb_nodes)
{
  const int dim = getSpaceDimension();

  // create new coordinates
  double* coord = new double[ final_nb_nodes * dim ];
  MEDARRAY<double> medarray( coord, dim, final_nb_nodes, MED_EN::MED_FULL_INTERLACE,
                             /*shallowCopy=*/true,/*ownershipOfValues=*/false);
  // copy old coordinates
  int nb_old_coord = getNumberOfNodes() * dim;
  memcpy( coord, getCoordinates( MED_EN::MED_FULL_INTERLACE ), nb_old_coord * sizeof( double ));

  // set new coords
  coord += nb_old_coord;
  const double* add_coord =_mesh->getCoordinates( MED_EN::MED_FULL_INTERLACE );
  if ( _merged_of_type[ _NODE_TYPE_ ].empty())
  {
    // no coincident nodes in the two meshes, just add coords
    memcpy( coord, add_coord, _mesh->getNumberOfNodes() * dim * sizeof( double ));
  }
  else
  {
    // copy coord of only unique nodes
    int first_added_node = getNumberOfNodes() + 1;
    const vector<int>& new_node_ids = _new_elem_ids_of_type[ _NODE_TYPE_ ];
    for ( int n = 0; n < new_node_ids.size(); ++n )
    {
      if ( new_node_ids[n] < first_added_node ) continue; // coincident node
      memcpy( coord, add_coord + n * dim, dim * sizeof( double ));
      coord += dim;
    }
  }
  _coordinate->setCoordinates( &medarray, /*shallowCopy=*/true );

  _numberOfNodes = final_nb_nodes;
}

//================================================================================
/*!
 * \brief Concatenate connectivity of meshes
 *
 * Current implementation impies that cells can't coincide in the meshes
 */
//================================================================================

void MeshFuse::expandConnectivity(int final_nb_nodes)
{
  const medConnectivity nodal   = MED_EN::MED_NODAL;

  // fill in _nb_index[ INIT_OLD ]
  for ( medEntityMesh entity = MED_EN::MED_CELL; entity < MED_ALL_ENTITIES; ++entity )
  {
    if ( existConnectivity( nodal, entity ))
    {
      const int *               index = this->getGlobalNumberingIndex(entity);
      const medGeometryElement* types = this->getTypes(entity);
      int                    nb_types = this->getNumberOfTypes(entity);
      for ( int t = 0; t < nb_types; ++t )
        _nb_index[ INIT_OLD ][ entity ][ types[t] ] = index[t+1]-index[0];
    }
  }

  CONNECTIVITY *prev_connectivity = 0, *cell_connectivity = 0;

  // loop on all entities
  for ( medEntityMesh entity = MED_EN::MED_CELL; entity < MED_ALL_ENTITIES; ++entity )
  {
    if ( entity == MED_EN::MED_FACE && getMeshDimension() == 2 )
      continue; // there can be EDGE

    if ( !_mesh->existConnectivity( nodal, entity ))
    {
      // no entity in added mesh
      if ( existConnectivity( nodal, entity ) && prev_connectivity )
        prev_connectivity->setConstituent
          ( new CONNECTIVITY( *getConnectivityptr()->getConstituent( entity )));
      break;
    }
    if ( !existConnectivity( nodal, entity ))
    {
      // no entity in the old mesh - fully copy the added connectivity
      CONNECTIVITY* connectivity =
        new CONNECTIVITY( *_mesh->getConnectivityptr()->getConstituent( entity ));
      connectivity->setNumberOfNodes( final_nb_nodes );
      updateNodeIds( connectivity );
      if ( entity == MED_EN::MED_CELL )
        cell_connectivity = connectivity;
      else
        cell_connectivity->setConstituent( connectivity );

      break;
    }
    // Fill in _nb_index[ INIT_ADD ]
    const int *                   index = _mesh->getGlobalNumberingIndex(entity);
    const medGeometryElement* add_types = _mesh->getTypes(entity);
    int                    nb_add_types = _mesh->getNumberOfTypes(entity);
    for ( int t = 0; t < nb_add_types; ++t )
      _nb_index[ INIT_ADD ][ entity ][ add_types[t] ] = index[t+1]-index[0];

    // Unite connectivities of std types

    // count types
    set<medGeometryElement> types;
    types.insert( this->getTypes(entity), this->getTypes(entity) + this->getNumberOfTypes(entity));
    types.insert(_mesh->getTypes(entity),_mesh->getTypes(entity) +_mesh->getNumberOfTypes(entity));

    int sum_old = 0, sum_add = 0;

    // make all data
    vector< TConnData >          conn_of_type( types.size() );
    vector< medGeometryElement > type_vec( types.size() );
    vector< int  >               count(1,1);
    set<medGeometryElement>::iterator type = types.begin();
    for ( int t = 0; type != types.end(); ++type, ++t )
    {
      int max_conn_len = 0;
      if ( this->getNumberOfElements( entity, *type ))
        max_conn_len += this->getConnectivityLength( nodal, entity, *type);
      if ( _mesh->getNumberOfElements( entity, *type ))
        max_conn_len += _mesh->getConnectivityLength( nodal, entity, *type);
      conn_of_type[t]._connectivity.reserve( max_conn_len );

      int nb_old = appendConnectivity( conn_of_type[t], this, entity, *type );
      sum_old += nb_old;
      _nb_index[ INIT_OLD ][ entity ][ *type ] = sum_old;

      int nb_add = appendConnectivity( conn_of_type[t],_mesh, entity, *type );
      sum_add += nb_add;
      _nb_index[ RSLT_ADD ][ entity ][ *type ] = sum_add;

      count.push_back( count.back() + conn_of_type[t]._nb_elems );
      type_vec[t] = *type;
    }
    // make new connectivity
    CONNECTIVITY* connectivity = new CONNECTIVITY( types.size(), entity );
    connectivity->setNumberOfNodes ( final_nb_nodes );
    connectivity->setGeometricTypes( &type_vec[0], entity );
    connectivity->setCount         ( &count   [0], entity );
    for ( int t = 0; t < types.size(); ++t )
      connectivity->setNodal( &conn_of_type[t]._connectivity[0],
                              entity, type_vec[t],
                              &conn_of_type[t]._index[0] );

    // store connectivity of an entity
    if ( !prev_connectivity )
    {
      cell_connectivity = connectivity;
      prev_connectivity = cell_connectivity;
    }
    else
    {
      prev_connectivity->setConstituent( connectivity );
      prev_connectivity = connectivity;
    }

  } // loop on entities

  if ( cell_connectivity )
  {
    delete _connectivity;
    _connectivity = cell_connectivity;
  }
}

//================================================================================
/*!
 * \brief Update node ids in the copied connectivity of theadded mesh
 */
//================================================================================

void MeshFuse::updateNodeIds( CONNECTIVITY* connectivity )
{
  const medConnectivity   nodal = MED_EN::MED_NODAL;
  const medGeometryElement type = MED_ALL_ELEMENTS;

  const vector<int>& new_node_ids = _new_elem_ids_of_type[ _NODE_TYPE_ ];

  medEntityMesh entity = connectivity->getEntity();

  for ( ; entity < MED_ALL_ENTITIES; entity++ )
  {
    // Collect all connectivities with their lengths

    list< pair< const int*, int > > conn_len_list;

    if ( connectivity->existConnectivity( nodal, entity ))
      conn_len_list.push_back
        ( make_pair( connectivity->getConnectivity(nodal,entity,type),
                     connectivity->getConnectivityLength(nodal,entity,type)));

    // Convert them

    list< pair< const int*, int > >::iterator conn_len = conn_len_list.begin();
    for ( ; conn_len != conn_len_list.end(); ++conn_len )
    {
      int* conn = (int* ) conn_len->first;
      int* conn_end = conn + conn_len->second;
      for ( ; conn < conn_end; ++conn )
        *conn = new_node_ids[ *conn-1 ];
    }
  }
}

//================================================================================
/*!
 * \brief Concatenate connectivities
 *  \param data - storage of resulting connectivities
 *  \param mesh - mesh to get connectivity from
 *  \param entity - mesh entity to process
 *  \param type - geom type to process
 *  \retval int - nb of appended elements
 */
//================================================================================

int MeshFuse::appendConnectivity( MeshFuse::TConnData& data,
                                  const MESH*          mesh,
                                  medEntityMesh        entity,
                                  medGeometryElement   type)
{
  // get connectivity of type

  const int* conn, *index = 0;
  int nb_elem, conn_len;

  nb_elem = mesh->getNumberOfElements ( entity, type );
  if ( !nb_elem ) return 0;
  conn    = mesh->getConnectivity     ( MED_EN::MED_NODAL, entity, type );
  index   = mesh->getConnectivityIndex( MED_EN::MED_NODAL, entity );
  int shift = getElemNbShift( entity, type, (mesh == this ? INIT_OLD : INIT_ADD), /*prev=*/true);
  index += shift;
  conn_len = index[ nb_elem ] - index[ 0 ];

  bool need_index = ( type == MED_POLYGON || type == MED_POLYHEDRA );
  if ( !need_index )
    data._index.resize( 1, 0 ); // for safe access to pointer even if no real index exists

  // Append

  int old_size = data._nb_elems;
  data._nb_elems += nb_elem;

  if ( mesh == this )
  {
    // append connectivity to data as is
    data._connectivity.insert( data._connectivity.end(), conn, conn + conn_len );
    if ( need_index )
    {
      if ( data._index.empty() )
        data._index.insert( data._index.end(), index, index + nb_elem + 1 );
      else
        for ( int i = 0; i < nb_elem; ++i )
          data._index.push_back( data._index.back() + index[i+1] - index[i] );
    }
  }
  else
  {
    // convert connectivity of other mesh

    vector<int> other_conn( conn_len );
    const vector<int>& new_node_ids = _new_elem_ids_of_type[ _NODE_TYPE_ ];
    if ( type == MED_POLYHEDRA )
      {
        for ( int n = 0; n < conn_len; ++n )
          if ( conn[ n ] > 0 )
            other_conn[ n ] = new_node_ids[ conn[ n ]-1 ];
      }
    else
      {
        for ( int n = 0; n < conn_len; ++n )
          other_conn[ n ] = new_node_ids[ conn[ n ]-1 ];
      }
    if ( entity == MED_EN::MED_CELL || _merged_of_type[ _NODE_TYPE_ ].empty() )
    {
      // store converted connectivity in data
      data._connectivity.insert( data._connectivity.end(), other_conn.begin(), other_conn.end());
      if ( need_index )
      {
        if ( data._index.empty() && index[0] == 1 )
          data._index.insert( data._index.end(), index, index + nb_elem + 1 );
        else
        {
          data._index.push_back( 1 );
          for ( int i = 0; i < nb_elem; ++i )
            data._index.push_back( data._index.back() + index[i+1] - index[i] );
        }
      }
    }
    else
    {
      // exclude coincident elements from connectivity

      if ( need_index && data._index.empty() )
        data._index.push_back( 1 );

      // find added elements possibly(!) coincident with old ones
      vector<int>& merged = _merged_of_type[ type ]; // to fill in
      int first_added_node = _nb_index[ INIT_OLD ][ MED_EN::MED_NODE ][ _NODE_TYPE_ ] + 1;
      for ( int i = 0; i < nb_elem; ++i )
      {
        // count coincident nodes
        int nb_coincident_nodes = 0;
        for ( int n = index[i]; n < index[i+1]; ++n )
          nb_coincident_nodes += int( other_conn[ n-1 ] < first_added_node );

        if ( nb_coincident_nodes == index[i+1] - index[i] )
          merged.push_back( i );
      }
      // find old elements equal to merged, if no equal exist there is zero in array
      vector<int>& equalo = _equalo_of_type[ type ];
      findEqualOldElements( entity, type, equalo );
      if ( equalo.size() < merged.size() )
        equalo.resize( merged.size(), 0 );

      // fill connectivity
      int rm_i = 0, nb_rm = 0;
      for ( int i = 0; i < nb_elem; ++i )
      {
        bool is_merged = ( rm_i < merged.size() && i == merged[rm_i] && equalo[rm_i] );
        if ( is_merged )
        {
          data._nb_elems--;
          rm_i++;
          nb_rm++;
        }
        else
        {
          for ( int n = index[i]; n < index[i+1]; ++n )
            data._connectivity.push_back( other_conn[ n-1 ] );
          if ( need_index )
            data._index.push_back( data._index.back() + index[i+1] - index[i] );
        }
      }
      if ( nb_rm == 0 )
        merged.clear(), equalo.clear();
    }
  }
  return data._nb_elems - old_size;
}

//================================================================================
/*!
 * \brief Return updated old support
 */
//================================================================================

template< class TSUPPORT >
TSUPPORT* MeshFuse::updateOldSupport(TSUPPORT* support) const 
{
  if ( support->isOnAllElements() )
  {
    //support->update(); -- this changes old nb elems to nb elems after fuse
  }
  else if ( support->getEntity() != MED_EN::MED_NODE )
  {
    // element numbers of partial support change if some elements had been added
    // before a type of the entity
    const medGeometryElement* types = support->getTypes();
    int                    nb_types = support->getNumberOfTypes();
    MEDSKYLINEARRAY *        number = support->getnumber();
    for ( int t = 0; t < nb_types; ++t )
    {
      int shift = getElemNbShift( support->getEntity(), types[t], RSLT_ADD, /*prev=*/true);
      if ( shift == 0 ) continue;
      int nb_elems = support->getNumberOfElements( types[t] );
      for ( int j = 0; j < nb_elems; ++j )
        number->setIJ( t+1, j+1, number->getIJ( t+1, j+1 ) + shift );
    }
  }
  return support;
}


//================================================================================
/*!
 * \brief Creates a copy of support being added or extands the present one
 *  \param add_support - support of the added mesh
 *  \param same_name_support - the present support with the same name
 *  \retval SUPPORT* - resulting support
 */
//================================================================================

template< class TSUPPORT >
TSUPPORT* MeshFuse::makeSupport(const TSUPPORT* add_support,
                                TSUPPORT*       same_name_support)
{
  if ( same_name_support && same_name_support->getEntity() != add_support->getEntity() )
    throw MEDEXCEPTION("MeshFuse: Supports with equal names and different entity!");

  // make resulting support

  TSUPPORT* res_support = same_name_support;
  if ( !same_name_support )
  {
    res_support = new TSUPPORT;
    res_support->setName       ( add_support->getName() );
    res_support->setDescription( add_support->getDescription() );
    res_support->setEntity     ( add_support->getEntity() );
    res_support->setMesh       ( this );
  }
  else if ( same_name_support->isOnAllElements() && add_support->isOnAllElements() )
  {
    same_name_support->update();
    return same_name_support;
  }

  // count nb of types

  set<medGeometryElement> add_types, old_types, all_types;
  if ( res_support->getEntity() == MED_EN::MED_NODE )
    add_types.insert( _NODE_TYPE_ );
  else
    add_types.insert( add_support->getTypes(),
                      add_support->getTypes() + add_support->getNumberOfTypes() );
  all_types = add_types;
  if ( same_name_support )
  {
    if ( same_name_support->getEntity() == MED_EN::MED_NODE )
      old_types.insert( _NODE_TYPE_ );
    else
      old_types.insert(same_name_support->getTypes(),
                       same_name_support->getTypes()+same_name_support->getNumberOfTypes() );
    all_types.insert( old_types.begin(), old_types.end() );
  }

  // make all data

  int nb_geom_types = all_types.size();
  vector< medGeometryElement > geom_types( nb_geom_types );
  vector< vector< int > >        elements( nb_geom_types );
  vector< int >               nb_elements( nb_geom_types );
  vector< int >                     index( 1,1 );
  set<medGeometryElement>::iterator type = all_types.begin();
  for ( int t = 0; type != all_types.end(); ++type, ++t )
  {
    uniteSupportElements( add_types.count( *type ) ? add_support       : 0,
                          old_types.count( *type ) ? same_name_support : 0,
                          *type,
                          elements[ t ]);
    nb_elements[ t ] = elements[ t ].size();
    index.push_back( index.back() + nb_elements[ t ] );
  }

  // set data to support

  res_support->setAll( false );
  res_support->setNumberOfGeometricType( nb_geom_types );
  res_support->setGeometricType( &geom_types[0] );
  res_support->setNumberOfElements( &nb_elements[0] );
  res_support->setNumber (new MEDSKYLINEARRAY( nb_geom_types, index[nb_geom_types]-1));
  res_support->getnumber()->setIndex( & index[0] );
  for ( int t = 0; t < nb_geom_types; ++t )
    res_support->getnumber()->setI( t+1, & elements[t][0] );

  return res_support;
}

//================================================================================
/*!
 * \brief Concatenate families and groups
 */
//================================================================================

void MeshFuse::expandSupports()
{
  // we unite supports hiving same names

  // make maps of updated old supports
  map< string, FAMILY* > name_to_family;
  map< string, GROUP* >  name_to_group;
  set<int> family_ids;

  vector<FAMILY*>* fams[4] = { &_familyNode, &_familyCell, &_familyFace, &_familyEdge};
  for ( int i = 0; i < 4; ++i )
    for ( int f = 0; f < fams[i]->size(); ++f )
    {
      name_to_family.insert
        ( make_pair( fams[i]->at(f)->getName(), updateOldSupport( fams[i]->at(f) )));
      family_ids.insert( fams[i]->at(f)->getIdentifier() );
    }

  vector<GROUP*>* groups[4] = { &_groupNode, &_groupCell, &_groupFace, &_groupEdge };
  for ( int i = 0; i < 4; ++i )
    for ( int g = 0; g < groups[i]->size(); ++g )
    {
      name_to_group.insert
        ( make_pair( groups[i]->at(g)->getName(), updateOldSupport( groups[i]->at(g) )));
    }

  // unite supports
  vector<FAMILY*> add_fams[4]={ _mesh->getFamilies(MED_EN::MED_NODE),
                                _mesh->getFamilies(MED_EN::MED_CELL),
                                _mesh->getFamilies(MED_EN::MED_FACE),
                                _mesh->getFamilies(MED_EN::MED_EDGE) };
  for ( int i = 0; i < 4; ++i )
    for ( int f = 0; f < add_fams[i].size(); ++f )
    {
      FAMILY* add_fam = add_fams[i][f];
      map<string,FAMILY*>::iterator name_fam = name_to_family.find( add_fam->getName());
      FAMILY* res_fam = makeSupport( add_fam,
                                     name_fam == name_to_family.end() ? 0 : name_fam->second);
      if ( name_to_family.insert( make_pair( res_fam->getName(), res_fam )).second )
      {
        fams[ i ]->push_back( res_fam );
        int id = add_fam->getIdentifier();
        if ( family_ids.count ( id ))
          id = ( id < 0 ) ? *family_ids.begin() - 1 : *family_ids.rbegin() + 1;
        res_fam->setIdentifier( id );
        family_ids.insert( id );
      }
      // update group names in a family
      vector<string> res_gr_names( res_fam->getGroupsNames(),
                                   res_fam->getGroupsNames()+res_fam->getNumberOfGroups());
      for ( int g = 0; g < add_fam->getNumberOfGroups(); ++g )
        if ( find( res_gr_names.begin(), res_gr_names.end(), add_fam->getGroupName(g+1))
             == res_gr_names.end())
          res_gr_names.push_back( add_fam->getGroupName( g+1 ));
      if ( res_fam->getNumberOfGroups() < res_gr_names.size() )
      {
        res_fam->setNumberOfGroups( res_gr_names.size() );
        res_fam->setGroupsNames( &res_gr_names[0] );
      }
    }

  vector<GROUP*> add_grp[4] ={ _mesh->getGroups(MED_EN::MED_NODE),
                               _mesh->getGroups(MED_EN::MED_CELL),
                               _mesh->getGroups(MED_EN::MED_FACE),
                               _mesh->getGroups(MED_EN::MED_EDGE) };
  for ( int i = 0; i < 4; ++i )
  {
    for ( int g = 0; g < add_grp[i].size(); ++g )
    {
      map< string, GROUP* >::iterator name_grp = name_to_group.find( add_grp[i][g]->getName());
//       STRING out("OLD GRP: ");
//       if ( name_grp == name_to_group.end() ) out << "none";
//       else out << *(name_grp->second);
//       out._s << endl;
//       out << "ADD GRP: " << *(add_grp[i][g]); out._s << endl; out << "";
      GROUP* res_grp = makeSupport( add_grp[i][g],
                                    name_grp == name_to_group.end() ? 0 : name_grp->second);
      if ( !name_to_group.count( res_grp->getName() ))
        groups[ i ]->push_back( res_grp );
      if ( res_grp->getFamilies().size() < add_grp[i][g]->getFamilies().size() )
      {
        res_grp->setFamilies( add_grp[i][g]->getFamilies() );
        res_grp->setNumberOfFamilies( add_grp[i][g]->getNumberOfFamilies() );
      }
//       out << "RES GRP: " << *res_grp;
//       cout << out << endl;
    }
    // update pointers to families in groups
    for ( int g = 0; g < groups[i]->size(); ++g )
    {
      GROUP* grp = groups[i]->at(g);
      if ( grp->getNumberOfFamilies() < 1 ) continue;
      vector<FAMILY*> fams = grp->getFamilies();
      for ( int f = 0; f < fams.size(); ++f )
        fams[f] = name_to_family[ fams[f]->getName() ];
      grp->setFamilies( fams );
      grp->setNumberOfFamilies( fams.size() );
    }
  }
}

//================================================================================
/*!
 * \brief Return shift for conversion of element numbers
 *  \param which - to select a global numbering index
 *  \param prev - true means "shift of type previous to given type"
 */
//================================================================================

int MeshFuse::getElemNbShift( const medEntityMesh& entity,
                              medGeometryElement   type,
                              int which, bool prev ) const
{
  //if ( type == MED_NONE ) type = MED_POINT1;

  const TNbOfGeom & shift_of_type = _nb_index[ which ][ int(entity) ];

  TNbOfGeom::const_iterator type_shift = shift_of_type.lower_bound( type );
  if ( type_shift == shift_of_type.end() )
    return shift_of_type.empty() ? 0 : shift_of_type.rbegin()->second;

  // get shift of exactly the type or of the previos type
  if ( ( prev && type_shift->first == type ) || type_shift->first > type )
  {
    if ( type_shift == shift_of_type.begin() )
      return 0;
    else
      type_shift--;
  }

  return type_shift->second;
}

//================================================================================
/*!
 * \brief Fills in elements of support of given type
 *  \param add_support - support of the added mesh
 *  \param old_support - the present support with the same name
 *  \param type - geometric type to process
 *  \param elements - output array of element
 */
//================================================================================

void MeshFuse::uniteSupportElements(const SUPPORT*     add_support,
                                    SUPPORT*           old_support,
                                    medGeometryElement type,
                                    vector<int> &      elements)
{
  int sup_type = ( type/100 == 0 ? MED_ALL_ELEMENTS : type );

  const medEntityMesh entity = (add_support ? add_support : old_support )->getEntity();

  // Add old elements

  int nb_add_elems = add_support ? add_support->getNumberOfElements( sup_type ) : 0;
  int nb_old_elems = 0;
  if ( old_support )
  {
    nb_old_elems = old_support->getNumberOfElements( sup_type );
    elements.reserve( nb_old_elems + nb_add_elems );
    int add_shift = getElemNbShift( entity, type, RSLT_ADD, /*prev=*/1);
    int old_shift = getElemNbShift( entity, type, INIT_OLD, /*prev=*/1);
    int shift = 1 + add_shift + old_shift;
    // convertion of numbers is already done in updateOldSupport()
    if ( old_support->isOnAllElements() )
      for ( int i = 0; i < nb_old_elems; ++i )
        elements.push_back( i + shift );
    else
      elements.insert( elements.end(), old_support->getNumber( sup_type ),
                       old_support->getNumber( sup_type ) + nb_old_elems );
    if ( nb_add_elems == 0 )
      return;
  }
  else
  {
    elements.reserve( nb_add_elems );
  }

  // Convert and add elements of the added support

  const int * add_elems = add_support->isOnAllElements() ? 0 : add_support->getNumber( sup_type );

  int add_shift = getElemNbShift( entity, type, RSLT_ADD, /*prev=*/1);
  int old_shift = getElemNbShift( entity, type, INIT_OLD, /*prev=*/0);

  if ( _merged_of_type[ type ].empty() )
  {
    // no coicident elements in the old and the added meshes - just unite two lists
    int shift = add_support->isOnAllElements() ? 1 + add_shift + old_shift : old_shift;
    if ( add_support->isOnAllElements() )
      for ( int i = 0; i < nb_add_elems; ++i )
        elements.push_back( i + shift );
    else
      for ( int i = 0; i < nb_add_elems; ++i )
        elements.push_back( add_elems[i] + shift );
  }
  else
  {
    // some elements of the added mesh coincide with old ones,
    // so conversion is not so easy, and some support members can
    // be twice in it - skip them
    vector<int> & new_elem_ids = _new_elem_ids_of_type[ type ];
    if ( new_elem_ids.empty() )
      makeNewElemIds( entity, type, new_elem_ids );

    set< int > old_elems_in_support( elements.begin(), elements.end() );
    int last_old_nb = old_shift + add_shift;
    int shift = 1 + getElemNbShift( entity, type, INIT_ADD, /*prev=*/1);
    for ( int i = 0; i < nb_add_elems; ++i )
    {
      int new_id = new_elem_ids[ add_elems ? add_elems[i]-shift : i ];
      if ( new_id > last_old_nb || old_elems_in_support.count( new_id ) == 0 )
        elements.push_back( new_id );
    }
  }
}

//================================================================================
/*!
 * \brief Fills in ids of elements of added mesh in the resulting mesh
 *  \param type - element type to treat
 *  \param new_ids - output array
 */
//================================================================================

void MeshFuse::makeNewElemIds(medEntityMesh      entity,
                              medGeometryElement type,
                              vector< int > &    new_ids)
{
  const char* LOC = "MeshFuse::makeNewElemIds(): ";
  if ( entity == MED_EN::MED_NODE )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"we must not be here"));

  vector<int>& merged_i = _merged_of_type[ type ];
  vector<int>::iterator rm_i = merged_i.begin();

  // find ids for merged added elements
  vector< int >& old_ids = _equalo_of_type[ type ];
//   if ( old_ids.empty() && !merged_i.empty() )
//     findEqualOldElements( entity, type, old_ids );
  vector< int >::iterator old_id = old_ids.begin();

  // nb of added elements
  int add_nb_elems = _mesh->getNumberOfElements( entity, type );
  new_ids.resize( add_nb_elems );

  // new id of the first added element
  int old_shift = getElemNbShift( entity, type, INIT_OLD, /*prev=*/0);
  int add_shift = getElemNbShift( entity, type, RSLT_ADD, /*prev=*/1);
  int elem_id = old_shift + add_shift + 1;

  // all new ids
  // (this works implying that numbers in SUPPORT are in inceasing order!)
  for (int i_elem = 0; i_elem < add_nb_elems; )
  {
    int i_until = ( rm_i == merged_i.end() ? add_nb_elems : *rm_i++ );
    // increase elem_id until the next merged element
    while ( i_elem < i_until )
      new_ids[ i_elem++ ] = elem_id++;
    // insert id of old element equal to merged one, if any
    if ( i_elem < add_nb_elems )
    {
      if ( *old_id )
        new_ids[ i_elem++ ] = *old_id++;
      else
        new_ids[ i_elem++ ] = elem_id++, old_id++; // no equal old elem exist
    }
  }
}

//================================================================================
/*!
 * \brief Finds ids of elements of the old mesh equal to merged elements of the added one
 */
//================================================================================

void MeshFuse::findEqualOldElements(medEntityMesh      entity,
                                    medGeometryElement type,
                                    vector< int > &    old_ids)
{
  // poly element can coincide with any type of the same entity
  const bool isPoly = ( type == MED_POLYGON || type == MED_POLYHEDRA );
  const medGeometryElement checkType = isPoly ? MED_ALL_ELEMENTS : type;

  if ( !_mesh->getNumberOfElements(entity, type) ||
       ! this->getNumberOfElements(entity, checkType) )
    return;

  int old_nb_elems_end, old_nb_elems_start;
  if ( isPoly )
    {
      old_nb_elems_start = 0;
      old_nb_elems_end   = this->getNumberOfElements( entity, MED_ALL_ELEMENTS );
    }
  else
    {
      // if this method is called when connectivity of the entity is not yet concatenated:
      old_nb_elems_start = getElemNbShift( entity, type, INIT_OLD, /*prev=*/true);
      old_nb_elems_end   = getElemNbShift( entity, type, INIT_OLD, /*prev=*/false);
      // if this method is called after expandConnectivity() and this mesh contains all elements
      //   int old_nb_elems = 
    }
  const int *old_conn, *old_index, *add_conn, *add_index;
  add_conn  = _mesh->getConnectivity( MED_EN::MED_NODAL, entity, type );
  old_conn  =  this->getConnectivity( MED_EN::MED_NODAL, entity, checkType );
  add_index = _mesh->getConnectivityIndex( MED_EN::MED_NODAL, entity );
  old_index =  this->getConnectivityIndex( MED_EN::MED_NODAL, entity );

  const vector<int>& new_node_ids = _new_elem_ids_of_type[ _NODE_TYPE_ ];

  const vector<int>& merged_i = _merged_of_type[ type ];
  vector<int>::const_iterator rm_i = merged_i.begin();

  old_ids.reserve( merged_i.size() );

  for ( ; rm_i != merged_i.end(); ++rm_i ) // rm_i counts from 0
  {
    // get nodes of rm_i-th added face
    const int* add_elem_conn = add_conn + add_index[ *rm_i ]-1;
    int    nb_add_elem_nodes = add_index[ *rm_i+1 ] - add_index[ *rm_i ];
    set<int> add_elem_nodes;
    for ( int n = 0; n < nb_add_elem_nodes; ++n )
      add_elem_nodes.insert( new_node_ids[ add_elem_conn[n]-1 ]);

    // look for equal old face among all old faces
    const int* old_node = old_conn;
    int        new_node = *add_elem_nodes.begin();
    int  found_old_elem = 0;
    for ( int i = old_nb_elems_start; i < old_nb_elems_end && !found_old_elem; ++i )
    {
      int nb_old_elem_nodes = old_index[ i+1 ] - old_index[ i ];
      for ( int j = 0; j < nb_old_elem_nodes; ++j, ++old_node )
      {
        if ( new_node != *old_node ) continue;
        // found common node, compare all nodes
        const int* old_elem_conn = old_conn + old_index[ i ]-1;
        set<int> old_elem_nodes( old_elem_conn, old_elem_conn + nb_old_elem_nodes);
        if ( add_elem_nodes == old_elem_nodes )
        {
          found_old_elem = i + 1;
          break;
        }
      }
    }
    // Issue 0020809. Its possible that no old element exists
//     if ( !found_old_elem )
//       throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"all nodes of added elements are merged "
//                                    "but equal old element not found. "
//                                    "Please check global nodes numbers."));
    old_ids.push_back( found_old_elem );
  }
}

//================================================================================
/*!
 * \brief Unite numbers of elements taking into account their types
 */
//================================================================================

void MeshFuse::append( medEntityMesh      entity,
                       vector<int>&       numbers,
                       const vector<int>& add_numbers )
{
  const char* LOC="MeshFuse::append(): ";

  int nb_types = getNumberOfTypes( entity );
  if ( numbers.empty() || add_numbers.empty() ||
       ( nb_types < 2 && _merged_of_type[ getElementType( entity, 1 )].empty() ))
  {
    numbers.insert( numbers.end(), add_numbers.begin(), add_numbers.end() );
    return;
  }

  vector<int> result;
  result.reserve( numbers.size() + add_numbers.size() );
  const int* old_nums = & numbers[0];
  const int* add_nums = & add_numbers[0];

  MEDMEM::PointerOf<medGeometryElement> types;
  types.setShallowAndOwnership( getTypes(entity));

  for ( int t = 0; t < nb_types; ++t )
  {
    int nb_old =
      getElemNbShift( entity, types[t], INIT_OLD, /*prev=*/false) -
      getElemNbShift( entity, types[t], INIT_OLD, /*prev=*/true);
    int nb_add =
      getElemNbShift( entity, types[t], INIT_ADD, /*prev=*/false) -
      getElemNbShift( entity, types[t], INIT_ADD, /*prev=*/true);

    result.insert( result.end(), old_nums, old_nums + nb_old );
    old_nums += nb_old;

    const vector<int>& loc_merged = _merged_of_type[ types[t] ];
    if ( loc_merged.empty() )
    {
      result.insert( result.end(), add_nums, add_nums + nb_add );
    }
    else
    {
      vector<int>::const_iterator imerged = loc_merged.begin();
      vector<int>::const_iterator equalid = _equalo_of_type[ types[t] ].begin();

      int from = 0, to = -1;
      while ( from < nb_add )
      {
        while ( imerged != loc_merged.end() && *equalid == 0 )
          imerged++, equalid++;
        to = ( imerged == loc_merged.end() ? nb_add : ( equalid++, *imerged++ ));
        if ( to > from )
          result.insert( result.end(), add_nums + from, add_nums + to );
        from = to + 1;
      }
    }
    add_nums += nb_add;
  }
  if ( result.size() != getNumberOfElements( entity, MED_ALL_ELEMENTS ))
    throw MED_EXCEPTION(MEDMEM::STRING(LOC) << "invalid nb of numbers of entity " << entity
                        << ": expect " << getNumberOfElements( entity, MED_ALL_ELEMENTS)
                        << " but get " << result.size());

  numbers.swap(result);
}
