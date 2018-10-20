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
message(STATUS "include dir: " ${xc_basic_INC})
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
set(Boost_DETAILED_FAILURE_MSG ON)
find_package(Boost 1.65.1 COMPONENTS regex math_c99 math_c99f math_c99l math_tr1 math_tr1f math_tr1l)
find_package(SQLITE3 REQUIRED)
find_package(ExecInfo)
find_package(MPI REQUIRED)

if(GTK2_FOUND)
  include_directories(${GTK2_INCLUDE_DIRS})
endif()
IF(USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
  message(STATUS "vtk file:  ${USE_VTK_FILE}")
  SET(VTK_FOUND 1)
ENDIF(USE_VTK_FILE)
if(MPI_FOUND)
  include_directories(${MPI_INCLUDE_PATH})
endif()

INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR})

SET(text text/text_string text/LabelContainer text/en_letra text/StrTok text/tab_cod text/latex text/StringFormatter text/FixedWidthReader)

SET(streams streams/imanip streams/prep)

SET(util util/cmd_args util/memoria util/inertia util/print_trace)

SET(sqlitepp sqlitepp/Database  sqlitepp/Query  sqlitepp/StderrLog  sqlitepp/SysLog)

SET(matrices matrices/MMatrix matrices/vcolM matrices/RangoIndice matrices/MatrixRange matrices/BoxConstRef matrices/ExpressionMatrix matrices/MExpressionMatrix matrices/PolynomialMatrix matrices/m_double matrices/m_int matrices/m_sizet matrices/m_string matrices/giros matrices/op_tensor matrices/solvers/solver matrices/IndicesMatrix matrices/Indices3dArray)

SET(skypack skypack/skypri.f skypack/skypds.f skypack/skyscf.f skypack/skyssb.f skypack/mtrxys.f skypack/skyssf.f skypack/skypsb.f skypack/skypdc.f skypack/skypf1.f skypack/skysdc.f skypack/skypsp.f skypack/skypcf.f skypack/skymef.f skypack/skyssd.f skypack/skyss.f skypack/skysf1.f skypack/skypf2.f skypack/mtrxy.f skypack/mtrxyd.f skypack/skysf2.f skypack/skypci.f)

SET(itpack itpack/dsrc2c.f itpack/jcg.f itpack/jsi.f itpack/rscg.f itpack/rssi.f itpack/sor.f itpack/ssorcg.f itpack/ssorsi.f)

SET(umfpack umfpack/umd2fb.f umfpack/umd2co.f umfpack/umd2fa.f umfpack/umd2f0.f umfpack/umd2f1.f umfpack/umd2f2.f umfpack/umd2fg.f umfpack/umd2in.f umfpack/umd2of.f umfpack/umd2s2.f umfpack/umd2sl.f umfpack/umd2so.f umfpack/umd2su.f umfpack/umd2er.f umfpack/umd2p1.f umfpack/umd2p2.f umfpack/umd2lt.f umfpack/umd2ut.f umfpack/umd2rf.f umfpack/umd2ra.f umfpack/umd2r0.f umfpack/umd2r2.f umfpack/umd2rg.f umfpack/umd21i.f umfpack/mc21b.f umfpack/mc13e.f)

SET(algebra functions/algebra/TablaLiterales functions/algebra/ArbolExpr functions/algebra/ExprAlgebra functions/algebra/Expresion functions/algebra/ExprInfija functions/algebra/ExprPostfija functions/algebra/LexAlgebra functions/algebra/Lexico functions/algebra/Literal functions/algebra/OpBinario functions/algebra/OpComparacion functions/algebra/OpBooleano functions/algebra/Macaulay functions/algebra/Operando functions/algebra/OpUnario functions/algebra/ProtoExpr functions/algebra/Rama functions/algebra/Variable functions/algebra/Operador functions/algebra/Segnal functions/algebra/ExprFunctor functions/algebra/OpndoConNombre functions/algebra/OpTrigonometricas functions/algebra/ConjVariables functions/algebra/MapValores functions/algebra/integ_num)

SET(functions_misc functions/estadisticas/statlib functions/estadisticas/Muestra functions/estadisticas/combinatoria functions/extremos/extremo functions/macaulay/Macaulay functions/polinomios/Polinomio functions/polinomios/IndPol functions/polinomios/NmbVars functions/raices/NewtonRaphson functions/raices/root functions/funcs_forma/FForma functions/funcs_forma/PtosIntegracion)

SET(functions ${algebra} ${functions_misc})

ADD_LIBRARY(xc_basic ${tiempo} ${text} ${streams} ${stl} ${util} ${sqlitepp} ${matrices} ${skypack} ${itpack} ${umfpack} ${functions} ${unidades})
INSTALL(TARGETS xc_basic DESTINATION lib)
 
