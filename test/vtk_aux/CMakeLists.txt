cmake_minimum_required(VERSION 2.6)
PROJECT(VTKBOOK)
SET(DIR_FUENTES $ENV{DIR_FUENTES_C})
SET(DIR_LIBRERIA ${DIR_FUENTES}/biblioteca)

INCLUDE_DIRECTORIES(${DIR_LIBRERIA})
INCLUDE(${CMAKE_ROOT}/Modules/FindVTK.cmake)

ADD_DEFINITIONS(-Wall)
SET(CMAKE_VERBOSE_MAKEFILE ON)


INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR})
# Make sure the compiler can find include files from our Hello library.
INCLUDE_DIRECTORIES(${DIR_LIBRERIA})

IF(USE_VTK_FILE)
  INCLUDE(${USE_VTK_FILE})
  SET(VTK_FOUND 1)
ENDIF(USE_VTK_FILE)

#ADD_LIBRARY(vtkgtk vtkGtkRenderWindowInteractor)

IF(VTK_FOUND)
  SET(FILES TakePhoto)
  FOREACH(f ${FILES})
    ADD_EXECUTABLE(${f} ${f})
    TARGET_LINK_LIBRARIES(${f} vtkRendering vtkIO vtkHybrid)
  ENDFOREACH(f)

ENDIF(VTK_FOUND)
