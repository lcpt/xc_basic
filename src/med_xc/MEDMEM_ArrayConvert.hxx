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

#ifndef MEDMEM_ARRAY_CONVERT_HXX
#define MEDMEM_ARRAY_CONVERT_HXX

namespace MEDMEM {

template  <class T, class CHECKING_POLICY >
MEDMEM_Array<T, FullInterlaceGaussPolicy, CHECKING_POLICY> *
ArrayConvert(
             const MEDMEM_Array< T, NoInterlaceGaussPolicy, CHECKING_POLICY > &array, T* values=0
             )
{
  MEDMEM_Array<T,FullInterlaceGaussPolicy,CHECKING_POLICY> * myArray;
  if(values)
    myArray = new MEDMEM_Array<T,FullInterlaceGaussPolicy,CHECKING_POLICY> 
      (values,
       array.getDim(),
       array.getNbElem(),
       array.getNbGeoType(),
       array.getNbElemGeoC(),
       array.getNbGaussGeo(),
       true,false
       );
  else
     myArray= new MEDMEM_Array<T,FullInterlaceGaussPolicy,CHECKING_POLICY> 
      (array.getDim(),
       array.getNbElem(),
       array.getNbGeoType(),
       array.getNbElemGeoC(),
       array.getNbGaussGeo()
       );
  for (int i=1; i <= array.getNbElem() ; i++ )
    for (int k=1; k<= array.getNbGauss(i); k++ )
      for (int j=1; j<= array.getDim(); j++ )
        myArray->setIJK(i,j,k,array.getIJK(i,j,k));

  return myArray;

}

template  <class T, class CHECKING_POLICY>
MEDMEM_Array<T, NoInterlaceGaussPolicy, CHECKING_POLICY> *
ArrayConvert(
             const MEDMEM_Array< T, FullInterlaceGaussPolicy, CHECKING_POLICY > &array, T* values=0
             )
{
  MEDMEM_Array<T,NoInterlaceGaussPolicy,CHECKING_POLICY> * myArray;
  if(values)
    myArray = new MEDMEM_Array<T, NoInterlaceGaussPolicy,CHECKING_POLICY> (
                                                                           values,
                                                                           array.getDim(),
                                                                           array.getNbElem(),
                                                                           array.getNbGeoType(),
                                                                           array.getNbElemGeoC(),
                                                                           array.getNbGaussGeo(),
                                                                           true,false
                                                                           );
  else
    myArray = new MEDMEM_Array<T, NoInterlaceGaussPolicy,CHECKING_POLICY> (
                                                                           array.getDim(),
                                                                           array.getNbElem(),
                                                                           array.getNbGeoType(),
                                                                           array.getNbElemGeoC(),
                                                                           array.getNbGaussGeo()
                                                                           );
  for (int i=1; i <= array.getNbElem() ; i++ )
    for (int k=1; k<= array.getNbGauss(i); k++ )
      for (int j=1; j<= array.getDim(); j++ )
        myArray->setIJK(i,j,k,array.getIJK(i,j,k));

  return myArray;

}

template  <class T, class CHECKING_POLICY>
MEDMEM_Array<T, FullInterlaceGaussPolicy, CHECKING_POLICY> *
ArrayConvert(
             const MEDMEM_Array< T, NoInterlaceByTypeGaussPolicy, CHECKING_POLICY > &array, T* values=0
             )
{
  MEDMEM_Array<T,FullInterlaceGaussPolicy,CHECKING_POLICY> * myArray;
  if(values)
    myArray = new MEDMEM_Array<T, FullInterlaceGaussPolicy,CHECKING_POLICY> (
                                                                           values,
                                                                           array.getDim(),
                                                                           array.getNbElem(),
                                                                           array.getNbGeoType(),
                                                                           array.getNbElemGeoC(),
                                                                           array.getNbGaussGeo(),
                                                                           true,false
                                                                           );
  else
    myArray = new MEDMEM_Array<T, FullInterlaceGaussPolicy,CHECKING_POLICY> (
                                                                           array.getDim(),
                                                                           array.getNbElem(),
                                                                           array.getNbGeoType(),
                                                                           array.getNbElemGeoC(),
                                                                           array.getNbGaussGeo()
                                                                           );
  for (int i=1; i <= array.getNbElem() ; i++ )
    for (int k=1; k<= array.getNbGauss(i); k++ )
      for (int j=1; j<= array.getDim(); j++ )
        myArray->setIJK(i,j,k,array.getIJK(i,j,k));
  
  return myArray;

}

template  <class T, class CHECKING_POLICY>
MEDMEM_Array<T, NoInterlaceGaussPolicy, CHECKING_POLICY> *
ArrayConvert2No(
             const MEDMEM_Array< T, NoInterlaceByTypeGaussPolicy, CHECKING_POLICY > &array, T* values=0
             )
{
  MEDMEM_Array<T,NoInterlaceGaussPolicy,CHECKING_POLICY> * myArray;
  if(values)
    myArray = new MEDMEM_Array<T, NoInterlaceGaussPolicy,CHECKING_POLICY> (
                                                                           values,
                                                                           array.getDim(),
                                                                           array.getNbElem(),
                                                                           array.getNbGeoType(),
                                                                           array.getNbElemGeoC(),
                                                                           array.getNbGaussGeo(),
                                                                           true,false
                                                                           );
  else
    myArray = new MEDMEM_Array<T, NoInterlaceGaussPolicy,CHECKING_POLICY> (
                                                                           array.getDim(),
                                                                           array.getNbElem(),
                                                                           array.getNbGeoType(),
                                                                           array.getNbElemGeoC(),
                                                                           array.getNbGaussGeo()
                                                                           );
  for (int i=1; i <= array.getNbElem() ; i++ )
    for (int k=1; k<= array.getNbGauss(i); k++ )
      for (int j=1; j<= array.getDim(); j++ )
        myArray->setIJK(i,j,k,array.getIJK(i,j,k));
  
  return myArray;

}

template  <class T, class CHECKING_POLICY>
MEDMEM_Array<T, NoInterlaceNoGaussPolicy, CHECKING_POLICY> *
ArrayConvert(
             const MEDMEM_Array< T, FullInterlaceNoGaussPolicy, CHECKING_POLICY > &array, T* values=0
             )
{
  MEDMEM_Array<T,NoInterlaceNoGaussPolicy,CHECKING_POLICY> * myArray;
  if(values)
    myArray=new MEDMEM_Array<T, NoInterlaceNoGaussPolicy, CHECKING_POLICY> ( values,
                                                                             array.getDim(),
                                                                             array.getNbElem(),
                                                                             true,false
                                                                             );
  else
    myArray = new MEDMEM_Array<T, NoInterlaceNoGaussPolicy, CHECKING_POLICY> (
                                                                              array.getDim(),
                                                                              array.getNbElem()
                                                                              );
  for (int i=1; i <= array.getNbElem() ; i++ )
    for (int j=1; j<= array.getDim(); j++ )
      myArray->setIJ(i,j,array.getIJ(i,j));

  return myArray;

}

template  <class T, class CHECKING_POLICY>
MEDMEM_Array<T, FullInterlaceNoGaussPolicy, CHECKING_POLICY> *
ArrayConvert(
             const MEDMEM_Array< T, NoInterlaceByTypeNoGaussPolicy, CHECKING_POLICY > &array, T* values=0
             )
{
  MEDMEM_Array<T,FullInterlaceNoGaussPolicy,CHECKING_POLICY> * myArray;
  if(values)
    myArray=new MEDMEM_Array<T, FullInterlaceNoGaussPolicy, CHECKING_POLICY> ( values,
                                                                             array.getDim(),
                                                                             array.getNbElem(),
                                                                             true,false
                                                                             );
  else
    myArray = new MEDMEM_Array<T, FullInterlaceNoGaussPolicy, CHECKING_POLICY> (
                                                                              array.getDim(),
                                                                              array.getNbElem()
                                                                              );
  for (int i=1; i <= array.getNbElem() ; i++ )
    for (int j=1; j<= array.getDim(); j++ )
      myArray->setIJ(i,j,array.getIJ(i,j));
  
  return myArray;

}

template  <class T, class CHECKING_POLICY>
MEDMEM_Array<T, NoInterlaceNoGaussPolicy, CHECKING_POLICY> *
ArrayConvert2No(
             const MEDMEM_Array< T, NoInterlaceByTypeNoGaussPolicy, CHECKING_POLICY > &array, T* values=0
             )
{
  MEDMEM_Array<T,NoInterlaceNoGaussPolicy,CHECKING_POLICY> * myArray;
  if(values)
    myArray=new MEDMEM_Array<T, NoInterlaceNoGaussPolicy, CHECKING_POLICY> ( values,
                                                                             array.getDim(),
                                                                             array.getNbElem(),
                                                                             true,false
                                                                             );
  else
    myArray = new MEDMEM_Array<T, NoInterlaceNoGaussPolicy, CHECKING_POLICY> (
                                                                              array.getDim(),
                                                                              array.getNbElem()
                                                                              );
  for (int i=1; i <= array.getNbElem() ; i++ )
    for (int j=1; j<= array.getDim(); j++ )
      myArray->setIJ(i,j,array.getIJ(i,j));
  
  return myArray;

}

template  <class T, class CHECKING_POLICY>
MEDMEM_Array<T, FullInterlaceNoGaussPolicy, CHECKING_POLICY> *
ArrayConvert(
             const MEDMEM_Array< T, NoInterlaceNoGaussPolicy, CHECKING_POLICY > &array, T* values=0
             )
{
  MEDMEM_Array<T,FullInterlaceNoGaussPolicy,CHECKING_POLICY> * myArray;
  if(values)
    myArray= new MEDMEM_Array<T, FullInterlaceNoGaussPolicy, CHECKING_POLICY> (values,
                                                                               array.getDim(),
                                                                               array.getNbElem(),
                                                                               true,false
                                                                               );
  else
    myArray= new MEDMEM_Array<T, FullInterlaceNoGaussPolicy, CHECKING_POLICY> (
                                                                               array.getDim(),
                                                                               array.getNbElem()
                                                                               );
  for (int i=1; i <= array.getNbElem() ; i++ )
    for (int j=1; j<= array.getDim(); j++ )
      myArray->setIJ(i,j,array.getIJ(i,j));

  return myArray;

}



// template  <class T,class INTERLACING_POLICY,class INTERLACING_POLICY_OTHER,
//         class CHECKING_POLICY>
// void ArrayConvert(
//                const MEDMEM_Array< T, INTERLACING_POLICY, CHECKING_POLICY > & arrayin,
//                MEDMEM_Array< T, INTERLACING_POLICY_OTHER, CHECKING_POLICY> * & arrayout
//                )
// {

//   MEDMEM_Array<T,INTERLACING_POLICY_OTHER,CHECKING_POLICY> *  myArray;
//   myArray= new MEDMEM_Array< T, INTERLACING_POLICY_OTHER, CHECKING_POLICY > (
//                    arrayin.getDim(),
//                    arrayin.getNbElem(),
//                    arrayin.getNbGeoType(),
//                    arrayin.getNbElemGeoC(),
//                    arrayin.getNbGaussGeo()
//                    );

//   for (int i=1; i <= arrayin.getNbElem() ; i++ )
//     for (int k=1; k<= arrayin.getNbGauss(i); k++ )
//       for (int j=1; j<= arrayin.getDim(); j++ )
//      myArray->setIJK(i,j,k,arrayin.getIJK(i,j,k));

//   arrayout=myArray;

// };





// template  <class ARRAY_ELEMENT_TYPE,
//         class GAUSS_TAG,class CHECKING_POLICY=IndexCheckPolicy>
//            typename MEDMEM_ArrayInterface<ARRAY_ELEMENT_TYPE,
//                    FullInterlace,
//                    GAUSS_TAG,
//                    CHECKING_POLICY>::Array &
// convertBis(const typename MEDMEM_ArrayInterface<
//         ARRAY_ELEMENT_TYPE,
//         NoInterlace,
//         GAUSS_TAG,
//         CHECKING_POLICY>::Array & array ) {
//   std::cout << "-------- Convert 3" << std::endl;
// };

} //END NAMESPACE
#endif
