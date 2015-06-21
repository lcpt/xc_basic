PROJECT(TESTMATRICES)

INCLUDE($ENV{DIR_FUENTES_C}/macros_cmake/dirs_cmake)

INCLUDE_DIRECTORIES(${DIR_LIBRERIA})

# Make sure the compiler can find include files from our funciones library.
INCLUDE_DIRECTORIES(${TESTFNC_SOURCE_DIR}/funciones)

# Make sure the linker can find the funciones library once it is built.
LINK_DIRECTORIES(${TESTFNC_BINARY_DIR}/funciones)


SUBDIRS(autovalores operaciones solver misc normas polinomios vector referencias arpack++)

