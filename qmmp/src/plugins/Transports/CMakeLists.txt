SET(USE_CURL TRUE CACHE BOOL "enable/disable curl-based http plugin")

IF(USE_CURL)
add_subdirectory(http)
ENDIF(USE_CURL)
