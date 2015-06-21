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

#ifndef MEDMEM_INTERLACING_TRAITS_HXX
#define MEDMEM_INTERLACING_TRAITS_HXX

#include "MEDMEM_Tags.hxx"

namespace MEDMEM {

struct __ERROR_INTERLACING_GAUSS_TRAITS_POLICY_INSTANCIATED_WITH_INCORRECT_TYPE {};

template <  class INTERLACE_TAG, class GAUSS_TAG>
struct MEDMEM_InterlacingTraits
{
  typedef  __ERROR_INTERLACING_GAUSS_TRAITS_POLICY_INSTANCIATED_WITH_INCORRECT_TYPE Type;
};

template <>
struct MEDMEM_InterlacingTraits< FullInterlace, Gauss >
{
  typedef FullInterlaceGaussPolicy Type;
};

template <>
struct MEDMEM_InterlacingTraits< FullInterlace, NoGauss >
{
  typedef FullInterlaceNoGaussPolicy Type;
};

template <>
struct MEDMEM_InterlacingTraits< NoInterlace, Gauss >
{
  typedef NoInterlaceGaussPolicy Type;
};

template <>
struct MEDMEM_InterlacingTraits< NoInterlace, NoGauss >
{
  typedef NoInterlaceNoGaussPolicy Type;
};

template <>
struct MEDMEM_InterlacingTraits< NoInterlaceByType, Gauss >
{
  typedef NoInterlaceByTypeGaussPolicy Type;
};

template <>
struct MEDMEM_InterlacingTraits< NoInterlaceByType, NoGauss >
{
  typedef NoInterlaceByTypeNoGaussPolicy Type;
};

} // END NAMESPACE

#endif
