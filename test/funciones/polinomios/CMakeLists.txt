
# Archivos fuente.
SET(SOURCES xIndPol xIndPol2 xFuncion xNmbVars xPol xPol2 xPol3 xPol4 xPol5 xPol6 xPol7 xpolmio)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} funciones)
ENDFOREACH(source)




