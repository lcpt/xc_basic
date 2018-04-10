# Add executable called "xq_car" that is built from the source file
# "xq_car.cc".  The extensions are automatically found.
ADD_EXECUTABLE(xq_car xq_car)
# Link the executable to the Texto library.
TARGET_LINK_LIBRARIES(xq_car xc_basic)

ADD_EXECUTABLE(xgetargs xgetargs)
# Link the executable to the Texto library.
TARGET_LINK_LIBRARIES(xgetargs xc_basic)

ADD_EXECUTABLE(xStrTok xStrTok)
# Link the executable to the Texto library.
TARGET_LINK_LIBRARIES(xStrTok xc_basic)

ADD_EXECUTABLE(xtab_cod xtab_cod)
# Link the executable to the Texto library.
TARGET_LINK_LIBRARIES(xtab_cod xc_basic)

ADD_EXECUTABLE(xIsNumber xIsNumber)
# Link the executable to the Texto library.
TARGET_LINK_LIBRARIES(xIsNumber xc_basic)

ADD_EXECUTABLE(xFixedWidthReader xFixedWidthReader)
# Link the executable to the Texto library.
TARGET_LINK_LIBRARIES(xFixedWidthReader xc_basic)

ADD_EXECUTABLE(xLabelContainer xLabelContainer)
# Link the executable to the Texto library.
TARGET_LINK_LIBRARIES(xLabelContainer xc_basic)
