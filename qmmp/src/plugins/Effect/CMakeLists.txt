SET(USE_SRC TRUE CACHE BOOL "enable/disable SRC plugin")

IF(USE_SRC)
add_subdirectory(srconverter)
ENDIF(USE_SRC)
