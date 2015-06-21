# Archivos fuente.
SET(SOURCES xvectorZ2 xvectorZ3 xvectorZ4 xvectorZ5 xvectorZ)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} Matrices)
ENDFOREACH(source)




