SET(USE_MPG123 TRUE CACHE BOOL "enable/disable mpg123 plugin")

INCLUDE(FindPkgConfig)
pkg_check_modules(TAGLIB taglib)

IF(USE_MPG123 AND QMMP_FOUND)
add_subdirectory(mpg123)
ENDIF(USE_MPG123 AND QMMP_FOUND)

