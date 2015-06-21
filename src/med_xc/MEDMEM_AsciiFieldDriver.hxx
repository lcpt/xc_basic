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

#ifndef ASCII_FIELD_DRIVER_HXX
#define ASCII_FIELD_DRIVER_HXX

#include "MEDMEM.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Unit.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_ArrayConvert.hxx"

#include <list>
#include <string>
#include <ctype.h>
#include <iomanip>
#include <stdlib.h>
#include <string.h>
#include <fstream>

namespace MEDMEM {

  const int    PRECISION_IN_ASCII_FILE = 10;
  const double PRECISION_IN_COMPARE    = 1e-10;
  const int    SPACE_BETWEEN_NBS       = 19;

  template<int N,unsigned int CODE>
  void fill(double *a, const double *b)
  {
    a[N]=b[CODE & 0x3 ];
    fill<N-1, (CODE>>2) >(a,b);
  }

  template<int N>
  bool compare(const double* a, const double* b)
  {
    double sign=b[N]<0 ? -1 : 1;
    if(a[N]<b[N]*(1-sign*PRECISION_IN_COMPARE))
      return true;
    if(a[N]>b[N]*(1+sign*PRECISION_IN_COMPARE))
      return false;
    return compare<N-1>(a,b);
  }

  template<> MEDMEM_EXPORT
  void fill<-1,0x3>(double *a, const double *b);

  template<> MEDMEM_EXPORT 
  bool compare<-1>(const double *a, const double *b);

  template <class T, int SPACEDIMENSION, unsigned int SORTSTRATEGY> 
  class SDForSorting
  {
  private:
    double _coords[SPACEDIMENSION];
    T *_components;
    int _nbComponents;
  public:
    SDForSorting(const double *coords, const T* comp, int nbComponents);
    SDForSorting(const SDForSorting& other);
    ~SDForSorting();
    bool operator< (const SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>& other) const;
    void writeLine(ofstream& file) const;
  };

  template <class T>
  class ASCII_FIELD_DRIVER : public GENDRIVER
  {
  private:
    MESH                   *_mesh;
    SUPPORT                *_support;
    mutable FIELD<T>       *_ptrField;
    std::string             _fileName;
    mutable ofstream        _file;
    unsigned int            _code;
    MED_EN::med_sort_direc  _direc;
    int                     _nbComponents;
    int                     _spaceDimension;
    //static int           _nbComponentsForCpyInfo;

  public:
    template <class INTERLACING_TAG>
    ASCII_FIELD_DRIVER():GENDRIVER(ASCII_DRIVER),
                         _ptrField((FIELD<T>)MED_NULL),
                         _fileName("") {}

    template <class INTERLACING_TAG>
    ASCII_FIELD_DRIVER(const string & fileName, FIELD<T,INTERLACING_TAG> * ptrField,
                       MED_EN::med_sort_direc direction=MED_EN::ASCENDING,
                       const char *priority="");


    ASCII_FIELD_DRIVER(const ASCII_FIELD_DRIVER<T>& other);
    void open() throw (MEDEXCEPTION);
    void close();
    void read ( void ) throw (MEDEXCEPTION);
    void write( void ) const throw (MEDEXCEPTION);
    GENDRIVER* copy() const;
  private:
    void buildIntroduction() const;
    template<int SPACEDIMENSION, unsigned int SORTSTRATEGY>
    void sortAndWrite() const;
    //template<int SPACEDIMENSION, unsigned int SORTSTRATEGY>//, std::list< SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY > > >
    //static void copyInfo(const double *a,T *b);
    //static void copyInfo2(const double *,T *);
  };
}


namespace MEDMEM {

  template <class T, int SPACEDIMENSION, unsigned int SORTSTRATEGY>
  SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>::SDForSorting(const double *coords, const T* comp, int nbComponents):_nbComponents(nbComponents)
  {
    fill<SPACEDIMENSION-1,SORTSTRATEGY>(_coords,coords);
    _components=new T[_nbComponents];
    memcpy(_components,comp,sizeof(T)*_nbComponents);
  }

  template <class T, int SPACEDIMENSION, unsigned int SORTSTRATEGY>
  SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>::SDForSorting(const SDForSorting& other):_nbComponents(other._nbComponents)
  {
    memcpy(_coords,other._coords,sizeof(double)*SPACEDIMENSION);
    _components=new T[_nbComponents];
    memcpy(_components,other._components,sizeof(T)*_nbComponents);
  }

  template <class T, int SPACEDIMENSION, unsigned int SORTSTRATEGY>
  SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>::~SDForSorting()
  {
    delete [] _components;
  }

  template <class T, int SPACEDIMENSION, unsigned int SORTSTRATEGY>
  bool SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>::operator< (const SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>& other) const
  {
    return compare<SPACEDIMENSION-1>(_coords,other._coords);
  }

  template <class T, int SPACEDIMENSION, unsigned int SORTSTRATEGY>
  void SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>::writeLine(ofstream& file) const
  {
    int i;
    double temp[SPACEDIMENSION];
    fill<SPACEDIMENSION-1,SORTSTRATEGY>(temp,_coords);
    for(i=0;i<SPACEDIMENSION;i++)
      file << setw(SPACE_BETWEEN_NBS) << temp[i];
    for(i=0;i<_nbComponents;i++)
      file << setw(SPACE_BETWEEN_NBS) << _components[i];
    file << endl;
  }

  template <class T>
  template <class INTERLACING_TAG>
  ASCII_FIELD_DRIVER<T>::ASCII_FIELD_DRIVER(const string & fileName,
                                            FIELD<T,INTERLACING_TAG> * ptrField,
                                            MED_EN::med_sort_direc direction,
                                            const char *priority)
    :GENDRIVER(fileName, MED_EN::WRONLY, ASCII_DRIVER),
     _ptrField((FIELD<T>*)ptrField),
     _fileName(fileName),
     _direc(direction)
    {
      _nbComponents=_ptrField->getNumberOfComponents();
      if(_nbComponents<=0)
        throw MEDEXCEPTION("ASCII_FIELD_DRIVER : No components in FIELD<T>");
      _support=(SUPPORT *)_ptrField->getSupport();
      _mesh=(MESH *)_support->getMesh();
      _spaceDimension=_mesh->getSpaceDimension();
      _code=3;
      int i;
      if(priority[0]=='\0')
        for(i=_spaceDimension-1;i>=0;i--)
          {
            _code<<=2;
            _code+=i;
          }
      else
        {
          if(_spaceDimension != (int)strlen(priority))
            throw MEDEXCEPTION("ASCII_FIELD_DRIVER : Coordinate priority invalid with spaceDim");
          for(i=_spaceDimension-1;i>=0;i--)
            {
              char c=toupper(priority[i]);
              if(int(c-'X')>(_spaceDimension-1) || int(c-'X')<0)
                throw MEDEXCEPTION("ASCII_FIELD_DRIVER : Invalid priority definition");
              _code<<=2;
              _code+=c-'X';
            }
        }
    }


  template <class T>
  ASCII_FIELD_DRIVER<T>::ASCII_FIELD_DRIVER(const ASCII_FIELD_DRIVER<T>& other):
    GENDRIVER(ASCII_DRIVER),
    _mesh(other._mesh),
    _support(other._support),
    _ptrField(other._ptrField),
    _fileName(other._fileName),
    _code(other._code),
    _direc(other._direc),
    _nbComponents(other._nbComponents),
    _spaceDimension(other._spaceDimension)
  {
  }

  template <class T>
  void ASCII_FIELD_DRIVER<T>::open() throw (MEDEXCEPTION)
  {
                if (_file.is_open())
                        throw MEDEXCEPTION("ASCII_FIELD_DRIVER::open() : file is already open !");
    _file.open(_fileName.c_str(),ofstream::out | ofstream::app);
    // for MEDMEMTest_AsciiFieldDriver.cxx:208 :
    // must throw because the file is opened
    //CPPUNIT_ASSERT_MED_THROW(aDriver1->setFileName("anyfile2"), MEDEXCEPTION);
    _status = _file.is_open() ? MED_OPENED : MED_INVALID;
  }

  template <class T>
  void ASCII_FIELD_DRIVER<T>::close()
  {
    _file.close();
    _status = MED_CLOSED;
  }

  template <class T>
  void ASCII_FIELD_DRIVER<T>::read ( void ) throw (MEDEXCEPTION)
  {
    throw MEDEXCEPTION("ASCII_FIELD_DRIVER::read : Can't read with a WRONLY driver !");
  }

  template <class T>
  GENDRIVER* ASCII_FIELD_DRIVER<T>::copy() const
  {
    return new ASCII_FIELD_DRIVER<T>(*this);
  }

  template <class T>
  void ASCII_FIELD_DRIVER<T>::write( void ) const throw (MEDEXCEPTION)
  {
                if (!_file.is_open()) 
                        throw MEDEXCEPTION("ASCII_FIELD_DRIVER::write : can't write a file that was not opened !");
                
    buildIntroduction();
    switch(_spaceDimension)
      {
      case 2:
        {
          switch(_code)
            {
            case 52: //XY
              {
                sortAndWrite<2,52>();
                break;
              }
            case 49: //YX
              {
                sortAndWrite<2,49>();
                break;
              }
            default:
              MEDEXCEPTION("ASCII_FIELD_DRIVER : Invalid priority definition");
            }
          break;
        }
        case 3:
        {
          switch(_code)
            {
            case 228: //XYZ
              {
                sortAndWrite<3,228>();
                break;
              }
            case 216: //XZY
              {
                sortAndWrite<3,216>();
                break;
              }
            case 225://YXZ
              {
                sortAndWrite<3,225>();
                break;
              }
            case 201://YZX
              {
                sortAndWrite<3,201>();
                break;
              }
            case 210://ZXY
              {
                sortAndWrite<3,210>();
                break;
              }
            case 198://ZYX
              {
                sortAndWrite<3,198>();
                break;
              }
             default:
              MEDEXCEPTION("ASCII_FIELD_DRIVER : Invalid priority definition");
            }
          break;
          }
      default:
        MEDEXCEPTION("ASCII_FIELD_DRIVER : Invalid space dimension must be 2 or 3");
        }
  }

  template <class T>
  void ASCII_FIELD_DRIVER<T>::buildIntroduction() const
  {

    int i;
    _file  << setiosflags(ios::scientific);
    _file << "#TITLE: table " << _ptrField->getName() << " TIME: " << _ptrField->getTime() << " IT: " << _ptrField->getIterationNumber() << endl;
    _file << "#COLUMN_TITLES: ";
    for(i=0;i<_spaceDimension;i++)
      _file << char('X'+i) << " | ";
    const std::string *compoNames=_ptrField->getComponentsNames();
    for(i=0;i<_nbComponents;i++)
      {
        if(!compoNames)
          _file << compoNames[i];
        else
          _file << "None";
        if(i<_nbComponents-1)
          _file << " | ";
      }
    _file << endl;
    _file << "#COLUMN_UNITS: ";
    compoNames=_mesh->getCoordinateptr()->getCoordinatesUnits();
    for(i=0;i<_spaceDimension;i++)
      {
        if(!compoNames)
          _file << compoNames[i];
        else
          _file << "None";
        _file << " | ";
      }
    const UNIT *compoUnits=_ptrField->getComponentsUnits();
    for(i=0;i<_nbComponents;i++)
      {
        if(!compoUnits)
          _file << compoUnits[i].getName();
        else
          _file << "None";
        if(i<_nbComponents-1)
          _file << " | ";
      }
    _file << endl;
  }
}

#include "MEDMEM_Field.hxx"

namespace MEDMEM
{
  template <class T>
  template<int SPACEDIMENSION, unsigned int SORTSTRATEGY>
  void ASCII_FIELD_DRIVER<T>::sortAndWrite() const
  {
    typedef typename MEDMEM_ArrayInterface<double,NoInterlace,NoGauss>::Array    ArrayDoubleNo;
    typedef typename MEDMEM_ArrayInterface<double,FullInterlace,NoGauss>::Array  ArrayDoubleFull;
    typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array         ArrayNo;
    typedef typename MEDMEM_ArrayInterface<T,NoInterlaceByType,NoGauss>::Array   ArrayNoByType;
    typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array       ArrayFull;

    int i,j;
    int numberOfValues=_ptrField->getNumberOfValues();
    std::list< SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY > > li;
    const double * coord;
    FIELD<double,FullInterlace> * barycenterField=0;
    ArrayDoubleNo * baryArrayTmp = NULL;
    double * xyz[SPACEDIMENSION];
    bool deallocateXyz=false;

    if(_support->getEntity()==MED_EN::MED_NODE) {
      if (_support->isOnAllElements()) {

        coord=_mesh->getCoordinates(MED_EN::MED_NO_INTERLACE);
        for(i=0; i<SPACEDIMENSION; i++)
          xyz[i]=(double *)coord+i*numberOfValues;

      } else {

        coord = _mesh->getCoordinates(MED_EN::MED_FULL_INTERLACE);
        const int * nodesNumber=_support->getNumber(MED_EN::MED_ALL_ELEMENTS);
        for(i=0; i<SPACEDIMENSION; i++)
          xyz[i]=new double[numberOfValues];
            deallocateXyz=true;
            for(i=0;i<numberOfValues;i++) {
              for(j=0;j<SPACEDIMENSION;j++)
                xyz[j][i]=coord[(nodesNumber[i]-1)*SPACEDIMENSION+j];
            }
      }
    } else {

      barycenterField = _mesh->getBarycenter(_support);
      baryArrayTmp = ArrayConvert
        ( *( static_cast<ArrayDoubleFull*>(barycenterField->getArray()) ) );
      coord = baryArrayTmp->getPtr();
      for(i=0; i<SPACEDIMENSION; i++)
        xyz[i]=(double *)(coord+i*numberOfValues);
    }

    const T * valsToSet;
    ArrayFull * tmpArray = NULL;
    if ( _ptrField->getInterlacingType() == MED_EN::MED_FULL_INTERLACE )
      valsToSet= _ptrField->getValue();
    else if ( _ptrField->getInterlacingType() == MED_EN::MED_NO_INTERLACE_BY_TYPE ) {
      tmpArray = ArrayConvert
        ( *( static_cast<ArrayNoByType*>(_ptrField->getArray()) ) );
      valsToSet= tmpArray->getPtr();
    }
    else {
      tmpArray = ArrayConvert
        ( *( static_cast<ArrayNo*>(_ptrField->getArray()) ) );
      valsToSet= tmpArray->getPtr();
    }
    double temp[SPACEDIMENSION];
    for(i=0;i<numberOfValues;i++) {
      for(j=0;j<SPACEDIMENSION;j++)
        temp[j]=xyz[j][i];
      li.push_back(SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>(temp,valsToSet+i*_nbComponents,_nbComponents));
    }

    if (barycenterField) barycenterField->removeReference();
    if (baryArrayTmp)    delete baryArrayTmp;
    if (tmpArray)        delete tmpArray;

    if(deallocateXyz)
      for(j=0;j<SPACEDIMENSION;j++)
        delete [] xyz[j];

    li.sort();
    _file << setprecision(PRECISION_IN_ASCII_FILE);
    if(_direc==MED_EN::ASCENDING) {
      typename std::list< SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY > >::iterator iter;
      for(iter=li.begin();iter!=li.end();iter++)
        (*iter).writeLine(_file);
      _file << endl;
    } else if (_direc==MED_EN::DESCENDING) {

      typename std::list< SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY > >::reverse_iterator iter;
      for(iter=li.rbegin();iter!=li.rend();iter++)
        (*iter).writeLine(_file);
      _file << endl;
    } else
      MEDEXCEPTION("ASCII_FIELD_DRIVER : Invalid sort direction");
  }

  //{
    //_nbComponentsForCpyInfo=_nbComponents;
    //_ptrField->fillFromAnalytic <TEST<T>::copyInfo3> ();
    //_ptrField->fillFromAnalytic<  ASCII_FIELD_DRIVER<T>::copyInfo<SPACEDIMENSION,SORTSTRATEGY> > ();
    //li.sort();
    //typename std::list< SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY > >::iterator iter;
    //_file << setiosflags(ios::scientific) << setprecision(PRECISION_IN_ASCII_FILE);
    //for(iter=li.begin();iter!=li.end();iter++)
    //  {
    //(*iter).writeLine(_file);
    // }

//   template <class T>
//   template<int SPACEDIMENSION, unsigned int SORTSTRATEGY>//, std::list< SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY> > lis>
//   void ASCII_FIELD_DRIVER<T>::copyInfo(const double *a,T *b)
//   {
//     //lis.push_back(SDForSorting<T,SPACEDIMENSION,SORTSTRATEGY>(a,b,_nbComponentsForCpyInfo));
//   }

//   template <class T>
//   int ASCII_FIELD_DRIVER<T>::_nbComponentsForCpyInfo=0;
}

#endif
