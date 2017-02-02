PROJECT(TESTSTL)

INCLUDE($ENV{DIR_FUENTES_C}/macros_cmake/dirs_cmake)

INCLUDE_DIRECTORIES(${DIR_LIBRERIA})

# Archivos fuente.
SET(SOURCES xdeque xlist2 xlist xlist_int xmap xpdeque xpoli_deque xpset xpuntero xvector1 xvector2 xvector3 xvector_char xvector_double xvector_int)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} funciones)
ENDFOREACH(source)




