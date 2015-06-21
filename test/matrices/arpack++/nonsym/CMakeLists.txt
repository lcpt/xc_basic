# Archivos fuente.
SET(SOURCES1 prb_arpack_01)

ADD_DEFINITIONS(-Wall)

# Creamos ejecutables
FOREACH(source ${SOURCES1})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} Matrices arpack arpack++)
ENDFOREACH(source)

