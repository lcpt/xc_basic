
# Archivos fuente.
SET(SOURCES1 xbusquedabruta xGetToken)

SET(SOURCES2 readlinebuf)

# Creamos ejecutables
FOREACH(source ${SOURCES1})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} streams Texto funciones)
ENDFOREACH(source)

FOREACH(source ${SOURCES2})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} streams Texto funciones readline)
ENDFOREACH(source)




