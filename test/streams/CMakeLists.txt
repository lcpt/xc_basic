PROJECT(TESTSTRMS)

INCLUDE($ENV{DIR_FUENTES_C}/macros_cmake/dirs_cmake)

INCLUDE_DIRECTORIES(${DIR_LIBRERIA})

#Biblioteca CGAL
INCLUDE_DIRECTORIES(${CGAL_INCL_DIR})
INCLUDE_DIRECTORIES(${CGAL_INCL_CONF_DIR})

# Make sure the compiler can find include files from our streams library.
INCLUDE_DIRECTORIES(${TESTSTRMS_SOURCE_DIR}/streams)

# Make sure the linker can find the streams library once it is built.
LINK_DIRECTORIES(${TESTSTRMS_BINARY_DIR}/streams)

ADD_DEFINITIONS(-Wall)
SET(CMAKE_VERBOSE_MAKEFILE ON)

SUBDIRS(misc streambuf)

