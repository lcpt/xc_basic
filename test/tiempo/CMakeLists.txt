# Nombre del proyecto.
PROJECT(TEST_TIEMPO)

INCLUDE($ENV{DIR_FUENTES_C}/macros_cmake/dirs_cmake)

ADD_DEFINITIONS(-Wall)
SET(CMAKE_VERBOSE_MAKEFILE ON)

# Make sure the compiler can find include files from our Hello library.
INCLUDE_DIRECTORIES(${DIR_LIBRERIA})

SET(SOURCES test_timer xagno xbisiesto xdias_agno xdiasemana xdias_mes xfecha xfechahora xhora xmes xTimer)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} Tiempo)
ENDFOREACH(source)
