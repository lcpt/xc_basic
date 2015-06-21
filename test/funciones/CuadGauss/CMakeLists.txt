
# Archivos fuente sin geom-2.0.
SET(SOURCES xCuadratura2D xPsoGss1d xPsoGss3d xCuadratura1D xPsoGss2d xPuntosGauss)

# Creamos ejecutables
FOREACH(source ${SOURCES})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} funciones)
ENDFOREACH(source)

# Archivos fuente con geom-2.0 y CGAL.
#Librería CGAL
INCLUDE_DIRECTORIES(${CGAL_INCL_DIR})
INCLUDE_DIRECTORIES(${CGAL_INCL_CONF_DIR})

SET(SRCSGEOM xCuadratura3D)

# Creamos ejecutables
FOREACH(source ${SRCSGEOM})
  ADD_EXECUTABLE(${source} ${source})
  TARGET_LINK_LIBRARIES(${source} lcmd VtkAux Texto vtkRendering vtkIO vtkHybrid geom-2.0 funciones CGAL Matrices boost_regex Coin mpfr gmp plotter)
ENDFOREACH(source)


