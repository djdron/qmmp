SET(QMMP_VERSION 0.2.0)
SET(QMMP_SOVERSION 0)
SET(SVN_VERSION TRUE)

SET(LIB_DIR "lib" CACHE STRING "library path")
add_subdirectory(src/qmmp)
add_subdirectory(src/qmmpui)
add_subdirectory(src/ui)
add_subdirectory(src/plugins)

