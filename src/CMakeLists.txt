cmake_minimum_required(VERSION 2.4)
#cygwin
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
set(CMAKE_VERSION_STRING ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION})
IF (${CMAKE_VERSION_STRING} GREATER 2.8.4)
  cmake_policy(SET CMP0017 NEW) 
  MESSAGE ("CMake version is greater than 2.8.4")
ELSE (${CMAKE_VERSION_STRING} GREATER 2.8.4)
  MESSAGE ("CMake version is not greater than 2.8.4")
ENDIF (${CMAKE_VERSION_STRING} GREATER 2.8.4)

# Nombre del proyecto.
PROJECT(xc_basic)
SET(xc_basic_DIR ${CMAKE_SOURCE_DIR}/..)
message(STATUS "project dir: ${xc_basic_DIR}")
string(REGEX REPLACE "/xc_basic/src.*" "" xc_basic_INC ${CMAKE_SOURCE_DIR})
message(STATUS "include dir: "${xc_basic_INC})
SET(xc_basic_LIB ${xc_basic_INC}/xc_basic/lib)
message(STATUS "lib dir: ${xc_basic_LIB}")

SET(build_setup_dir $ENV{HOME}/.xc_build)
SET(build_setup_file ${build_setup_dir}/basica_dirs.cmake)
file(MAKE_DIRECTORY ${build_setup_dir})
file(WRITE ${build_setup_file} "SET(basica_src_dir ${CMAKE_SOURCE_DIR})\n")
file(APPEND ${build_setup_file} "SET(basica_inc_dir ${xc_basic_INC})\n")
file(APPEND ${build_setup_file} "SET(basica_lib_dir ${xc_basic_LIB})\n")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${xc_basic_DIR}/CMake)
SET(LIBRARY_OUTPUT_PATH ${xc_basic_LIB})

INCLUDE_DIRECTORIES(${xc_basic_INC})

ADD_DEFINITIONS(-Wall -O3 -march=native -pedantic -DH5_USE_16_API -fPIC)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
#SET(CMAKE_VERBOSE_MAKEFILE ON)

enable_language(Fortran)

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(VTK REQUIRED)
find_package(GTK2 REQUIRED gtk)
set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
#set(Boost_DETAILED_FAILURE_MSG ON)
find_package(Boost 1.38 COMPONENTS regex math)
find_package(SQLITE3 REQUIRED)
find_package(MED REQUIRED)
find_package(ExecInfo)
find_package(HDF5 REQUIRED)

if(GTK2_FOUND)
  include_directories(${GTK2_INCLUDE_DIRS})
endif()
IF(USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
  message(STATUS "vtk file:  ${USE_VTK_FILE}")
  SET(VTK_FOUND 1)
ENDIF(USE_VTK_FILE)
if(MED_FOUND)
  include_directories(${MED_INCLUDE_DIRS})
endif()

INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIRS})
include_directories(${HDF5_HEADER_INCLUDE_DIR})
message(STATUS "hdf5 found:  ${HDF5_FOUND}")
# message(STATUS "********* hdf5 version: " ${HDF5_VERSION})
# message(STATUS "********* hdf5 includes: " ${HDF5_INCLUDE_DIRS})
# message(STATUS "********* hdf5 include: " ${HDF5_INCLUDE_FILENAME})
# message(STATUS "********* hdf5 definitions: " ${HDF_DEFINITIONS})

SET(tiempo tiempo/agno tiempo/mes tiempo/diasemana tiempo/hora tiempo/fecha tiempo/fechahora tiempo/timer)

SET(texto texto/cadena_carac texto/LabelContainer texto/en_letra texto/StrTok texto/tab_cod texto/latex texto/StringFormatter texto/FixedWidthReader)

SET(streams streams/imanip streams/prep)

SET(stl stl/persist)

SET(util util/cmd_args util/memoria util/inercia util/print_trace)

SET(sqlitepp sqlitepp/Database  sqlitepp/Query  sqlitepp/StderrLog  sqlitepp/SysLog)

SET(matrices matrices/matrizM matrices/vcolM matrices/RangoIndice matrices/RangoMatriz matrices/ConstRefCaja matrices/matrizExpr matrices/matrizMExpr matrices/matrizPolmio matrices/m_double matrices/m_int matrices/m_sizet matrices/m_string matrices/giros matrices/op_tensor matrices/solvers/solver matrices/MatrizIndices matrices/TritrizIndices)

SET(skypack skypack/skypri.f skypack/skypds.f skypack/skyscf.f skypack/skyssb.f skypack/mtrxys.f skypack/skyssf.f skypack/skypsb.f skypack/skypdc.f skypack/skypf1.f skypack/skysdc.f skypack/skypsp.f skypack/skypcf.f skypack/skymef.f skypack/skyssd.f skypack/skyss.f skypack/skysf1.f skypack/skypf2.f skypack/mtrxy.f skypack/mtrxyd.f skypack/skysf2.f skypack/skypci.f)

SET(itpack itpack/dsrc2c.f itpack/jcg.f itpack/jsi.f itpack/rscg.f itpack/rssi.f itpack/sor.f itpack/ssorcg.f itpack/ssorsi.f)

SET(umfpack umfpack/umd2fb.f umfpack/umd2co.f umfpack/umd2fa.f umfpack/umd2f0.f umfpack/umd2f1.f umfpack/umd2f2.f umfpack/umd2fg.f umfpack/umd2in.f umfpack/umd2of.f umfpack/umd2s2.f umfpack/umd2sl.f umfpack/umd2so.f umfpack/umd2su.f umfpack/umd2er.f umfpack/umd2p1.f umfpack/umd2p2.f umfpack/umd2lt.f umfpack/umd2ut.f umfpack/umd2rf.f umfpack/umd2ra.f umfpack/umd2r0.f umfpack/umd2r2.f umfpack/umd2rg.f umfpack/umd21i.f umfpack/mc21b.f umfpack/mc13e.f)

SET(algebra funciones/algebra/TablaLiterales funciones/algebra/ArbolExpr funciones/algebra/ExprAlgebra funciones/algebra/Expresion funciones/algebra/ExprInfija funciones/algebra/ExprPostfija funciones/algebra/LexAlgebra funciones/algebra/Lexico funciones/algebra/Literal funciones/algebra/OpBinario funciones/algebra/OpComparacion funciones/algebra/OpBooleano funciones/algebra/Macaulay funciones/algebra/Operando funciones/algebra/OpUnario funciones/algebra/ProtoExpr funciones/algebra/Rama funciones/algebra/Variable funciones/algebra/Operador funciones/algebra/Segnal funciones/algebra/ExprFunctor funciones/algebra/OpndoConNombre funciones/algebra/OpTrigonometricas funciones/algebra/ConjVariables funciones/algebra/MapValores funciones/algebra/integ_num)

SET(misc_funciones funciones/estadisticas/statlib funciones/estadisticas/Muestra funciones/estadisticas/combinatoria funciones/extremos/extremo funciones/macaulay/Macaulay funciones/polinomios/Polinomio funciones/polinomios/IndPol funciones/polinomios/NmbVars funciones/raices/NewtonRaphson funciones/raices/root funciones/funcs_forma/FForma funciones/funcs_forma/PtosIntegracion)

SET(funciones ${algebra} ${misc_funciones})

SET(MedXC  med_xc/CellModel med_xc/MEDMEM_CellModel med_xc/InterpKernelGaussCoords med_xc/InterpKernelException med_xc/MEDMEM_Coordinate med_xc/MEDMEM_DriversDef med_xc/MEDMEM_Exception med_xc/MEDMEM_Family med_xc/MEDMEM_Field med_xc/MEDMEM_GaussLocalization med_xc/MEDMEM_GenDriver med_xc/MEDMEM_Grid med_xc/MEDMEM_Group med_xc/MEDMEM_Init med_xc/MEDMEM_MedFileBrowser med_xc/MEDMEM_MedVersion med_xc/MEDMEM_MeshFuse med_xc/MEDMEM_PorflowMeshDriver med_xc/MEDMEM_RCBase med_xc/MEDMEM_SkyLineArray med_xc/MEDMEM_Support med_xc/MEDMEM_TopLevel med_xc/MEDMEM_Unit med_xc/MEDMEM_Connectivity med_xc/MEDMEM_ConnectZone med_xc/MEDMEM_DriverFactory med_xc/MEDMEM_DriverTools med_xc/MEDMEM_Mesh med_xc/MEDMEM_GMesh med_xc/PointLocator med_xc/MEDMEM_Meshing med_xc/MEDMEM_VtkMeshDriver med_xc/MEDMEM_GibiMeshDriver med_xc/MEDMEM_EnsightFieldDriver med_xc/MEDMEM_EnsightMeshDriver med_xc/MEDMEM_EnsightMedDriver med_xc/MEDMEM_EnsightUtils med_xc/MEDMEM_MedMeshDriver med_xc/MEDMEM_VtkMedDriver med_xc/MEDMEM_InterlacingPolicy)

ADD_LIBRARY(xc_basic ${tiempo} ${texto} ${streams} ${stl} ${util} ${sqlitepp} ${matrices} ${skypack} ${itpack} ${umfpack} ${funciones} ${unidades} ${MedXC})
INSTALL(TARGETS xc_basic DESTINATION lib)
 
SET(CPACK_GENERATOR "DEB")
INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "xc_basic")
SET(CPACK_PACKAGE_VENDOR "XC")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${xc_basic_DIR}/readme.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${xc_basic_DIR}/license.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "12")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

SET(CPACK_DEBIAN_PACKAGE_NAME "xc_basic")
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libstdc++6 (>=4.3), libgcc1 (>=1:4.3), libc6 (>=2.7), libvtk5 (>=5.0)")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Luis C. Pérez Tato") #required
SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Biblioteca de funciones para XC y otros programas.")
SET(CPACK_DEBIAN_PACKAGE_SECTION "contrib/science")
 
INCLUDE(CPack)
