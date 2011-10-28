cmake_minimum_required(VERSION 2.6.0)

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

SET(QMMP_PLUGIN_PACK_VERSION 0.6.0)
SET(SVN_VERSION TRUE)

SET(QT_MIN_VERSION 4.6.0)
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

#debian hack
IF(EXISTS ${QT_BINARY_DIR}/lrelease-qt4)
SET(QT_LRELEASE_EXECUTABLE ${QT_BINARY_DIR}/lrelease-qt4)
ENDIF(EXISTS ${QT_BINARY_DIR}/lrelease-qt4)

IF(QT_LRELEASE_EXECUTABLE)
MESSAGE(STATUS "Found lrelease executable: " ${QT_LRELEASE_EXECUTABLE})
ELSE(QT_LRELEASE_EXECUTABLE)
MESSAGE(FATAL_ERROR "Could NOT find lrelease executable")
ENDIF(QT_LRELEASE_EXECUTABLE)

MESSAGE(STATUS "Generating translations ...")
IF(SVN_VERSION)
execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent -nounfinished)
ELSE(SVN_VERSION)
execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name *.ts COMMAND xargs ${QT_LRELEASE_EXECUTABLE} -silent)
ENDIF(SVN_VERSION)

IF(NOT LIB_SUFFIX)
IF("${QT_LIBRARY_DIR}" MATCHES lib64)
set(LIB_SUFFIX 64)
ENDIF("${QT_LIBRARY_DIR}" MATCHES lib64)
ENDIF(NOT LIB_SUFFIX)

SET(LIB_DIR "lib${LIB_SUFFIX}" CACHE STRING "library path")

add_subdirectory(src)

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
    ARGS ${CMAKE_CURRENT_SOURCE_DIR} -name "*.qm" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_BINARY_DIR} -name "*.pc" | xargs rm -rf
    COMMAND find
    ARGS ${CMAKE_CURRENT_SOURCE_DIR} -name "*.depends" | xargs rm -rf
    COMMAND rm
    ARGS -rf ${CMAKE_CURRENT_BINARY_DIR}/install_manifest.txt
    TARGET  distclean
)
ENDIF(UNIX)

MESSAGE("")
MESSAGE("Plugins:")
PRINT_SUMMARY ("mpg123 plugin ..................." USE_MPG123  MPG123_FOUND)
PRINT_SUMMARY ("simple UI plugin ................" USE_SIMPLE  QMMPUI_FOUND)


MESSAGE("")
