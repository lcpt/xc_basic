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
 File MEDMEM_CellModel.hxx
*/

#ifndef CELLMODEL_HXX
#define CELLMODEL_HXX

#include "MEDMEM.hxx"

#include <set>
#include <map>
#include <string>

#include "MEDMEM_Utilities.hxx"
#include "MEDMEM_define.hxx"

using namespace std ;

/*!
  This class is an internal class and should not be used by the end-user.
  This class describes all possible cell models and is used in order
  to acces informations about geometric type of the cell : 
  Each object (if instancied), contains generic informations about
  the cell model it describes as cell dimensions, number of nodes...
*/


namespace MEDMEM {
  class CELLMODEL;
  MEDMEM_EXPORT ostream & operator<<(ostream &os,const CELLMODEL &my);

class MEDMEM_EXPORT CELLMODEL 
{

private:
                        /*! private method : \n
                            used by constructor and operator= */
  void init(const CELLMODEL &m);

                        /*! private method : \n */
  void clean();


  //protected:
                        /*! explicit name (as MED_POINT1)           */
  string             _name;
                        /*! type of cell (cf define.h)              */
  MED_EN::medGeometryElement _type;
                        /*! Cell dimension (not space dimension)    */
  int                _dimension;
                        /*! number of nodes forming this type of a cell    */
  int                _numberOfNodes;
                        /*! number of vertexes forming this type of a cell */
  int                _numberOfVertexes;
                        /*!  2 for a  3D Cell and 1 for a 2DCell */
  int                _numberOfConstituentsDimension;
                        /*! Array of size numberOfConstituentsDimension */
  int*               _numberOfConstituents ;
                        /*! Array of size _numberOfConstituentsDimension
                            x_numberOfConstituents[i]               */
  int**              _numberOfNodeOfEachConstituent ;
                        /*! defines nodal local connectivity for each 
                            constituents of each dimension: 
                            should be seen as a vector<vector<vector>> \n
                            - first vector : for each cell dimension 
                              (first : dim-1, second if any : dim-2)\n
                            - second vector : for each constituent of 
                              this dimension\n
                            - third vector : list of local nodes    */
  int***                _constituents ;
  MED_EN::medGeometryElement**  _constituentsType ;

public :

                        /*! Constructor. */
  inline CELLMODEL();
                        /*! Constructor. */
  CELLMODEL(MED_EN::medGeometryElement t);
                        /*! Copy constructor. */
  inline CELLMODEL(const CELLMODEL &m);
                        /*! Destructor. */
  inline ~CELLMODEL();

                        /*! Operator = : duplicate CELLMODEL. */
  inline CELLMODEL & operator=(const CELLMODEL &m);

                        /*! Operator << : print CELLMODEL to the given stream. */
  friend MEDMEM_EXPORT ostream & operator<<(ostream &os,const CELLMODEL &my);

                        /*! returns _name attribute (ie: MED_PENTA15).\n
                            see med.h (in med/include) */
  inline string                 getName()             const;

                        /*! returns number of vertexes forming this type of cell */
  inline int                    getNumberOfVertexes() const;

                        /*! returns number of nodes forming this type of cell    */
  inline int                    getNumberOfNodes()    const;

                        /*! returns the dimension of this type of cell.\n
                            it can be different from mesh dimension              */
  inline int                    getDimension()        const;

                        /*! returns the geometric type of the cell. \n
                            see med.h (in med/include) */
  inline MED_EN::medGeometryElement     getType()             const;

                        /*! returns all constituents which dimension is _dimension-dim.*/
  int** getConstituents(int dim) const;

                        /*! returns number of constituents which dimension is _dimension-dim.*/
  int   getNumberOfConstituents(int dim) const;

                        /*! returns local nodes numbers vector for num-th constituent 
                            which dimension is _dimension-dim.*/
  int* getNodesConstituent(int dim,int num) const;

                        /*! returns local node number of nodes_index-th node for 
                            num-th constituent which dimension is _dimension-dim.*/
  int getNodeConstituent(int dim,int num,int nodes_index) const;

                        /*! returns types of each constituents which dimension 
                            is _dimension-dim.*/
  MED_EN::medGeometryElement*  getConstituentsType(int dim) const;

                        /*! returns type of num-th constituent which dimension 
                            is _dimension-dim.*/
  MED_EN::medGeometryElement getConstituentType(int dim,int num) const;

                        /*! returns number of constituents type 
                            (which dimension is _dimension-1).*/
  int getNumberOfConstituentsType() const;

                        /*! returns all types of constituents which dimension 
                            is (_dimension-1).*/
  set<MED_EN::medGeometryElement>  getAllConstituentsType() const;

                        /*! returns number of constituents foreach type (which dimension 
                            is _dimension-1).*/
  map<MED_EN::medGeometryElement,int>  getNumberOfConstituentsForeachType() const;


};

// ------------------------------------------
//      Methodes Inline
// ------------------------------------------

inline CELLMODEL::CELLMODEL():
  _type(MED_EN::MED_NONE),
    _dimension(0),
    _numberOfNodes(0),
    _numberOfVertexes(0),
    _numberOfConstituentsDimension(0),
    _numberOfConstituents((int*)NULL),
    _numberOfNodeOfEachConstituent((int**)NULL),
    _constituents((int***)NULL),
    _constituentsType((MED_EN::medGeometryElement**)NULL)
{
}
inline CELLMODEL::CELLMODEL(const CELLMODEL &m):
    _type(MED_EN::MED_NONE),
    _dimension(0),
    _numberOfNodes(0),
    _numberOfVertexes(0),
    _numberOfConstituentsDimension(0),
    _numberOfConstituents((int*)NULL),
    _numberOfNodeOfEachConstituent((int**)NULL),
    _constituents((int***)NULL),
    _constituentsType((MED_EN::medGeometryElement**)NULL)
{
    init(m) ;
}
inline CELLMODEL::~CELLMODEL() 
{
  //  MESSAGE_MED("CELLMODEL::~CELLMODEL() destroying the cell");
  clean() ;
}
inline CELLMODEL & CELLMODEL::operator=(const CELLMODEL &m) 
{
    clean() ;
    init(m) ;
    return *this ;
}
inline string CELLMODEL::getName() const
{
  return _name ;
}
inline int CELLMODEL::getNumberOfVertexes() const
{
  return _numberOfVertexes;
}
inline int CELLMODEL::getNumberOfNodes() const
{
  return _numberOfNodes;
}
inline int CELLMODEL::getDimension() const
{
  return _dimension;
}
inline MED_EN::medGeometryElement CELLMODEL::getType() const
{
  return _type;
}
inline int** CELLMODEL::getConstituents(int dim) const
{
  return _constituents[dim-1] ;
}
inline int CELLMODEL::getNumberOfConstituents(int dim) const
{
  return _numberOfConstituents[dim-1] ;
}
inline int* CELLMODEL::getNodesConstituent(int dim,int num) const
{
  return _constituents[dim-1][num-1];
}
inline int CELLMODEL::getNodeConstituent(int dim,int num,int nodesNumber) const
{
  return _constituents[dim-1][num-1][nodesNumber-1] ;
}
inline MED_EN::medGeometryElement* CELLMODEL::getConstituentsType(int dim) const
{
  return _constituentsType[dim-1];
}
inline MED_EN::medGeometryElement CELLMODEL::getConstituentType(int dim,int num) const
{
  return _constituentsType[dim-1][num-1];
}

/*!: Utility  class for storing cell models. Avoids calling the cellmodel constructor too
often.
*/
class MEDMEM_EXPORT CELLMODEL_Map
{
public:
  static const MEDMEM::CELLMODEL& retrieveCellModel(MED_EN::medGeometryElement type);
private:
  static CELLMODEL_Map *getInstance();
  static CELLMODEL_Map *_singleton;
  std::map<MED_EN::medGeometryElement,MEDMEM::CELLMODEL> _cell_models;

  CELLMODEL_Map() {}
  ~CELLMODEL_Map(){ if(_singleton) delete _singleton;}

  const MEDMEM::CELLMODEL& getCellModel(MED_EN::medGeometryElement type);

};

}//End of namespace MEDMEM

#endif /* CELLMODEL_HXX */
