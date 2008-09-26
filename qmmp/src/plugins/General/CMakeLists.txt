SET(USE_DBUS TRUE CACHE BOOL "enable/disable dbus plugin")
SET(USE_SCROBBLER TRUE CACHE BOOL "enable/disable scrobbler plugin")
SET(USE_STATICON TRUE CACHE BOOL "enable/disable status icon plugin")
SET(USE_NOTIFIER TRUE CACHE BOOL "enable/disable notifier plugin")

IF(USE_DBUS)
add_subdirectory(dbuscontrol)
ENDIF(USE_DBUS)

IF(USE_SCROBBLER)
#add_subdirectory(scrobbler)
ENDIF(USE_SCROBBLER)

IF(USE_STATICON)
add_subdirectory(statusicon)
ENDIF(USE_STATICON)

IF(USE_NOTIFIER)
add_subdirectory(notifier)
ENDIF(USE_NOTIFIER)
