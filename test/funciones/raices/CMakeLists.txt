
# Archivos fuente.
SET(SOURCES tsroot test_biseccion)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} funciones Matrices)
ENDFOREACH(source)




