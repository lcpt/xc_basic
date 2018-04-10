cmake_minimum_required(VERSION 2.6)

# Nombre del proyecto.
PROJECT(TESTLIBTEXTO)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/share/cmake_modules)
SET(DIR_FUENTES $ENV{DIR_FUENTES_C})
SET(DIR_LIBRERIA ${DIR_FUENTES}/biblioteca)

INCLUDE_DIRECTORIES(${DIR_LIBRERIA})

# Make sure the compiler can find include files from our Hello library.
INCLUDE_DIRECTORIES(${DIR_LIBRERIA})

# Make sure the linker can find the Hello library once it is built.
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})


ADD_DEFINITIONS(-Wall)

SUBDIRS(text_string en_letra latex pcl pjl rtf)
