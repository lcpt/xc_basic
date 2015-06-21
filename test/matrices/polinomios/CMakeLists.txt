# Archivos fuente.
SET(SOURCES xmatpol1 xmatpol2 xmatpol4 xmatpol xPolinomio)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} Matrices funciones)
ENDFOREACH(source)




