cmake_minimum_required(VERSION 2.4.7)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

SET(QMMP_VERSION 0.3.0)
SET(QMMP_SOVERSION 0)
SET(SVN_VERSION TRUE)

INCLUDE(FindQt4)
INCLUDE(${QT_USE_FILE})
INCLUDE(FindPkgConfig)

IF(NOT QT_LRELEASE_EXECUTABLE)
FIND_PROGRAM(QT_LRELEASE_EXECUTABLE 
    NAMES lrelease
    PATHS ${QT_BINARY_DIR}
    NO_DEFAULT_PATH
)
ENDIF(NOT QT_LRELEASE_EXECUTABLE)

IF(QT_LRELEASE_EXECUTABLE)
MESSAGE(STATUS "Found lrelease executable: " ${QT_LRELEASE_EXECUTABLE})
ELSE(QT_LRELEASE_EXECUTABLE)
MESSAGE(FATAL_ERROR "Could NOT find lrelease executable")
ENDIF(QT_LRELEASE_EXECUTABLE)

MESSAGE(STATUS "Generating translations ...")
execute_process(COMMAND find ${CMAKE_CURRENT_BINARY_DIR} -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent)

SET(LIB_DIR "lib" CACHE STRING "library path")
add_subdirectory(src/qmmp)
add_subdirectory(src/qmmpui)
add_subdirectory(src/ui)
add_subdirectory(src/plugins)

MACRO(PRINT_SUMMARY str USE FOUND)
IF(${USE} AND ${FOUND})
MESSAGE("${str}enabled")
ELSE(${USE} AND ${FOUND})
MESSAGE("${str}disabled")
ENDIF(${USE} AND ${FOUND})
ENDMACRO(PRINT_SUMMARY)

IF (UNIX)
ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)

ADD_CUSTOM_COMMAND(
    COMMENT "distribution clean"
    COMMAND make
    ARGS -C ${CMAKE_CURRENT_BINARY_DIR} clean
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "CMakeCache.txt" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "CMakeFiles" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "Makefile" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name \"*.cmake\" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*.qm" | xargs rm -rf
    COMMAND rm
    ARGS -rf ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
    TARGET  distclean
)
ENDIF(UNIX)


MESSAGE("")
MESSAGE("Input Plugins:")
PRINT_SUMMARY ("mp3 support ......................." USE_MAD  MAD_FOUND)
PRINT_SUMMARY ("FLAC support ......................" USE_FLAC FLAC_FOUND)
PRINT_SUMMARY ("Ogg Vorbis support ................" USE_VORBIS VORBIS_FOUND)
PRINT_SUMMARY ("Musepack support .................." USE_MPC MPC_FOUND)
PRINT_SUMMARY ("MOD support ......................." USE_MODPLUG MODPLUG_FOUND)
PRINT_SUMMARY ("Wave support ......................" USE_SNDFILE SNDFILE_FOUND)
PRINT_SUMMARY ("WavPack support ..................." USE_WAVPACK WAVPACK_FOUND)
PRINT_SUMMARY ("WMA support ......................." USE_FFMPEG FFMPEG_FOUND)

MESSAGE("")
MESSAGE("Output Plugins:")
PRINT_SUMMARY ("ALSA support ......................" USE_ALSA ALSA_FOUND)
PRINT_SUMMARY ("OSS support ......................." USE_OSS 1)
PRINT_SUMMARY ("JACK support ......................" USE_JACK JACK_FOUND)
PRINT_SUMMARY ("PulseAudio support ................" USE_PULSE PULSE_FOUND)

MESSAGE("")
MESSAGE("Effect Plugins:")
PRINT_SUMMARY ("Sample Rate Conversion ............" USE_SRC SAMPLERATE_FOUND)

MESSAGE("")
MESSAGE("Visual Plugins:")
PRINT_SUMMARY ("Analyzer .........................." USE_ANALYZER 1)

MESSAGE("")
MESSAGE("General Plugins:")
PRINT_SUMMARY ("DBUS support ......................" USE_DBUS 1)
PRINT_SUMMARY ("Scrobbler ........................." USE_SCROBBLER 1)
PRINT_SUMMARY ("Tray support ......................" USE_STATICON 1)
PRINT_SUMMARY ("Notifier support .................." USE_NOTIFIER 1)

MESSAGE("")
MESSAGE("File Dialogs:")
PRINT_SUMMARY ("QMMP File Dialog .................." USE_QMMP_DIALOG 1)
MESSAGE("")



