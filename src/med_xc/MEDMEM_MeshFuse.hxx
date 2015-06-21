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
// File      : MEDMEM_MeshFuse.hxx
// Created   : Tue Jul  7 17:04:38 2009
// Author    : Edward AGAPOV (eap)

#ifndef __MEDMEM_MeshFuse_HXX__
#define __MEDMEM_MeshFuse_HXX__

#include "MEDMEM_Meshing.hxx"

#include <vector>
#include <map>
#include <set>

namespace MEDMEM
{
  /*!
   * \brief Mesh able to concatenate other meshes
   */
  class MEDMEM_EXPORT MeshFuse : public MESHING
  {
  public:
    MeshFuse();
    virtual ~MeshFuse();

    void concatenate( const MESH* mesh, const std::vector<int>& node_glob_numbers );

    // unite glob_numbers and add_glob_numbers 
    void append( MED_EN::medEntityMesh   entity,
                 std::vector<int>&       glob_numbers,
                 const std::vector<int>& add_glob_numbers );

    // if MeshFuse is filled via MESHING
    void setNodeNumbers( const std::vector<int>& node_glob_numbers );

    // return number collected during all concatenate()s
    const std::vector<int> & getNodeNumbers() const { return _node_glob_numbers; }

  private:

    int makeNewNodeIds(const std::vector<int>& node_glob_numbers);

    void expandCoordinates(int final_nb_nodes);

    void expandConnectivity(int final_nb_nodes);

    void updateNodeIds( CONNECTIVITY* connectivity );

    struct TConnData
    {
      int _nb_elems;
      std::vector< int > _connectivity, _index;

      TConnData(): _nb_elems(0) {}
    };

    int appendConnectivity( TConnData&                 data,
                            const MESH*                mesh,
                            MED_EN::medEntityMesh      entity,
                            MED_EN::medGeometryElement type);

    template< class TSUPPORT >
    TSUPPORT* updateOldSupport(TSUPPORT* support) const;

    template< class TSUPPORT >
    TSUPPORT* makeSupport(const TSUPPORT* add_support, TSUPPORT* same_name_support);

    void expandSupports();

    int getElemNbShift( const MED_EN::medEntityMesh& entity,
                        MED_EN::medGeometryElement   type,
                        int which, bool prev ) const;

    void uniteSupportElements(const SUPPORT*             add_support,
                              SUPPORT*                   old_support,
                              MED_EN::medGeometryElement type,
                              std::vector<int> &         elements);

    void makeNewElemIds(MED_EN::medEntityMesh      entity,
                        MED_EN::medGeometryElement type,
                        std::vector< int > &       new_ids);

    void findEqualOldElements(MED_EN::medEntityMesh      entity,
                              MED_EN::medGeometryElement type,
                              std::vector< int > &       old_ids);

  private:

    const MESH* _mesh; // mesh to add

    std::vector<int> _node_glob_numbers; // global numbers of nodes

    // local ids of merged entities (whose all nodes are merged) of the added mesh
    std::map< MED_EN::medGeometryElement, std::vector<int> > _merged_of_type;
    // and corresponding ids of EQUAL elements (if any) of OLD mesh
    std::map< MED_EN::medGeometryElement, std::vector<int> > _equalo_of_type;

    // ids in final mesh of added elements of a geom type filled in case of double elements
    std::map< MED_EN::medGeometryElement, std::vector<int> > _new_elem_ids_of_type;

    // global numbering index by type:
    // - of old mesh before addition
    // - of added mesh before addition
    // - of mesh added (taking merging into account)
    enum { INIT_OLD=0, INIT_ADD, RSLT_ADD, NB_INDICES };
    typedef std::map< MED_EN::medGeometryElement, int > TNbOfGeom;
    vector< TNbOfGeom > _nb_index[NB_INDICES]; // for each entity

  };
}

#endif
