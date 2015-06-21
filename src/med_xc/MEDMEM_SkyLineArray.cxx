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

#include "MEDMEM_SkyLineArray.hxx"
#include "MEDMEM_Utilities.hxx"
#include <map>

using namespace std;
using namespace MEDMEM;

MEDSKYLINEARRAY::MEDSKYLINEARRAY(): _count(0), _length(0),
                                _index((int*)NULL),_value((int*)NULL)
{
  MESSAGE_MED("Constructeur MEDSKYLINEARRAY sans parametre");
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const MEDSKYLINEARRAY &myArray):
                                _count(myArray._count),_length(myArray._length),
                                _index(_count+1),_value(_length)
{
  const char* LOC = "MEDSKYLINEARRAY(const MEDSKYLINEARRAY &)";
  BEGIN_OF_MED(LOC);
  if ( myArray._index )
    memcpy(_index,myArray._index,sizeof(int)*(_count+1));
  if ( myArray._value )
    memcpy(_value,myArray._value,sizeof(int)*_length);
  END_OF_MED(LOC);
}

MEDSKYLINEARRAY::~MEDSKYLINEARRAY()
{
  MESSAGE_MED("Destructeur ~MEDSKYLINEARRAY");

  //if (_index != NULL) delete [] _index;
  //if (_value != NULL) delete [] _value;
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const int count, const int length):
                                _count(count), _length(length),
                                _index(_count+1),_value(_length)
{
        MESSAGE_MED("Constructeur MEDSKYLINEARRAY(count="<<count<<", length="<<length<<") avec parametres");
}

MEDSKYLINEARRAY::MEDSKYLINEARRAY(const int count, const int length,
                                 const int* index, const int* value,bool shallowCopy):
                                _count(count), _length(length)
{
//      MESSAGE_MED("Constructeur MEDSKYLINEARRAY(count="<<count<<", length="<<length<<") avec parametres");
                if(shallowCopy)
          {
            _index.setShallowAndOwnership(index);
            _value.setShallowAndOwnership(value);
          }
        else
          {
            _index.set(_count+1,index);
            _value.set(_length,value);
          }
}

//creates the reverse array
//creates an array with count maxvalue, where maxvalue
//is the maximum of the value_ of the present array
//length is the same as present array
//
// For instance
// 1 : 2 4
// 2 : 1 2
//
//will give
// 1 : 2
// 2 : 1 2
// 3 :
// 4 : 1

MEDSKYLINEARRAY* MEDSKYLINEARRAY::makeReverseArray()
{
        multimap<int,int > reverse;
        int size=0;
  for (int i=0; i<_count;i++)
                for (int j=_index[i];j<_index[i+1];j++)
                        {
                                int value=_value[j-1];
                                reverse.insert(make_pair(value,i+1));
                                if (value>size) size=value;
                        }
        int* r_index=new int [size+1];
        int* r_value=new int [_length];
        r_index[0]=1;
        pair<multimap<int,int>::iterator,multimap<int,int>::iterator>piter;
        int* ptr_value=r_value;
        for (int i=0; i<size;i++)
                {
                        piter=reverse.equal_range(i);
                        int index_incr=0;
                        for (multimap<int,int>::iterator iter=piter.first; iter!=piter.second; iter++)
                                {
                                        *ptr_value++=iter->second;
                                        index_incr++;
                                }
                        r_index[i+1]=r_index[i]+index_incr;
                }
        return new MEDSKYLINEARRAY(size,_length,r_index,r_value,true);
}
ostream& MEDMEM::operator<<(ostream &os, const MEDSKYLINEARRAY &sky) {
  os << "_count : " << sky._count << " ,_length : " << sky._length;
  for (int i = 0; i < sky._count ; i++) {
    os << endl << "Values of type n°" << i+1 << " (index["<<i<<"]="<< sky._index[i]<<") :" << endl;
    for (int j=sky._index[i]-1;j < sky._index[i+1]-1;j++)
      os << sky._value[j] << " " ;
  }
  return os;
}

//  void MEDSKYLINEARRAY::setMEDSKYLINEARRAY( const int count , const int length, int* index , int* value )
//  {
//    MESSAGE_MED("void MEDSKYLINEARRAY::setMEDSKYLINEARRAY(count, length, index, value)");
//    _count  = count  ;
//    _length = length ;

//    //if (_index != NULL) delete [] _index;
//    //if (_value != NULL) delete [] _value;

//        _index.set(index);
//        _value.set(value);
//  }
