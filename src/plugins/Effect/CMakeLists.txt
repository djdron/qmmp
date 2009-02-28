SET(USE_SRC TRUE CACHE BOOL "enable/disable SRC plugin")
SET(USE_BS2B TRUE CACHE BOOL "enable/disable bs2b plugin")

IF(USE_SRC)
add_subdirectory(srconverter)
ENDIF(USE_SRC)

IF(USE_BS2B)
add_subdirectory(bs2b)
ENDIF(USE_BS2B)
