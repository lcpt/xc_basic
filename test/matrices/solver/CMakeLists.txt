# Archivos fuente.
SET(SOURCES xcholsky xGaussJordan xlu4 xsolver_gauss2 xtridiag xcrout xinvierte xlu xsolver_gauss xgausim xjordan xorto xsolver_lu2 xgauss xlu3 xpotiter xsolver_lu3)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} Matrices)
ENDFOREACH(source)




