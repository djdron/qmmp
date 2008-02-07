SET(USE_SRC TRUE CACHE BOOL "enable/disable SRC plugin")

IF(USE_SRC)
MESSAGE( STATUS "SRC ON")
add_subdirectory(srconverter)
ELSE(USE_SRC)
MESSAGE( STATUS "SRC OFF")
ENDIF(USE_SRC)
