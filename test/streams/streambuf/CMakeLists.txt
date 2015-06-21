
# Archivos fuente.
SET(SOURCES1 ifdstreambuf)

# Creamos ejecutables
FOREACH(source ${SOURCES1})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} streams Texto funciones)
ENDFOREACH(source)



