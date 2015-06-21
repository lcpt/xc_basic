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

#ifndef FIELD_CONVERT_HXX
#define FIELD_CONVERT_HXX

//#include "MEDMEM_FieldForward.hxx"
#include "MEDMEM_Field.hxx" // issue 0021050: compilation with clang
#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_ArrayConvert.hxx"

namespace MEDMEM {
class FIELD_;

template <class T> FIELD<T,FullInterlace> *
FieldConvert(const FIELD<T,NoInterlace> & field )
{
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFullNo;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,Gauss>::Array ArrayFullGa;

  FIELD<T,FullInterlace> * myField = new FIELD<T,FullInterlace>();
  FIELD_ * myField_ = myField;
  FIELD_ * field_ = &(const_cast< FIELD<T,NoInterlace> &> (field));
  *myField_ = *field_;                        // Opérateur d'affectation de FIELD_ OK
  // *((FIELD_ *) myField) = (FIELD_ ) field; //Contructeur par recopie de FIELD_ Pourquoi?

  if  ( field.getGaussPresence() ) {
    ArrayFullGa * myArray = ArrayConvert( *(field.getArrayGauss()) );
    myField->setArray(myArray);
    return myField;
  } else {
    ArrayFullNo * myArray = ArrayConvert( *(field.getArrayNoGauss()) );
    myField->setArray(myArray);
    return myField;
  }
}

template <class T> FIELD<T,NoInterlace> *
FieldConvert(const FIELD<T,FullInterlace> & field )
{
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,NoGauss>::Array ArrayNoNo;
  typedef typename MEDMEM_ArrayInterface<T,NoInterlace,Gauss>::Array ArrayNoGa;


  FIELD<T,NoInterlace> * myField = new FIELD<T,NoInterlace>();
  FIELD_ * myField_ = myField;
  FIELD_ * field_ = &(const_cast< FIELD<T,FullInterlace> &> (field));
  *myField_ = *field_;                        // Opérateur d'affectation de FIELD_ OK
  //   *((FIELD_*) myField) = (FIELD_) field; //Contructeur par recopie de FIELD_ Pourquoi?

  if  ( field.getGaussPresence() ) {
    ArrayNoGa * myArray = ArrayConvert( *(field.getArrayGauss()) );
    myField->setArray(myArray);
    return myField;
  } else {
    ArrayNoNo * myArray = ArrayConvert( *(field.getArrayNoGauss()) );
    myField->setArray(myArray);
    return myField;
  }

}

template <class T> FIELD<T,FullInterlace> *
FieldConvert(const FIELD<T,NoInterlaceByType> & field )
{
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,NoGauss>::Array ArrayFullNo;
  typedef typename MEDMEM_ArrayInterface<T,FullInterlace,Gauss>::Array ArrayFullGa;

  FIELD<T,FullInterlace> * myField = new FIELD<T,FullInterlace>();
  FIELD_ * myField_ = myField;
  FIELD_ * field_ = &(const_cast< FIELD<T,NoInterlaceByType> &> (field));
  *myField_ = *field_;                        // Opérateur d'affectation de FIELD_ OK
  // *((FIELD_ *) myField) = (FIELD_ ) field; //Contructeur par recopie de FIELD_ Pourquoi?

  if  ( field.getGaussPresence() ) {
    ArrayFullGa * myArray = ArrayConvert( *(field.getArrayGauss()) );
    myField->setArray(myArray);
    return myField;
  } else {
    ArrayFullNo * myArray = ArrayConvert( *(field.getArrayNoGauss()) );
    myField->setArray(myArray);
    return myField;
  }
}

}
#endif
