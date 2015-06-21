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

/*
 File MEDMEM_CellModel.cxx
*/

#include "MEDMEM_CellModel.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

CELLMODEL_Map *CELLMODEL_Map::_singleton=0;

const MEDMEM::CELLMODEL& CELLMODEL_Map::getCellModel(MED_EN::medGeometryElement type)
{
  map<MED_EN::medGeometryElement,MEDMEM::CELLMODEL>::iterator type2model = _cell_models.find(type);
  if ( type2model == _cell_models.end() )
    type2model = _cell_models.insert( make_pair( type, CELLMODEL( type ))).first;

  return type2model->second;
}

const MEDMEM::CELLMODEL& CELLMODEL_Map::retrieveCellModel(MED_EN::medGeometryElement type)
{
  return CELLMODEL_Map::getInstance()->getCellModel(type);
}

CELLMODEL_Map *CELLMODEL_Map::getInstance()
{
  if(!_singleton) _singleton=new CELLMODEL_Map;
  return _singleton;
}

CELLMODEL::CELLMODEL(medGeometryElement t)
{
  // init first all to nothing
  _dimension=0 ;
  _numberOfNodes=0 ;
  _numberOfVertexes=0 ;
  _numberOfConstituentsDimension=0 ;
  _numberOfConstituents=(int*)NULL ;
  _numberOfNodeOfEachConstituent=(int**)NULL ;
  _constituents=(int***)NULL ; 
  _constituentsType=(medGeometryElement**)NULL ;
  
  MESSAGE_MED("CELLMODEL : constructeur pour le type " << t);
  switch (t)
    {
    case MED_EN::MED_POINT1  : {
      _name="MED_POINT1";
      _type=t;
      _dimension=0;
      _numberOfVertexes=1;
      _numberOfNodes=1;
      // nothing else : POINT are none constituent
      break;
    }
    case MED_EN::MED_SEG2    : {
      _name="MED_SEG2" ;
      _type=t;
      _dimension=1;
      _numberOfVertexes=2;
      _numberOfNodes=2;
      // constituent are POINT1 and we have no need to define _constituents vector
      _numberOfConstituentsDimension=1 ;
      _numberOfConstituents=new int[1] ;
      _numberOfConstituents[0]=2 ;
      _numberOfNodeOfEachConstituent=new int*[1] ;
      _numberOfNodeOfEachConstituent[0]=new int[2] ;
      _numberOfNodeOfEachConstituent[0][0]=1 ;
      _numberOfNodeOfEachConstituent[0][1]=1 ;

      _constituents = new int**[1] ;
      _constituents[0] = new int*[2] ;
      _constituents[0][0] = new int[1] ;
      _constituents[0][0][0] = 1 ;
      _constituents[0][1] = new int[1] ;
      _constituents[0][1][0] = 2 ;

      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[2] ;
      tmpConstituentsType1[0] = MED_EN::MED_POINT1 ;
      tmpConstituentsType1[1] = MED_EN::MED_POINT1 ;
      _constituentsType = new medGeometryElement*[1] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      break;
    }
    case MED_EN::MED_SEG3    : {
      _name="MED_SEG3" ;
      _type=t;
      _dimension=1;
      _numberOfVertexes=2;
      _numberOfNodes=3;
      // constituent are POINT1 and we have no need to define _constituents vector
      _numberOfConstituentsDimension=1 ;
      _numberOfConstituents=new int[1] ;
      _numberOfConstituents[0]=3 ;
      _numberOfNodeOfEachConstituent=new int*[1] ;
      _numberOfNodeOfEachConstituent[0]=new int[3] ;
      _numberOfNodeOfEachConstituent[0][0]=1 ;
      _numberOfNodeOfEachConstituent[0][1]=1 ;
      _numberOfNodeOfEachConstituent[0][2]=1 ;

      _constituents = new int**[1] ;
      _constituents[0] = new int*[3] ;
      _constituents[0][0] = new int[1] ;
      _constituents[0][0][0] = 1 ;
      _constituents[0][1] = new int[1] ;
      _constituents[0][1][0] = 2 ;
      _constituents[0][2] = new int[1] ;
      _constituents[0][2][0] = 3 ;

      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[3] ;
      tmpConstituentsType1[0] = MED_EN::MED_POINT1 ;
      tmpConstituentsType1[1] = MED_EN::MED_POINT1 ;
      tmpConstituentsType1[2] = MED_EN::MED_POINT1 ;
      _constituentsType = new medGeometryElement*[1] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      break;
    }
    case MED_EN::MED_TRIA3   : {
      _name="MED_TRIA3" ;
      _type=t;
      _dimension=2;
      _numberOfVertexes=3;
      _numberOfNodes=3;

      _numberOfConstituentsDimension=1 ;
      _numberOfConstituents=new int[1] ;
      _numberOfConstituents[0]=3 ;
      _numberOfNodeOfEachConstituent=new int*[1] ;
      _numberOfNodeOfEachConstituent[0]=new int[3] ;
      _numberOfNodeOfEachConstituent[0][0]=2 ;
      _numberOfNodeOfEachConstituent[0][1]=2 ;
      _numberOfNodeOfEachConstituent[0][2]=2 ;

      int* _edge1=new int[2];
      _edge1[0]=1;
      _edge1[1]=2;
      int* _edge2=new int[2];
      _edge2[0]=2;
      _edge2[1]=3;
      int* _edge3=new int[2];
      _edge3[0]=3;
      _edge3[1]=1;
      int ** tmpConstituents1 = new int*[3];
      tmpConstituents1[0]=_edge1 ;
      tmpConstituents1[1]=_edge2 ;
      tmpConstituents1[2]=_edge3 ;
      _constituents = new int**[1] ;
      _constituents[0]=tmpConstituents1 ;

      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[3] ;
      tmpConstituentsType1[0] = MED_EN::MED_SEG2 ;
      tmpConstituentsType1[1] = MED_EN::MED_SEG2 ;
      tmpConstituentsType1[2] = MED_EN::MED_SEG2 ;
      _constituentsType = new medGeometryElement*[1] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      // Well, point are defined, but could not be acces because we have 
      // only 1 numberOfConstituentsDimension !

      break;
    }
    case MED_EN::MED_TRIA6   : {
      _name="MED_TRIA6" ;
      _type=t;
      _dimension=2;
      _numberOfVertexes=3;
      _numberOfNodes=6;
      _numberOfConstituentsDimension=1 ;
      _numberOfConstituents=new int[1] ;
      _numberOfConstituents[0]=3 ;
      _numberOfNodeOfEachConstituent=new int*[1] ;
      _numberOfNodeOfEachConstituent[0]=new int[3] ;
      _numberOfNodeOfEachConstituent[0][0]=3 ;
      _numberOfNodeOfEachConstituent[0][1]=3 ;
      _numberOfNodeOfEachConstituent[0][2]=3 ;

      int* _edge1=new int[3];
      _edge1[0]=1;
      _edge1[1]=2;
      _edge1[2]=4;
      int* _edge2=new int[3];
      _edge2[0]=2;
      _edge2[1]=3;
      _edge2[2]=5;
      int* _edge3=new int[3];
      _edge3[0]=3;
      _edge3[1]=1;
      _edge3[2]=6;
      int ** tmpConstituents1 = new int*[3];
      tmpConstituents1[0]=_edge1 ;
      tmpConstituents1[1]=_edge2 ;
      tmpConstituents1[2]=_edge3 ;
      _constituents = new int**[1] ;
      _constituents[0]=tmpConstituents1 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[3] ;
      tmpConstituentsType1[0] = MED_EN::MED_SEG3 ;
      tmpConstituentsType1[1] = MED_EN::MED_SEG3 ;
      tmpConstituentsType1[2] = MED_EN::MED_SEG3 ;
      _constituentsType = new medGeometryElement*[1] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      // Well, point are defined, but could not be acces because we have 
      // only 1 numberOfConstituentsDimension !

      break;
    }
    case MED_EN::MED_QUAD4   : {
      _name="MED_QUAD4" ;
      _type=t;
      _dimension=2;
      _numberOfVertexes=4;
      _numberOfNodes=4;
      _numberOfConstituentsDimension=1 ;
      _numberOfConstituents=new int[1] ;
      _numberOfConstituents[0]=4 ;
      _numberOfNodeOfEachConstituent=new int*[1] ;
      _numberOfNodeOfEachConstituent[0]=new int[4] ;
      _numberOfNodeOfEachConstituent[0][0]=2 ;
      _numberOfNodeOfEachConstituent[0][1]=2 ;
      _numberOfNodeOfEachConstituent[0][2]=2 ;
      _numberOfNodeOfEachConstituent[0][3]=2 ;

      int* _edge1=new int[2];
      _edge1[0]=1;
      _edge1[1]=2;
      int* _edge2=new int[2];
      _edge2[0]=2;
      _edge2[1]=3;
      int* _edge3=new int[2];
      _edge3[0]=3;
      _edge3[1]=4;
      int* _edge4=new int[2];
      _edge4[0]=4;
      _edge4[1]=1;
      int ** tmpConstituents1 = new int*[4];
      tmpConstituents1[0]=_edge1 ;
      tmpConstituents1[1]=_edge2 ;
      tmpConstituents1[2]=_edge3 ;
      tmpConstituents1[3]=_edge4 ;
      _constituents = new int**[1] ;
      _constituents[0]=tmpConstituents1 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[4] ;
      tmpConstituentsType1[0] = MED_EN::MED_SEG2 ;
      tmpConstituentsType1[1] = MED_EN::MED_SEG2 ;
      tmpConstituentsType1[2] = MED_EN::MED_SEG2 ;
      tmpConstituentsType1[3] = MED_EN::MED_SEG2 ;
      _constituentsType = new medGeometryElement*[1] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      // Well, point are defined, but could not be acces because we have 
      // only 1 numberOfConstituentsDimension !

      break;
    }
    case MED_EN::MED_QUAD8   : {
      _name="MED_QUAD8" ;
      _type=t;
      _dimension=2;
      _numberOfVertexes=4;
      _numberOfNodes=8;
      _numberOfConstituentsDimension=1 ;
      _numberOfConstituents=new int[1] ;
      _numberOfConstituents[0]=4 ;
      _numberOfNodeOfEachConstituent=new int*[1] ;
      _numberOfNodeOfEachConstituent[0]=new int[4] ;
      _numberOfNodeOfEachConstituent[0][0]=3 ;
      _numberOfNodeOfEachConstituent[0][1]=3 ;
      _numberOfNodeOfEachConstituent[0][2]=3 ;
      _numberOfNodeOfEachConstituent[0][3]=3 ;

      int* _edge1=new int[3];
      _edge1[0]=1;
      _edge1[1]=2;
      _edge1[2]=5;
      int* _edge2=new int[3];
      _edge2[0]=2;
      _edge2[1]=3;
      _edge2[2]=6;
      int* _edge3=new int[3];
      _edge3[0]=3;
      _edge3[1]=4;
      _edge3[2]=7;
      int* _edge4=new int[3];
      _edge4[0]=4;
      _edge4[1]=1;
      _edge4[2]=8;
      int ** tmpConstituents1 = new int*[4];
      tmpConstituents1[0]=_edge1 ;
      tmpConstituents1[1]=_edge2 ;
      tmpConstituents1[2]=_edge3 ;
      tmpConstituents1[3]=_edge4 ;
      _constituents = new int**[1] ;
      _constituents[0]=tmpConstituents1 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[4] ;
      tmpConstituentsType1[0] = MED_EN::MED_SEG3 ;
      tmpConstituentsType1[1] = MED_EN::MED_SEG3 ;
      tmpConstituentsType1[2] = MED_EN::MED_SEG3 ;
      tmpConstituentsType1[3] = MED_EN::MED_SEG3 ;
      _constituentsType = new medGeometryElement*[1] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      // Well, point are defined, but could not be acces because we have 
      // only 1 numberOfConstituentsDimension !

      break;
    }
    case MED_EN::MED_TETRA4  : {
      _name="MED_TETRA4" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=4;
      _numberOfNodes=4;
      _numberOfConstituentsDimension=2 ;
      _numberOfConstituents=new int[2] ;
      _numberOfConstituents[0]=4 ;
      _numberOfConstituents[1]=6 ;
      _numberOfNodeOfEachConstituent=new int*[2] ;
      _numberOfNodeOfEachConstituent[0]=new int[4] ;
      _numberOfNodeOfEachConstituent[0][0]=3 ;
      _numberOfNodeOfEachConstituent[0][1]=3 ;
      _numberOfNodeOfEachConstituent[0][2]=3 ;
      _numberOfNodeOfEachConstituent[0][3]=3 ;
      _numberOfNodeOfEachConstituent[1]=new int[6] ;
      _numberOfNodeOfEachConstituent[1][0]=2 ;
      _numberOfNodeOfEachConstituent[1][1]=2 ;
      _numberOfNodeOfEachConstituent[1][2]=2 ;
      _numberOfNodeOfEachConstituent[1][3]=2 ;
      _numberOfNodeOfEachConstituent[1][4]=2 ;
      _numberOfNodeOfEachConstituent[1][5]=2 ;

      int* _face1=new int[3];
      _face1[0]=1;
      _face1[1]=2;
      _face1[2]=3;
      int* _face2=new int[3];
      _face2[0]=1;
      _face2[1]=4;
      _face2[2]=2;
      int* _face3=new int[3];
      _face3[0]=2;
      _face3[1]=4;
      _face3[2]=3;
      int* _face4=new int[3];
      _face4[0]=3;
      _face4[1]=4;
      _face4[2]=1;
      int* _edge1=new int[2];
      _edge1[0]=1;
      _edge1[1]=2;
      int* _edge2=new int[2];
      _edge2[0]=2;
      _edge2[1]=3;
      int* _edge3=new int[2];
      _edge3[0]=3;
      _edge3[1]=1;
      int* _edge4=new int[2];
      _edge4[0]=1;
      _edge4[1]=4;
      int* _edge5=new int[2];
      _edge5[0]=2;
      _edge5[1]=4;
      int* _edge6=new int[2];
      _edge6[0]=3;
      _edge6[1]=4;
      int ** tmpConstituents1 = new int*[4];
      tmpConstituents1[0]=_face1 ;
      tmpConstituents1[1]=_face2 ;
      tmpConstituents1[2]=_face3 ;
      tmpConstituents1[3]=_face4 ;
      int ** tmpConstituents2 = new int*[6];
      tmpConstituents2[0]=_edge1 ;
      tmpConstituents2[1]=_edge2 ;
      tmpConstituents2[2]=_edge3 ;
      tmpConstituents2[3]=_edge4 ;
      tmpConstituents2[4]=_edge5 ;
      tmpConstituents2[5]=_edge6 ;
      _constituents = new int**[2] ;
      _constituents[0]=tmpConstituents1 ;
      _constituents[1]=tmpConstituents2 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[4] ;
      tmpConstituentsType1[0] = MED_EN::MED_TRIA3 ;
      tmpConstituentsType1[1] = MED_EN::MED_TRIA3 ;
      tmpConstituentsType1[2] = MED_EN::MED_TRIA3 ;
      tmpConstituentsType1[3] = MED_EN::MED_TRIA3 ;
      medGeometryElement * tmpConstituentsType2 = new medGeometryElement[6] ;
      tmpConstituentsType2[0] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[1] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[2] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[3] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[4] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[5] = MED_EN::MED_SEG2 ;
      _constituentsType = new medGeometryElement*[2] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      _constituentsType[1]=tmpConstituentsType2 ;
      break;
    }
    case MED_EN::MED_TETRA10 : {
      _name="MED_TETRA10" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=4;
      _numberOfNodes=10;
      _numberOfConstituentsDimension=2 ;
      _numberOfConstituents=new int[2] ;
      _numberOfConstituents[0]=4 ;
      _numberOfConstituents[1]=6 ;
      _numberOfNodeOfEachConstituent=new int*[2] ;
      _numberOfNodeOfEachConstituent[0]=new int[4] ;
      _numberOfNodeOfEachConstituent[0][0]=6 ;
      _numberOfNodeOfEachConstituent[0][1]=6 ;
      _numberOfNodeOfEachConstituent[0][2]=6 ;
      _numberOfNodeOfEachConstituent[0][3]=6 ;
      _numberOfNodeOfEachConstituent[1]=new int[6] ;
      _numberOfNodeOfEachConstituent[1][0]=3 ;
      _numberOfNodeOfEachConstituent[1][1]=3 ;
      _numberOfNodeOfEachConstituent[1][2]=3 ;
      _numberOfNodeOfEachConstituent[1][3]=3 ;
      _numberOfNodeOfEachConstituent[1][4]=3 ;
      _numberOfNodeOfEachConstituent[1][5]=3 ;

      int* _face1=new int[6];
      _face1[0]=1;
      _face1[1]=2;
      _face1[2]=3;
      _face1[3]=5;
      _face1[4]=6;
      _face1[5]=7;
      int* _face2=new int[6];
      _face2[0]=1;
      _face2[1]=4;
      _face2[2]=2;
      _face2[3]=8;
      _face2[4]=9;
      _face2[5]=5;
      int* _face3=new int[6];
      _face3[0]=2;
      _face3[1]=4;
      _face3[2]=3;
      _face3[3]=9;
      _face3[4]=10;
      _face3[5]=6;
      int* _face4=new int[6];
      _face4[0]=3;
      _face4[1]=4;
      _face4[2]=1;
      _face4[3]=10;
      _face4[4]=8;
      _face4[5]=7;
      int* _edge1=new int[3];
      _edge1[0]=1;
      _edge1[1]=2;
      _edge1[2]=5;
      int* _edge2=new int[3];
      _edge2[0]=2;
      _edge2[1]=3;
      _edge2[2]=6;
      int* _edge3=new int[3];
      _edge3[0]=3;
      _edge3[1]=1;
      _edge3[2]=7;
      int* _edge4=new int[3];
      _edge4[0]=1;
      _edge4[1]=4;
      _edge4[2]=8;
      int* _edge5=new int[3];
      _edge5[0]=2;
      _edge5[1]=4;
      _edge5[2]=9;
      int* _edge6=new int[3];
      _edge6[0]=3;
      _edge6[1]=4;
      _edge6[2]=10;
      int ** tmpConstituents1 = new int*[4];
      tmpConstituents1[0]=_face1 ;
      tmpConstituents1[1]=_face2 ;
      tmpConstituents1[2]=_face3 ;
      tmpConstituents1[3]=_face4 ;
      int ** tmpConstituents2 = new int*[6];
      tmpConstituents2[0]=_edge1 ;
      tmpConstituents2[1]=_edge2 ;
      tmpConstituents2[2]=_edge3 ;
      tmpConstituents2[3]=_edge4 ;
      tmpConstituents2[4]=_edge5 ;
      tmpConstituents2[5]=_edge6 ;
      _constituents = new int**[2] ;
      _constituents[0]=tmpConstituents1 ;
      _constituents[1]=tmpConstituents2 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[4] ;
      tmpConstituentsType1[0] = MED_EN::MED_TRIA6 ;
      tmpConstituentsType1[1] = MED_EN::MED_TRIA6 ;
      tmpConstituentsType1[2] = MED_EN::MED_TRIA6 ;
      tmpConstituentsType1[3] = MED_EN::MED_TRIA6 ;
      medGeometryElement * tmpConstituentsType2 = new medGeometryElement[6] ;
      tmpConstituentsType2[0] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[1] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[2] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[3] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[4] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[5] = MED_EN::MED_SEG3 ;
      _constituentsType = new medGeometryElement*[2] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      _constituentsType[1]=tmpConstituentsType2 ;
    break;
    }
    case MED_EN::MED_HEXA8   : {
      _name="MED_HEXA8" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=8;
      _numberOfNodes=8;
      _numberOfConstituentsDimension=2 ;
      _numberOfConstituents=new int[2] ;
      _numberOfConstituents[0]=6 ;
      _numberOfConstituents[1]=12 ;
      _numberOfNodeOfEachConstituent=new int*[2] ;
      _numberOfNodeOfEachConstituent[0]=new int[6] ;
      _numberOfNodeOfEachConstituent[0][0]=4 ;
      _numberOfNodeOfEachConstituent[0][1]=4 ;
      _numberOfNodeOfEachConstituent[0][2]=4 ;
      _numberOfNodeOfEachConstituent[0][3]=4 ;
      _numberOfNodeOfEachConstituent[0][4]=4 ;
      _numberOfNodeOfEachConstituent[0][5]=4 ;
      _numberOfNodeOfEachConstituent[1]=new int[12] ;
      _numberOfNodeOfEachConstituent[1][0]=2 ;
      _numberOfNodeOfEachConstituent[1][1]=2 ;
      _numberOfNodeOfEachConstituent[1][2]=2 ;
      _numberOfNodeOfEachConstituent[1][3]=2 ;
      _numberOfNodeOfEachConstituent[1][4]=2 ;
      _numberOfNodeOfEachConstituent[1][5]=2 ;
      _numberOfNodeOfEachConstituent[1][6]=2 ;
      _numberOfNodeOfEachConstituent[1][7]=2 ;
      _numberOfNodeOfEachConstituent[1][8]=2 ;
      _numberOfNodeOfEachConstituent[1][9]=2 ;
      _numberOfNodeOfEachConstituent[1][10]=2 ;
      _numberOfNodeOfEachConstituent[1][11]=2 ;

      int* _edge1=new int[2];
      _edge1[0]=1;
      _edge1[1]=2;
      int* _edge2=new int[2];
      _edge2[0]=2;
      _edge2[1]=3;
      int* _edge3=new int[2];
      _edge3[0]=3;
      _edge3[1]=4;
      int* _edge4=new int[2];
      _edge4[0]=4;
      _edge4[1]=1;
      int* _edge5=new int[2];
      _edge5[0]=5;
      _edge5[1]=6;
      int* _edge6=new int[2];
      _edge6[0]=6;
      _edge6[1]=7;
      int* _edge7=new int[2];
      _edge7[0]=7;
      _edge7[1]=8;
      int* _edge8=new int[2];
      _edge8[0]=8;
      _edge8[1]=5;
      int* _edge9=new int[2];
      _edge9[0]=1;
      _edge9[1]=5;
      int* _edge10=new int[2];
      _edge10[0]=2;
      _edge10[1]=6;
      int* _edge11=new int[2];
      _edge11[0]=3;
      _edge11[1]=7;
      int* _edge12=new int[2];
      _edge12[0]=4;
      _edge12[1]=8;
      int* _face1=new int[4];
      _face1[0]=1;
      _face1[1]=2;
      _face1[2]=3;
      _face1[3]=4;
      int* _face2=new int[4];
      _face2[0]=5;
      _face2[1]=8;
      _face2[2]=7;
      _face2[3]=6;
      int* _face3=new int[4];
      _face3[0]=1;
      _face3[1]=5;
      _face3[2]=6;
      _face3[3]=2;
      int* _face4=new int[4];
      _face4[0]=2;
      _face4[1]=6;
      _face4[2]=7;
      _face4[3]=3;
      int* _face5=new int[4];
      _face5[0]=3;
      _face5[1]=7;
      _face5[2]=8;
      _face5[3]=4;
     int* _face6=new int[4];
      _face6[0]=4;
      _face6[1]=8;
      _face6[2]=5;
      _face6[3]=1;
      int ** tmpConstituents1 = new int*[6];
      tmpConstituents1[0]=_face1 ;
      tmpConstituents1[1]=_face2 ;
      tmpConstituents1[2]=_face3 ;
      tmpConstituents1[3]=_face4 ;
      tmpConstituents1[4]=_face5 ;
      tmpConstituents1[5]=_face6 ;
      int ** tmpConstituents2 = new int*[12];
      tmpConstituents2[0]=_edge1 ;
      tmpConstituents2[1]=_edge2 ;
      tmpConstituents2[2]=_edge3 ;
      tmpConstituents2[3]=_edge4 ;
      tmpConstituents2[4]=_edge5 ;
      tmpConstituents2[5]=_edge6 ;
      tmpConstituents2[6]=_edge7 ;
      tmpConstituents2[7]=_edge8 ;
      tmpConstituents2[8]=_edge9 ;
      tmpConstituents2[9]=_edge10;
      tmpConstituents2[10]=_edge11;
      tmpConstituents2[11]=_edge12;
      _constituents = new int**[2] ;
      _constituents[0]=tmpConstituents1 ;
      _constituents[1]=tmpConstituents2 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[6] ;
      tmpConstituentsType1[0] = MED_EN::MED_QUAD4 ;
      tmpConstituentsType1[1] = MED_EN::MED_QUAD4 ;
      tmpConstituentsType1[2] = MED_EN::MED_QUAD4 ;
      tmpConstituentsType1[3] = MED_EN::MED_QUAD4 ;
      tmpConstituentsType1[4] = MED_EN::MED_QUAD4 ;
      tmpConstituentsType1[5] = MED_EN::MED_QUAD4 ;
      medGeometryElement * tmpConstituentsType2 = new medGeometryElement[12] ;
      tmpConstituentsType2[0] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[1] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[2] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[3] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[4] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[5] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[6] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[7] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[8] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[9] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[10] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[11] = MED_EN::MED_SEG2 ;
      _constituentsType = new medGeometryElement*[2] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      _constituentsType[1]=tmpConstituentsType2 ;
      break;
    }
    case MED_EN::MED_HEXA20  : {
      _name="MED_HEXA20" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=8;
      _numberOfNodes=20;
      _numberOfConstituentsDimension=2 ;
      _numberOfConstituents=new int[2] ;
      _numberOfConstituents[0]=6 ;
      _numberOfConstituents[1]=12 ;
      _numberOfNodeOfEachConstituent=new int*[2] ;
      _numberOfNodeOfEachConstituent[0]=new int[6] ;
      _numberOfNodeOfEachConstituent[0][0]=8 ;
      _numberOfNodeOfEachConstituent[0][1]=8 ;
      _numberOfNodeOfEachConstituent[0][2]=8 ;
      _numberOfNodeOfEachConstituent[0][3]=8 ;
      _numberOfNodeOfEachConstituent[0][4]=8 ;
      _numberOfNodeOfEachConstituent[0][5]=8 ;
      _numberOfNodeOfEachConstituent[1]=new int[12] ;
      _numberOfNodeOfEachConstituent[1][0]=3 ;
      _numberOfNodeOfEachConstituent[1][1]=3 ;
      _numberOfNodeOfEachConstituent[1][2]=3 ;
      _numberOfNodeOfEachConstituent[1][3]=3 ;
      _numberOfNodeOfEachConstituent[1][4]=3 ;
      _numberOfNodeOfEachConstituent[1][5]=3 ;
      _numberOfNodeOfEachConstituent[1][6]=3 ;
      _numberOfNodeOfEachConstituent[1][7]=3 ;
      _numberOfNodeOfEachConstituent[1][8]=3 ;
      _numberOfNodeOfEachConstituent[1][9]=3 ;
      _numberOfNodeOfEachConstituent[1][10]=3 ;
      _numberOfNodeOfEachConstituent[1][11]=3 ;

      int* _edge1=new int[3];
      _edge1[0]=1;
      _edge1[1]=2;
      _edge1[2]=9;
      int* _edge2=new int[3];
      _edge2[0]=2;
      _edge2[1]=3;
      _edge2[2]=10;
      int* _edge3=new int[3];
      _edge3[0]=3;
      _edge3[1]=4;
      _edge3[2]=11;
      int* _edge4=new int[3];
      _edge4[0]=4;
      _edge4[1]=1;
      _edge4[2]=12;
      int* _edge5=new int[3];
      _edge5[0]=5;
      _edge5[1]=6;
      _edge5[2]=13;
      int* _edge6=new int[3];
      _edge6[0]=6;
      _edge6[1]=7;
      _edge6[2]=14;
      int* _edge7=new int[3];
      _edge7[0]=7;
      _edge7[1]=8;
      _edge7[2]=15;
      int* _edge8=new int[3];
      _edge8[0]=8;
      _edge8[1]=5;
      _edge8[2]=16;
      int* _edge9=new int[3];
      _edge9[0]=1;
      _edge9[1]=5;
      _edge9[2]=17;
      int* _edge10=new int[3];
      _edge10[0]=2;
      _edge10[1]=6;
      _edge10[2]=18;
      int* _edge11=new int[3];
      _edge11[0]=3;
      _edge11[1]=7;
      _edge11[2]=19;
      int* _edge12=new int[3];
      _edge12[0]=4;
      _edge12[1]=8;
      _edge12[2]=20;
      int* _face1=new int[8];
      _face1[0]=1;
      _face1[1]=2;
      _face1[2]=3;
      _face1[3]=4;
      _face1[4]=9;
      _face1[5]=10;
      _face1[6]=11;
      _face1[7]=12;
      int* _face2=new int[8];
      _face2[0]=5;
      _face2[1]=8;
      _face2[2]=7;
      _face2[3]=6;
      _face2[4]=16;
      _face2[5]=15;
      _face2[6]=14;
      _face2[7]=13;
      int* _face3=new int[8];
      _face3[0]=1;
      _face3[1]=5;
      _face3[2]=6;
      _face3[3]=2;
      _face3[4]=17;
      _face3[5]=13;
      _face3[6]=18;
      _face3[7]=9;
      int* _face4=new int[8];
      _face4[0]=2;
      _face4[1]=6;
      _face4[2]=7;
      _face4[3]=3;
      _face4[4]=18;
      _face4[5]=14;
      _face4[6]=19;
      _face4[7]=10;
      int* _face5=new int[8];
      _face5[0]=3;
      _face5[1]=7;
      _face5[2]=8;
      _face5[3]=4;
      _face5[4]=19;
      _face5[5]=15;
      _face5[6]=20;
      _face5[7]=11;
     int* _face6=new int[8];
      _face6[0]=4;
      _face6[1]=8;
      _face6[2]=5;
      _face6[3]=1;
      _face6[4]=20;
      _face6[5]=16;
      _face6[6]=17;
      _face6[7]=12;
      int ** tmpConstituents1 = new int*[6];
      tmpConstituents1[0]=_face1 ;
      tmpConstituents1[1]=_face2 ;
      tmpConstituents1[2]=_face3 ;
      tmpConstituents1[3]=_face4 ;
      tmpConstituents1[4]=_face5 ;
      tmpConstituents1[5]=_face6 ;
      int ** tmpConstituents2 = new int*[12];
      tmpConstituents2[0]=_edge1 ;
      tmpConstituents2[1]=_edge2 ;
      tmpConstituents2[2]=_edge3 ;
      tmpConstituents2[3]=_edge4 ;
      tmpConstituents2[4]=_edge5 ;
      tmpConstituents2[5]=_edge6 ;
      tmpConstituents2[6]=_edge7 ;
      tmpConstituents2[7]=_edge8 ;
      tmpConstituents2[8]=_edge9 ;
      tmpConstituents2[9]=_edge10;
      tmpConstituents2[10]=_edge11;
      tmpConstituents2[11]=_edge12;
      _constituents = new int**[2] ;
      _constituents[0]=tmpConstituents1 ;
      _constituents[1]=tmpConstituents2 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[6] ;
      tmpConstituentsType1[0] = MED_EN::MED_QUAD8 ;
      tmpConstituentsType1[1] = MED_EN::MED_QUAD8 ;
      tmpConstituentsType1[2] = MED_EN::MED_QUAD8 ;
      tmpConstituentsType1[3] = MED_EN::MED_QUAD8 ;
      tmpConstituentsType1[4] = MED_EN::MED_QUAD8 ;
      tmpConstituentsType1[5] = MED_EN::MED_QUAD8 ;
      medGeometryElement * tmpConstituentsType2 = new medGeometryElement[12] ;
      tmpConstituentsType2[0] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[1] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[2] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[3] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[4] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[5] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[6] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[7] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[8] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[9] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[10] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[11] = MED_EN::MED_SEG3 ;
      _constituentsType = new medGeometryElement*[2] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      _constituentsType[1]=tmpConstituentsType2 ;
      break;
    }
    case MED_EN::MED_PENTA6  : {
      _name="MED_PENTA6" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=6;
      _numberOfNodes=6;

      _numberOfConstituentsDimension=2 ;
      _numberOfConstituents=new int[2] ;
      _numberOfConstituents[0]=5 ;
      _numberOfConstituents[1]=9 ;
      _numberOfNodeOfEachConstituent=new int*[2] ;
      _numberOfNodeOfEachConstituent[0]=new int[5] ;
      _numberOfNodeOfEachConstituent[0][0]=3 ;
      _numberOfNodeOfEachConstituent[0][1]=3 ;
      _numberOfNodeOfEachConstituent[0][2]=4 ;
      _numberOfNodeOfEachConstituent[0][3]=4 ;
      _numberOfNodeOfEachConstituent[0][4]=4 ;
      _numberOfNodeOfEachConstituent[1]=new int[9] ;
      _numberOfNodeOfEachConstituent[1][0]=2 ;
      _numberOfNodeOfEachConstituent[1][1]=2 ;
      _numberOfNodeOfEachConstituent[1][2]=2 ;
      _numberOfNodeOfEachConstituent[1][3]=2 ;
      _numberOfNodeOfEachConstituent[1][4]=2 ;
      _numberOfNodeOfEachConstituent[1][5]=2 ;
      _numberOfNodeOfEachConstituent[1][6]=2 ;
      _numberOfNodeOfEachConstituent[1][7]=2 ;
      _numberOfNodeOfEachConstituent[1][8]=2 ;

      int* _edge1=new int[2];
      _edge1[0]=1;
      _edge1[1]=2;
      int* _edge2=new int[2];
      _edge2[0]=2;
      _edge2[1]=3;
      int* _edge3=new int[2];
      _edge3[0]=3;
      _edge3[1]=1;
      int* _edge4=new int[2];
      _edge4[0]=4;
      _edge4[1]=5;
      int* _edge5=new int[2];
      _edge5[0]=5;
      _edge5[1]=6;
      int* _edge6=new int[2];
      _edge6[0]=6;
      _edge6[1]=4;
      int* _edge7=new int[2];
      _edge7[0]=1;
      _edge7[1]=4;
      int* _edge8=new int[2];
      _edge8[0]=2;
      _edge8[1]=5;
      int* _edge9=new int[2];
      _edge9[0]=3;
      _edge9[1]=6;
      int* _face1=new int[3];
      _face1[0]=1;
      _face1[1]=2;
      _face1[2]=3;
      int* _face2=new int[3];
      _face2[0]=4;
      _face2[1]=6;
      _face2[2]=5;
      int* _face3=new int[4];
      _face3[0]=1;
      _face3[1]=4;
      _face3[2]=5;
      _face3[3]=2;
      int* _face4=new int[4];
      _face4[0]=2;
      _face4[1]=5;
      _face4[2]=6;
      _face4[3]=3;
      int* _face5=new int[4];
      _face5[0]=3;
      _face5[1]=6;
      _face5[2]=4;
      _face5[3]=1;
      int ** tmpConstituents1 = new int*[5];
      tmpConstituents1[0]=_face1 ;
      tmpConstituents1[1]=_face2 ;
      tmpConstituents1[2]=_face3 ;
      tmpConstituents1[3]=_face4 ;
      tmpConstituents1[4]=_face5 ;
      int ** tmpConstituents2 = new int*[9];
      tmpConstituents2[0]=_edge1 ;
      tmpConstituents2[1]=_edge2 ;
      tmpConstituents2[2]=_edge3 ;
      tmpConstituents2[3]=_edge4 ;
      tmpConstituents2[4]=_edge5 ;
      tmpConstituents2[5]=_edge6 ;
      tmpConstituents2[6]=_edge7 ;
      tmpConstituents2[7]=_edge8 ;
      tmpConstituents2[8]=_edge9 ;
      _constituents = new int**[2] ;
      _constituents[0]=tmpConstituents1 ;
      _constituents[1]=tmpConstituents2 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[5] ;
      tmpConstituentsType1[0] = MED_EN::MED_TRIA3 ;
      tmpConstituentsType1[1] = MED_EN::MED_TRIA3 ;
      tmpConstituentsType1[2] = MED_EN::MED_QUAD4 ;
      tmpConstituentsType1[3] = MED_EN::MED_QUAD4 ;
      tmpConstituentsType1[4] = MED_EN::MED_QUAD4 ;
      medGeometryElement * tmpConstituentsType2 = new medGeometryElement[9] ;
      tmpConstituentsType2[0] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[1] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[2] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[3] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[4] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[5] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[6] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[7] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[8] = MED_EN::MED_SEG2 ;
      _constituentsType = new medGeometryElement*[2] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      _constituentsType[1]=tmpConstituentsType2 ;
      break;
    }
    case MED_EN::MED_PENTA15 : {
      _name="MED_PENTA15" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=6;
      _numberOfNodes=15;
      _numberOfConstituentsDimension=2 ;
      _numberOfConstituents=new int[2] ;
      _numberOfConstituents[0]=5 ;
      _numberOfConstituents[1]=9 ;
      _numberOfNodeOfEachConstituent=new int*[2] ;
      _numberOfNodeOfEachConstituent[0]=new int[5] ;
      _numberOfNodeOfEachConstituent[0][0]=6 ;
      _numberOfNodeOfEachConstituent[0][1]=6 ;
      _numberOfNodeOfEachConstituent[0][2]=8 ;
      _numberOfNodeOfEachConstituent[0][3]=8 ;
      _numberOfNodeOfEachConstituent[0][4]=8 ;
      _numberOfNodeOfEachConstituent[1]=new int[9] ;
      _numberOfNodeOfEachConstituent[1][0]=3 ;
      _numberOfNodeOfEachConstituent[1][1]=3 ;
      _numberOfNodeOfEachConstituent[1][2]=3 ;
      _numberOfNodeOfEachConstituent[1][3]=3 ;
      _numberOfNodeOfEachConstituent[1][4]=3 ;
      _numberOfNodeOfEachConstituent[1][5]=3 ;
      _numberOfNodeOfEachConstituent[1][6]=3 ;
      _numberOfNodeOfEachConstituent[1][7]=3 ;
      _numberOfNodeOfEachConstituent[1][8]=3 ;

      int* _edge1=new int[3];
      _edge1[0]=1;
      _edge1[1]=2;
      _edge1[2]=7;
      int* _edge2=new int[3];
      _edge2[0]=2;
      _edge2[1]=3;
      _edge2[2]=8;
      int* _edge3=new int[3];
      _edge3[0]=3;
      _edge3[1]=1;
      _edge3[2]=9;
      int* _edge4=new int[3];
      _edge4[0]=4;
      _edge4[1]=5;
      _edge4[2]=10;
      int* _edge5=new int[3];
      _edge5[0]=5;
      _edge5[1]=6;
      _edge5[2]=11;
      int* _edge6=new int[3];
      _edge6[0]=6;
      _edge6[1]=4;
      _edge6[2]=12;
      int* _edge7=new int[3];
      _edge7[0]=1;
      _edge7[1]=4;
      _edge7[2]=13;
      int* _edge8=new int[3];
      _edge8[0]=2;
      _edge8[1]=5;
      _edge8[2]=14;
      int* _edge9=new int[3];
      _edge9[0]=3;
      _edge9[1]=6;
      _edge9[2]=15;
      int* _face1=new int[6];
      _face1[0]=1;
      _face1[1]=2;
      _face1[2]=3;
      _face1[3]=7;
      _face1[4]=8;
      _face1[5]=9;
      int* _face2=new int[6];
      _face2[0]=4;
      _face2[1]=5;
      _face2[2]=6;
      _face2[3]=12;
      _face2[4]=11;
      _face2[5]=10;
      int* _face3=new int[8];
      _face3[0]=1;
      _face3[1]=4;
      _face3[2]=5;
      _face3[3]=2;
      _face3[4]=13;
      _face3[5]=10;
      _face3[6]=14;
      _face3[7]=7;
      int* _face4=new int[8];
      _face4[0]=2;
      _face4[1]=5;
      _face4[2]=6;
      _face4[3]=3;
      _face4[4]=14;
      _face4[5]=11;
      _face4[6]=15;
      _face4[7]=8;
      int* _face5=new int[8];
      _face5[0]=3;
      _face5[1]=6;
      _face5[2]=4;
      _face5[3]=1;
      _face5[4]=15;
      _face5[5]=12;
      _face5[6]=13;
      _face5[7]=9;
      int ** tmpConstituents1 = new int*[5];
      tmpConstituents1[0]=_face1 ;
      tmpConstituents1[1]=_face2 ;
      tmpConstituents1[2]=_face3 ;
      tmpConstituents1[3]=_face4 ;
      tmpConstituents1[4]=_face5 ;
      int ** tmpConstituents2 = new int*[9];
      tmpConstituents2[0]=_edge1 ;
      tmpConstituents2[1]=_edge2 ;
      tmpConstituents2[2]=_edge3 ;
      tmpConstituents2[3]=_edge4 ;
      tmpConstituents2[4]=_edge5 ;
      tmpConstituents2[5]=_edge6 ;
      tmpConstituents2[6]=_edge7 ;
      tmpConstituents2[7]=_edge8 ;
      tmpConstituents2[8]=_edge9 ;
      _constituents = new int**[2] ;
      _constituents[0]=tmpConstituents1 ;
      _constituents[1]=tmpConstituents2 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[5] ;
      tmpConstituentsType1[0] = MED_EN::MED_TRIA6 ;
      tmpConstituentsType1[1] = MED_EN::MED_TRIA6 ;
      tmpConstituentsType1[2] = MED_EN::MED_QUAD8 ;
      tmpConstituentsType1[3] = MED_EN::MED_QUAD8 ;
      tmpConstituentsType1[4] = MED_EN::MED_QUAD8 ;
      medGeometryElement * tmpConstituentsType2 = new medGeometryElement[9] ;
      tmpConstituentsType2[0] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[1] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[2] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[3] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[4] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[5] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[6] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[7] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[8] = MED_EN::MED_SEG3 ;
      _constituentsType = new medGeometryElement*[2] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      _constituentsType[1]=tmpConstituentsType2 ;
      break ;
    }
    case MED_EN::MED_PYRA5   : {
      _name="MED_PYRA5" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=5;
      _numberOfNodes=5;
      _numberOfConstituentsDimension=2 ;
      _numberOfConstituents=new int[2] ;
      _numberOfConstituents[0]=5 ;
      _numberOfConstituents[1]=8 ;
      _numberOfNodeOfEachConstituent=new int*[2] ;
      _numberOfNodeOfEachConstituent[0]=new int[5] ;
      _numberOfNodeOfEachConstituent[0][0]=4 ;
      _numberOfNodeOfEachConstituent[0][1]=3 ;
      _numberOfNodeOfEachConstituent[0][2]=3 ;
      _numberOfNodeOfEachConstituent[0][3]=3 ;
      _numberOfNodeOfEachConstituent[0][4]=3 ;
      _numberOfNodeOfEachConstituent[1]=new int[8] ;
      _numberOfNodeOfEachConstituent[1][0]=2 ;
      _numberOfNodeOfEachConstituent[1][1]=2 ;
      _numberOfNodeOfEachConstituent[1][2]=2 ;
      _numberOfNodeOfEachConstituent[1][3]=2 ;
      _numberOfNodeOfEachConstituent[1][4]=2 ;
      _numberOfNodeOfEachConstituent[1][5]=2 ;
      _numberOfNodeOfEachConstituent[1][6]=2 ;
      _numberOfNodeOfEachConstituent[1][7]=2 ;

      int* _edge1=new int[2];
      _edge1[0]=1;
      _edge1[1]=2;
      int* _edge2=new int[2];
      _edge2[0]=2;
      _edge2[1]=3;
      int* _edge3=new int[2];
      _edge3[0]=3;
      _edge3[1]=4;
      int* _edge4=new int[2];
      _edge4[0]=4;
      _edge4[1]=1;
      int* _edge5=new int[2];
      _edge5[0]=1;
      _edge5[1]=5;
      int* _edge6=new int[2];
      _edge6[0]=2;
      _edge6[1]=5;
      int* _edge7=new int[2];
      _edge7[0]=3;
      _edge7[1]=5;
      int* _edge8=new int[2];
      _edge8[0]=4;
      _edge8[1]=5;
      int* _face1=new int[4];
      _face1[0]=1;
      _face1[1]=2;
      _face1[2]=3;
      _face1[3]=4;
      int* _face2=new int[3];
      _face2[0]=1;
      _face2[1]=5;
      _face2[2]=2;
      int* _face3=new int[3];
      _face3[0]=2;
      _face3[1]=5;
      _face3[2]=3;
      int* _face4=new int[3];
      _face4[0]=3;
      _face4[1]=5;
      _face4[2]=4;
      int* _face5=new int[3];
      _face5[0]=4;
      _face5[1]=5;
      _face5[2]=1;
      int ** tmpConstituents1 = new int*[5];
      tmpConstituents1[0]=_face1 ;
      tmpConstituents1[1]=_face2 ;
      tmpConstituents1[2]=_face3 ;
      tmpConstituents1[3]=_face4 ;
      tmpConstituents1[4]=_face5 ;
      int ** tmpConstituents2 = new int*[8];
      tmpConstituents2[0]=_edge1 ;
      tmpConstituents2[1]=_edge2 ;
      tmpConstituents2[2]=_edge3 ;
      tmpConstituents2[3]=_edge4 ;
      tmpConstituents2[4]=_edge5 ;
      tmpConstituents2[5]=_edge6 ;
      tmpConstituents2[6]=_edge7 ;
      tmpConstituents2[7]=_edge8 ;
      _constituents = new int**[2] ;
      _constituents[0]=tmpConstituents1 ;
      _constituents[1]=tmpConstituents2 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[5] ;
      tmpConstituentsType1[0] = MED_EN::MED_QUAD4 ;
      tmpConstituentsType1[1] = MED_EN::MED_TRIA3 ;
      tmpConstituentsType1[2] = MED_EN::MED_TRIA3 ;
      tmpConstituentsType1[3] = MED_EN::MED_TRIA3 ;
      tmpConstituentsType1[4] = MED_EN::MED_TRIA3 ;
      medGeometryElement * tmpConstituentsType2 = new medGeometryElement[8] ;
      tmpConstituentsType2[0] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[1] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[2] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[3] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[4] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[5] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[6] = MED_EN::MED_SEG2 ;
      tmpConstituentsType2[7] = MED_EN::MED_SEG2 ;
      _constituentsType = new medGeometryElement*[2] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      _constituentsType[1]=tmpConstituentsType2 ;
      break;
    }
    case MED_EN::MED_PYRA13  : {
      _name="MED_PYRA13" ;
      _type=t;
      _dimension=3;
      _numberOfVertexes=5;
      _numberOfNodes=13;
      _numberOfConstituentsDimension=2 ;
      _numberOfConstituents=new int[2] ;
      _numberOfConstituents[0]=5 ;
      _numberOfConstituents[1]=8 ;
      _numberOfNodeOfEachConstituent=new int*[2] ;
      _numberOfNodeOfEachConstituent[0]=new int[5] ;
      _numberOfNodeOfEachConstituent[0][0]=8 ;
      _numberOfNodeOfEachConstituent[0][1]=6 ;
      _numberOfNodeOfEachConstituent[0][2]=6 ;
      _numberOfNodeOfEachConstituent[0][3]=6 ;
      _numberOfNodeOfEachConstituent[0][4]=6 ;
      _numberOfNodeOfEachConstituent[1]=new int[8] ;
      _numberOfNodeOfEachConstituent[1][0]=3 ;
      _numberOfNodeOfEachConstituent[1][1]=3 ;
      _numberOfNodeOfEachConstituent[1][2]=3 ;
      _numberOfNodeOfEachConstituent[1][3]=3 ;
      _numberOfNodeOfEachConstituent[1][4]=3 ;
      _numberOfNodeOfEachConstituent[1][5]=3 ;
      _numberOfNodeOfEachConstituent[1][6]=3 ;
      _numberOfNodeOfEachConstituent[1][7]=3 ;

      int* _edge1=new int[3];
      _edge1[0]=1;
      _edge1[1]=2;
      _edge1[2]=6;
      int* _edge2=new int[3];
      _edge2[0]=2;
      _edge2[1]=3;
      _edge2[2]=7;
      int* _edge3=new int[3];
      _edge3[0]=3;
      _edge3[1]=4;
      _edge3[2]=8;
      int* _edge4=new int[3];
      _edge4[0]=4;
      _edge4[1]=1;
      _edge4[2]=9;
      int* _edge5=new int[3];
      _edge5[0]=1;
      _edge5[1]=5;
      _edge5[2]=10;
      int* _edge6=new int[3];
      _edge6[0]=2;
      _edge6[1]=5;
      _edge6[2]=11;
      int* _edge7=new int[3];
      _edge7[0]=3;
      _edge7[1]=5;
      _edge7[2]=12;
      int* _edge8=new int[3];
      _edge8[0]=4;
      _edge8[1]=5;
      _edge8[2]=13;
      int* _face1=new int[8];
      _face1[0]=1;
      _face1[1]=2;
      _face1[2]=3;
      _face1[3]=4;
      _face1[4]=6;
      _face1[5]=7;
      _face1[6]=8;
      _face1[7]=9;
      int* _face2=new int[6];
      _face2[0]=1;
      _face2[1]=5;
      _face2[2]=2;
      _face2[3]=10;
      _face2[4]=11;
      _face2[5]=6;
      int* _face3=new int[6];
      _face3[0]=2;
      _face3[1]=5;
      _face3[2]=3;
      _face3[3]=11;
      _face3[4]=12;
      _face3[5]=7;
      int* _face4=new int[6];
      _face4[0]=3;
      _face4[1]=5;
      _face4[2]=4;
      _face4[3]=12;
      _face4[4]=13;
      _face4[5]=8;
      int* _face5=new int[6];
      _face5[0]=4;
      _face5[1]=5;
      _face5[2]=1;
      _face5[3]=13;
      _face5[4]=10;
      _face5[5]=9;
      int ** tmpConstituents1 = new int*[5];
      tmpConstituents1[0]=_face1 ;
      tmpConstituents1[1]=_face2 ;
      tmpConstituents1[2]=_face3 ;
      tmpConstituents1[3]=_face4 ;
      tmpConstituents1[4]=_face5 ;
      int ** tmpConstituents2 = new int*[8];
      tmpConstituents2[0]=_edge1 ;
      tmpConstituents2[1]=_edge2 ;
      tmpConstituents2[2]=_edge3 ;
      tmpConstituents2[3]=_edge4 ;
      tmpConstituents2[4]=_edge5 ;
      tmpConstituents2[5]=_edge6 ;
      tmpConstituents2[6]=_edge7 ;
      tmpConstituents2[7]=_edge8 ;
      _constituents = new int**[2] ;
      _constituents[0]=tmpConstituents1 ;
      _constituents[1]=tmpConstituents2 ;
      medGeometryElement * tmpConstituentsType1 = new medGeometryElement[5] ;
      tmpConstituentsType1[0] = MED_EN::MED_QUAD8 ;
      tmpConstituentsType1[1] = MED_EN::MED_TRIA6 ;
      tmpConstituentsType1[2] = MED_EN::MED_TRIA6 ;
      tmpConstituentsType1[3] = MED_EN::MED_TRIA6 ;
      tmpConstituentsType1[4] = MED_EN::MED_TRIA6 ;
      medGeometryElement * tmpConstituentsType2 = new medGeometryElement[8] ;
      tmpConstituentsType2[0] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[1] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[2] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[3] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[4] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[5] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[6] = MED_EN::MED_SEG3 ;
      tmpConstituentsType2[7] = MED_EN::MED_SEG3 ;
      _constituentsType = new medGeometryElement*[2] ;
      _constituentsType[0]=tmpConstituentsType1 ;
      _constituentsType[1]=tmpConstituentsType2 ;
      break;
    }
    case MED_POLYGON:
      _name="MED_POLYGON" ;
      _type=t;
      _dimension=2;
      _numberOfConstituentsDimension=1 ;
      _numberOfConstituents=new int[_numberOfConstituentsDimension] ;
      _numberOfConstituents[0]=0 ;
      _numberOfNodeOfEachConstituent=new int*[_numberOfConstituentsDimension] ;
      _numberOfNodeOfEachConstituent[0]=0;
      _constituentsType = new medGeometryElement*[_numberOfConstituentsDimension] ;
      _constituentsType[0]=0 ;
      _constituents = new int**[_numberOfConstituentsDimension] ;
      _constituents[0]=0;
     break;
    case MED_POLYHEDRA:
      _name="MED_POLYHEDRA" ;
      _type=t;
      _dimension=3;
      _numberOfConstituentsDimension=2 ;
      _numberOfConstituents=new int[_numberOfConstituentsDimension] ;
      _numberOfConstituents[0]=_numberOfConstituents[1]=0;
      _numberOfNodeOfEachConstituent=new int*[_numberOfConstituentsDimension] ;
      _numberOfNodeOfEachConstituent[0]=_numberOfNodeOfEachConstituent[1]=0;
      _constituentsType = new medGeometryElement*[_numberOfConstituentsDimension] ;
      _constituentsType[0]=_constituentsType[1]=0 ;
      _constituents = new int**[_numberOfConstituentsDimension] ;
      _constituents[0]=_constituents[1]=0;
      break;
//      default : 
//        _type=0;
//        break;
    }
}

ostream & MEDMEM::operator<<(ostream &os,const CELLMODEL &my)
{
  os << "Cell Model :" << endl ;
  os << "  - name               : " << my._name << endl;
  os << "  - type               : " << (int)my._type << endl;
  os << "  - dimension          : " << my._dimension << endl;
  os << "  - number of nodes    : " << my._numberOfNodes << endl ;
  os << "  - number of vertexes : " << my._numberOfVertexes << endl ;

  os << "  - number of Constituents Dimension : " << my._numberOfConstituentsDimension << endl ;

  for(int i=0;i<my._numberOfConstituentsDimension;i++)
    {

      os << "  - for dimension (-" << i+1 << ") : " << endl ;
      os << "    - number of Constituents for this Dimension : " << my._numberOfConstituents[i] << endl ;

      for(int j=0;j<my._numberOfConstituents[i];j++)
        {
          os << "    - number of node for this constituent : " << my._numberOfNodeOfEachConstituent[i][j] << endl ;
          os << "      - constituents " << j+1 << " of type "<< my._constituentsType[i][j] <<" : " ;
          for(int k=0;k<my._numberOfNodeOfEachConstituent[i][j];k++)
            os << my._constituents[i][j][k] << " " ;
          os << endl ;
        }
    }
  return os;
}

// Return number of constituents type (which dimension is _dimension-1).
int CELLMODEL::getNumberOfConstituentsType() const
{
  set<medGeometryElement> constituentsTypes ;
//   if (_constituentsType.size() > 0 ) {
//     vector<medGeometryElement> constituentsType = _constituentsType[0] ;
//     vector<medGeometryElement>::iterator itvec ;
//     for(itvec=constituentsType.begin();itvec!=constituentsType.end();itvec++)
//       constituentsTypes.insert(*itvec);
//   }
  return constituentsTypes.size() ;
}

  // Return all types of constituents which dimension is (_dimension-1).
set<medGeometryElement>  CELLMODEL::getAllConstituentsType() const
{
  set<medGeometryElement> constituentsTypes ;
//   if (_constituentsType.size() > 0 ) {
//     vector<medGeometryElement> constituentsType = _constituentsType[0] ;
//     vector<medGeometryElement>::iterator itvec ;
//     for(itvec=constituentsType.begin();itvec!=constituentsType.end();itvec++)
//       constituentsTypes.insert(*itvec);
//   }
  return constituentsTypes ;
}

  // Return number of constituents foreach type (which dimension is _dimension-1).
map<medGeometryElement,int>  CELLMODEL::getNumberOfConstituentsForeachType() const
{
  map<medGeometryElement,int> numberOfConstituentsForeachType ;
//   map<medGeometryElement,int>::iterator itmap ;
//   if (_constituentsType.size() > 0 ) {
//     vector<medGeometryElement> constituentsType = _constituentsType[0] ;
//     vector<medGeometryElement>::iterator itvec ;
//     for(itvec=constituentsType.begin();itvec!=constituentsType.end();itvec++) {
//       itmap = numberOfConstituentsForeachType.find(*itvec);
//       if (itmap==numberOfConstituentsForeachType.end()) // new element
//      numberOfConstituentsForeachType[*itvec]=1 ;
//       else
//      numberOfConstituentsForeachType[*itvec]++ ;
//     }
//   }
  return numberOfConstituentsForeachType ;
}

void CELLMODEL::init(const CELLMODEL &m)
{
  _name=m._name ;
  _type= m._type ;
  _dimension = m._dimension ;
  _numberOfNodes = m._numberOfNodes ;
  _numberOfVertexes = m._numberOfVertexes ;
  
  _numberOfConstituentsDimension = m._numberOfConstituentsDimension ;
  
  _numberOfConstituents = new int[_numberOfConstituentsDimension] ;
  for(int i=0; i<_numberOfConstituentsDimension; i++)
    _numberOfConstituents[i]=m._numberOfConstituents[i] ;
  
  _numberOfNodeOfEachConstituent = new int*[_numberOfConstituentsDimension] ;
  for(int i=0; i<_numberOfConstituentsDimension; i++) {
    int numberOf = _numberOfConstituents[i] ;
    int * newArray = new int[numberOf] ;
    int * oldArray = m._numberOfNodeOfEachConstituent[i] ;
    for(int j=0; j<numberOf; j++)
      newArray[j] = oldArray[j] ;
    _numberOfNodeOfEachConstituent[i] = newArray ;
  }
  _constituents = new int**[_numberOfConstituentsDimension] ;
  if(_constituentsType)
    {
      for(int i=0; i<_numberOfConstituentsDimension; i++)
        delete [] _constituentsType[i];
      delete [] _constituentsType;
    }
  _constituentsType = new medGeometryElement*[_numberOfConstituentsDimension] ;
  for(int i=0; i<_numberOfConstituentsDimension; i++) {
    int numberOf = _numberOfConstituents[i] ;
    int ** tmpArray = new int*[numberOf] ;
    medGeometryElement * newArrayType = new medGeometryElement[numberOf] ;
    medGeometryElement * oldArrayType = m._constituentsType[i] ;
    
    for(int j=0; j<numberOf; j++) {
      int numberOf2 = _numberOfNodeOfEachConstituent[i][j] ;
      int * newArray = new int[numberOf2] ;
      int * oldArray = m._constituents[i][j] ;
      for(int k=0; k<numberOf2; k++)
        newArray[k] = oldArray[k] ;
      tmpArray[j] = newArray ;
      newArrayType[j] = oldArrayType[j] ;
    }
    _constituents[i] = tmpArray ;
    _constituentsType[i] = newArrayType ;
  }
  
}
void CELLMODEL::clean()
{
  for(int i=0; i<_numberOfConstituentsDimension; i++) {
    int numberOf = _numberOfConstituents[i] ;
    for(int j=0; j<numberOf; j++) {
      if (_constituents[i][j])
        delete[] _constituents[i][j] ;
    }
    if (_constituentsType[i])
      delete [] _constituentsType[i] ;
    if (_constituents[i])
      delete[] _constituents[i] ;
    if (_numberOfNodeOfEachConstituent[i])
      delete[] _numberOfNodeOfEachConstituent[i] ;
  }
  delete [] _constituentsType; 
  if (_numberOfConstituents)
    delete[]_numberOfConstituents ;
  if (_constituents)
    delete[] _constituents ;
  if (_numberOfNodeOfEachConstituent)
    delete[] _numberOfNodeOfEachConstituent ;

  _constituentsType = 0;
  _numberOfConstituents = 0;
  _constituents = 0;
  _numberOfNodeOfEachConstituent = 0;
}
