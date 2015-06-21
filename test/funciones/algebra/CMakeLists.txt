
# Archivos fuente.
SET(SOURCES prueba xExprAlgebra1 xExprAlgebra2 xExprAlgebra3 xExprAlgebra4 xExprAlgebra5 xExprAlgebra6 xExprAlgebra7 xExprAlgebra8 xExprAlgebra9 xExprAlgebra10 xExprAlgebra11 xExprAlgebra12 xExprAlgebra xExpresion xExprTrigonometricas xInfija xLeeExprAlgebra xLexico xPostfija2 xPostfija xPow xAbs xSimpsonSimple xPi xOperadoresBooleanos xPruebaMemoria)

SET (SOURCESB xExprAlgebra13)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} funciones Texto)
ENDFOREACH(source)

# Creamos ejecutables
FOREACH(source ${SOURCESB})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} funciones Tiempo Texto)
ENDFOREACH(source)




