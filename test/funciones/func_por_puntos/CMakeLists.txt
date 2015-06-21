
# Archivos fuente.
SET(SOURCES xFuncPorPuntosR2_R xFuncPorPuntosR_R xFuncPorPuntosR2_T)

#Librería CGAL
INCLUDE_DIRECTORIES(${CGAL_INCL_DIR})
INCLUDE_DIRECTORIES(${CGAL_INCL_CONF_DIR})

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} lcmd VtkAux Texto Tiempo vtkRendering vtkIO vtkHybrid geom-2.0 funciones CGAL Matrices plotter mpfr gmp Coin boost_regex)
ENDFOREACH(source)




