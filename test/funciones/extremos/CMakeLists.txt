
# Archivos fuente.
SET(SOURCES xminx)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} funciones)
ENDFOREACH(source)




