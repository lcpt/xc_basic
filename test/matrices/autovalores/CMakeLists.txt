# Archivos fuente.
SET(SOURCES xAtomMZ xAtoMZ xrust xsouriau)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} Matrices)
ENDFOREACH(source)




