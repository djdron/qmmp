SET(USE_MPRIS TRUE CACHE BOOL "enable/disable mpris plugin")
SET(USE_SCROBBLER TRUE CACHE BOOL "enable/disable scrobbler plugin")
SET(USE_STATICON TRUE CACHE BOOL "enable/disable status icon plugin")
SET(USE_NOTIFIER TRUE CACHE BOOL "enable/disable notifier plugin")
SET(USE_LYRICS TRUE CACHE BOOL "enable/disable lyrics version")
SET(USE_HAL TRUE CACHE BOOL "enable/disable hal plugin")
SET(USE_HOTKEY TRUE CACHE BOOL "enable/disable global hotkey plugin")
SET(USE_FILEOPS TRUE CACHE BOOL "enable/disable fileops plugin")

IF(USE_MPRIS)
add_subdirectory(mpris)
ENDIF(USE_MPRIS)

IF(USE_SCROBBLER)
add_subdirectory(scrobbler)
ENDIF(USE_SCROBBLER)

IF(USE_STATICON)
add_subdirectory(statusicon)
ENDIF(USE_STATICON)

IF(USE_NOTIFIER)
add_subdirectory(notifier)
ENDIF(USE_NOTIFIER)

IF(USE_LYRICS)
add_subdirectory(lyrics)
ENDIF(USE_LYRICS)

IF(USE_HAL)
add_subdirectory(hal)
ENDIF(USE_HAL)

IF(USE_HOTKEY)
add_subdirectory(hotkey)
ENDIF(USE_HOTKEY)

IF(USE_FILEOPS)
add_subdirectory(fileops)
ENDIF(USE_FILEOPS)
