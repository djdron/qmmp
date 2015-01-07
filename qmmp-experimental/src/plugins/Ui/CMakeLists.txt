SET(USE_SKINNED TRUE CACHE BOOL "enable/disable skinned user interface")

IF(USE_SKINNED)
add_subdirectory(skinned)
ENDIF(USE_SKINNED)
