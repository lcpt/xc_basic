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

#ifndef DEFINE_HXX
#define DEFINE_HXX

#include <cstdio>
#include <iostream>
#include <string>
#include <cassert>

// UTILE AUX DEUX NAMESPACES
#include <hdf5.h>
#include "med.h"
#include "med_err.h"
//#include "med_xc.h"
#include "med_proto.h"
#include "med_definitions.h"

//using namespace med_xc;


namespace MED_EN {
  extern "C" { // on est la

// #define MED_NULL       (void *) NULL
// #define MED_MAX_PARA     20

// #define MED_TAILLE_DESC 200
// #define MED_TAILLE_IDENT  8
// #define MED_TAILLE_NOM   32
// #define MED_TAILLE_LNOM  80
// #define MED_TAILLE_PNOM   8

#ifdef MED_NONE
#undef MED_NONE
#endif
#ifdef MED_POINT1
#undef MED_POINT1
#endif
#ifdef MED_SEG2
#undef MED_SEG2
#endif
#ifdef MED_SEG3
#undef MED_SEG3
#endif
#ifdef MED_TRIA3
#undef MED_TRIA3
#endif
#ifdef MED_QUAD4
#undef MED_QUAD4
#endif
#ifdef MED_TRIA6
#undef MED_TRIA6
#endif
#ifdef MED_QUAD8
#undef MED_QUAD8
#endif
#ifdef MED_TETRA4
#undef MED_TETRA4
#endif
#ifdef MED_PYRA5
#undef MED_PYRA5
#endif
#ifdef MED_PENTA6
#undef MED_PENTA6
#endif
#ifdef MED_HEXA8
#undef MED_HEXA8
#endif
#ifdef MED_TETRA10
#undef MED_TETRA10
#endif
#ifdef MED_PYRA13
#undef MED_PYRA13
#endif
#ifdef MED_PENTA15
#undef MED_PENTA15
#endif
#ifdef MED_HEXA20
#undef MED_HEXA20
#endif
#ifdef MED_POLYGON
#undef MED_POLYGON
#endif
#ifdef MED_POLYHEDRA
#undef MED_POLYHEDRA
#endif
#ifdef MED_ALL_ELEMENTS
#undef MED_ALL_ELEMENTS
#endif

    typedef long medGeometryElement;
    //Modificado LCPT
    const medGeometryElement MED_NONE= med_defined_MED_NONE;
    const medGeometryElement MED_POINT1= med_defined_MED_POINT1;
    const medGeometryElement MED_SEG2= med_defined_MED_SEG2;
    const medGeometryElement MED_SEG3= med_defined_MED_SEG3;
    const medGeometryElement MED_TRIA3= med_defined_MED_TRIA3;
    const medGeometryElement MED_QUAD4= med_defined_MED_QUAD4;
    const medGeometryElement MED_TRIA6= med_defined_MED_TRIA6;
    const medGeometryElement MED_QUAD8= med_defined_MED_QUAD8;
    const medGeometryElement MED_TETRA4= med_defined_MED_TETRA4;
    const medGeometryElement MED_PYRA5= med_defined_MED_PYRA5;
    const medGeometryElement MED_PENTA6= med_defined_MED_PENTA6;
    const medGeometryElement MED_HEXA8= med_defined_MED_HEXA8;
    const medGeometryElement MED_TETRA10= med_defined_MED_TETRA10;
    const medGeometryElement MED_PYRA13= med_defined_MED_PYRA13;
    const medGeometryElement MED_PENTA15= med_defined_MED_PENTA15;
    const medGeometryElement MED_HEXA20= med_defined_MED_HEXA20;
    const medGeometryElement MED_POLYGON= med_defined_MED_POLYGON;
    const medGeometryElement MED_POLYHEDRA= med_defined_MED_POLYHEDRA;
    //Fin modificacion LCPT
    const medGeometryElement MED_ALL_ELEMENTS= 999;

    typedef long medEntityMesh;
    const medEntityMesh MED_CELL= 0;
    const medEntityMesh MED_FACE= 1;
    const medEntityMesh MED_EDGE= 2;
    const medEntityMesh MED_NODE= 3;
    const medEntityMesh MED_ALL_ENTITIES= 4;

    typedef long medModeSwitch;
    const medModeSwitch MED_FULL_INTERLACE= 0;
    const medModeSwitch MED_NO_INTERLACE= 1;
    const medModeSwitch MED_NO_INTERLACE_BY_TYPE= 2;
    const medModeSwitch MED_UNDEFINED_INTERLACE= 3;
    
    typedef long medConnectivity;
    const medConnectivity MED_NODAL= 0;
    const medConnectivity MED_DESCENDING= 1;

    typedef enum {MED_CARTESIAN, MED_POLAR, MED_BODY_FITTED} med_grid_type;

    //typedef enum {MED_LECT,MED_ECRI,MED_REMP,MED_CREA} med_mode_acces;
    typedef enum {RDONLY,WRONLY,RDWR} med_mode_acces;

    typedef enum {ASCENDING=7,DESCENDING=77} med_sort_direc;

    typedef enum {MED_REEL64=6, MED_INT32=24,MED_INT64=26, MED_UNDEFINED_TYPE=0} med_type_champ;

// #define MED_NBR_GEOMETRIE_MAILLE 15
// #define MED_NBR_GEOMETRIE_FACE 4
// #define MED_NBR_GEOMETRIE_ARETE 2

typedef enum {MED_CART, MED_CYL, MED_SPHER} med_repere;

typedef enum {MED_FAUX, MED_VRAI} med_booleen ;

typedef enum {MED_GROUPE, MED_ATTR, MED_FAMILLE} med_dim_famille;

typedef enum {MED_COMP, MED_DTYPE} med_dim_champ;

typedef enum {MED_HDF_VERSION, MED_VERSION, MED_FICH_DES} med_fich_info;

// #define MED_NOPG   1                   /* -> pas de point de Gauss                    */
// #define MED_NOPFL  ""                  /* -> pas de profils utilisateur               */
// #define MED_NOPFLi "                                "  /* Variable Interne                      */
// #define MED_NOPF   0                   /* -> pas de profils pour _MEDdataseNnumEcrire */
// #define MED_NOPDT -1                   /* rem: pas de pas de temps negatifs           */
// #define MED_NONOR -1                   /* rem: pas de n°ordre negatif                 */
// #define MED_DIM1   1                   /* PAS */
//   //#define MED_ALL    0 !!!!! NB: WARNING MED_ALL deja utilise dans l'enum medGeometryElement !!!!!!!!
// #define MED_ALL    0

#if defined(SUN4SOL2) || defined(PCLINUX) || defined(PCLINUX64_32) || defined(OSF1_32) || defined(IRIX64_32) || defined(RS6000)
/* interface C/FORTRAN */
/* this true only with g77 and gcc : we must change it to use directly NOMF_... and INT32 or INT64 - it will be more simple to understand and to use ! */
// #define NOMF_POST_UNDERSCORE

/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
    //typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef int            med_int;
typedef double         med_float;
#endif

#if defined(HP9000)
/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
    //typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef int            med_int;
typedef double         med_float;
#endif

#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64) 
// #define NOMF_POST_UNDERSCORE

/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
    //typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef long           med_int;
typedef double         med_float;
#endif


#if defined(PPRO_NT)
/* correspondance des types avec HDF 5 */
typedef hsize_t        med_size;
typedef hssize_t       med_ssize;
    //typedef hid_t          med_idt;
typedef herr_t         med_err;

/* types elementaires */
typedef int            med_int;
typedef double         med_float;
#endif


// #if defined(NOMF_PRE_UNDERSCORE) && defined(NOMF_POST_UNDERSCORE)
// #   define NOMF(x)     _##x##_
// #endif
// #if defined(NOMF_PRE_UNDERSCORE) && !defined(NOMF_POST_UNDERSCORE)
// #   define NOMF(x)     _##x
// #endif
// #if !defined(NOMF_PRE_UNDERSCORE) && defined(NOMF_POST_UNDERSCORE)
// #   define NOMF(x)     x##_
// #endif
// #if !defined(NOMF_PRE_UNDERSCORE) && !defined(NOMF_POST_UNDERSCORE)
// #   define NOMF(x)     x
// #endif

  //#include "med_proto.h"

  }

  // valeurs des algos de connexites
  const int FRACT_ALGO= 10;
  const int MAX_LOOP  = 200;
// fin

// Valeurs de IO_MED
//#define READ 0
//#define WRITE 1


  const int MED_CLOSED  = 0;
  const int MED_OPENED  = 1;
  const int MED_INVALID = -1;
  const int MED_ERROR   = -1;
  const int MED_VALID   = 0;
//#define MED_NULL     NULL

//#define MED_RDONLY RDONLY
////   rem: MED_WRONLY=MED_ECR n'empêche malheureusement pas de lire le fichier
////   mais permet de conserver l'existant. Attention à la création d'objet
////   ne prenant pas de paramètre de type mode d'accès il faut tester si il
////   est déjà présent dans le fichier. Si  MED_WRONLY=MED_REMP le fichier est
////   réinitialisé. Cf une évolution de MED.
//#define MED_WRONLY WRONLY
//#define MED_RDWR   RDWR
//#define MED_CREATE MED_CREA
// Fin

  const int longueur_string= 100;
  const double PRECISION= 0.0000001;
  const int MED_UNDEFINED= -1;

// type des connectivites
//#define CONN_NOD 0
//#define CONN_DESC_PART 1
//#define CONN_DESC_TOT 2

// valeurs de type_interpolation
  const int P1                = 0;
  const int pseudo_P2         = 1;
  const int trilineaire       = 2;
  const int P2_adapte         = 3;
  const int VF_conservatif    = 4;
  const int VF_non_conservatif= 5;
  // valeurs de type_mapping
  const int connexite  = 0;
  const int conn_octree= 1;
  const int brutal     = 2;
// valeurs des type_support
  const int support_noeud= 0;
  const int support_maille= 1;
// retour des fonctions
  //const int OK= 0;
  //const int KO= 1;

//maximum number of groups handled by the Family Group converter
  const int MAX_NB_GROUP= 100000;
  /*
    enum definition for the used Med File version V2.1 or V2.2
    because of incompatibility between them.
  */

  typedef enum {V21= 26, V22= 75} medFileVersion;

// fin
}
// End Of NameSpace MED_EN

#endif /* DEFINE_HXX */
