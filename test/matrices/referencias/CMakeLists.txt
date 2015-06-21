# Archivos fuente.
SET(SOURCES1 xConstRefCaja xConstRefFila xConstRefCol)

# Creamos ejecutables
FOREACH(source ${SOURCES1})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} Matrices)
ENDFOREACH(source)




