PROJECT(TESTFNC)

INCLUDE($ENV{DIR_FUENTES_C}/macros_cmake/dirs_cmake)
INCLUDE(${CMAKE_ROOT}/Modules/FindVTK.cmake)

INCLUDE($ENV{DIR_FUENTES_C}/macros_cmake/dirs_cmake)
INCLUDE_DIRECTORIES(${DIR_LIBRERIA})

#Biblioteca CGAL
INCLUDE_DIRECTORIES(${CGAL_INCL_DIR})
INCLUDE_DIRECTORIES(${CGAL_INCL_CONF_DIR})

# Make sure the compiler can find include files from our funciones library.
INCLUDE_DIRECTORIES(${TESTFNC_SOURCE_DIR}/funciones)

# Make sure the linker can find the funciones library once it is built.
LINK_DIRECTORIES(${TESTFNC_BINARY_DIR}/funciones)

ADD_DEFINITIONS(-Wall)
SET(CMAKE_VERBOSE_MAKEFILE ON)

IF(USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
  SET(VTK_FOUND 1)
ENDIF(USE_VTK_FILE)

SUBDIRS(algebra CuadGauss estadisticas func_por_puntos op_dif raices extremos macaulay polinomios)

