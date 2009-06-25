SET(USE_ALSA TRUE CACHE BOOL "enable/disable alsa plugin")
SET(USE_JACK TRUE CACHE BOOL "enable/disable jack plugin")
SET(USE_OSS TRUE CACHE BOOL "enable/disable oss plugin")
SET(USE_PULSE TRUE CACHE BOOL "enable/disable pulse audio plugin")

IF(USE_ALSA)
add_subdirectory(alsa)
ENDIF(USE_ALSA)

IF(USE_JACK)
#add_subdirectory(jack)
ENDIF(USE_JACK)

IF(USE_OSS)
add_subdirectory(oss)
ENDIF(USE_OSS)

IF(USE_PULSE)
add_subdirectory(pulseaudio)
ENDIF(USE_PULSE)
