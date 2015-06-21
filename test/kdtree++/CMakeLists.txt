cmake_minimum_required(VERSION 2.4)
PROJECT(TESTMEDMEM)
SET(DIR_FUENTES_LIB $ENV{DIR_FUENTES_LIB})
SET(xc_basic_DIR ${DIR_FUENTES_LIB}/xc_basic)

INCLUDE_DIRECTORIES(${DIR_FUENTES_LIB} /usr/lib/openmpi/include/)

SET(CMAKE_VERBOSE_MAKEFILE ON)

# Archivos fuente.
SET(SOURCES test_hayne test_kdtree)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} xc_basic med)
ENDFOREACH(source)




