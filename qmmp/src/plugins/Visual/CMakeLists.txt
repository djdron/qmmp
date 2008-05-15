SET(USE_ANALYZER TRUE CACHE BOOL "enable/disable analyzer plugin")

IF(USE_ANALYZER)
add_subdirectory(analyzer)
ENDIF(USE_ANALYZER)
