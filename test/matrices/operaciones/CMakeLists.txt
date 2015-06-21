# Archivos fuente.
SET(SOURCES xAbs xdet xDot xpvect)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} Matrices)
ENDFOREACH(source)




