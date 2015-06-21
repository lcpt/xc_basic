
# Archivos fuente.
SET(SOURCES xcoeficiente_binomial xfactorial xMuestra)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} funciones)
ENDFOREACH(source)




