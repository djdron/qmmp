project(libzxtune)

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_AUTOMOC OFF)
set(CMAKE_INCLUDE_CURRENT_DIR OFF)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018 /wd4244 /wd4800 /wd4996")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} /wd4018 /wd4090")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedef")
endif()


#boost
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-DHAVE_BOOST)

#zxtune
SET(ZXTUNE_FOUND TRUE CACHE BOOL INTERNAL)
add_definitions(-DNO_DEBUG_LOGS)
set(ZXTUNE_DIR "../../../../../zxtune")

include_directories(${ZXTUNE_DIR})
include_directories(${ZXTUNE_DIR}/include)
include_directories(${ZXTUNE_DIR}/src)

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

#3rdparty
add_subdirectory("${ZXTUNE_DIR}/3rdparty/gme" "gme")
add_subdirectory("${ZXTUNE_DIR}/3rdparty/hvl" "hvl")
add_subdirectory("${ZXTUNE_DIR}/3rdparty/lhasa" "lhasa")
add_subdirectory("${ZXTUNE_DIR}/3rdparty/lzma" "lzma")
add_subdirectory("${ZXTUNE_DIR}/3rdparty/sidplayfp" "sidplayfp")
add_subdirectory("${ZXTUNE_DIR}/3rdparty/snesspc" "snesspc")
add_subdirectory("${ZXTUNE_DIR}/3rdparty/unrar" "unrar")
add_subdirectory("${ZXTUNE_DIR}/3rdparty/xmp" "xmp")
add_subdirectory("${ZXTUNE_DIR}/3rdparty/z80ex" "z80ex")

#core
add_subdirectory("${ZXTUNE_DIR}/src/analysis" "analysis")
add_subdirectory("${ZXTUNE_DIR}/src/binary" "binary")
add_subdirectory("${ZXTUNE_DIR}/src/binary/format" "binary_format")
add_subdirectory("${ZXTUNE_DIR}/src/core" "core")
add_subdirectory("${ZXTUNE_DIR}/src/core/plugins/archives" "core_plugins_archives")
add_subdirectory("${ZXTUNE_DIR}/src/core/plugins/containers" "core_plugins_containers")
add_subdirectory("${ZXTUNE_DIR}/src/core/plugins/players" "core_plugins_players")
add_subdirectory("${ZXTUNE_DIR}/src/debug" "dbg")
add_subdirectory("${ZXTUNE_DIR}/src/devices/aym" "devices_aym")
add_subdirectory("${ZXTUNE_DIR}/src/devices/beeper" "devices_beeper")
add_subdirectory("${ZXTUNE_DIR}/src/devices/dac" "devices_dac")
add_subdirectory("${ZXTUNE_DIR}/src/devices/fm" "devices_fm")
add_subdirectory("${ZXTUNE_DIR}/src/devices/saa" "devices_saa")
add_subdirectory("${ZXTUNE_DIR}/src/devices/z80" "devices_z80")
add_subdirectory("${ZXTUNE_DIR}/src/formats/archived" "formats_archived")
add_subdirectory("${ZXTUNE_DIR}/src/formats/archived/multitrack" "formats_archived_multitrack")
add_subdirectory("${ZXTUNE_DIR}/src/formats/chiptune" "formats_chiptune")
add_subdirectory("${ZXTUNE_DIR}/src/formats/packed" "formats_packed")
add_subdirectory("${ZXTUNE_DIR}/src/formats/multitrack" "formats_multitrack")
add_subdirectory("${ZXTUNE_DIR}/src/l10n/stub" "l10n_stub")
add_subdirectory("${ZXTUNE_DIR}/src/parameters" "parameters")
add_subdirectory("${ZXTUNE_DIR}/src/platform/version" "platform_version")
add_subdirectory("${ZXTUNE_DIR}/src/sound" "sound")
add_subdirectory("${ZXTUNE_DIR}/src/strings" "strings")
add_subdirectory("${ZXTUNE_DIR}/src/tools" "tools")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libzxtune_SRCS
  decoder_zxtune.cpp
  decoderzxtunefactory.cpp
)

SET(libzxtune_HDRS
  decoder_zxtune.h
  decoderzxtunefactory.h
  player.h
)

#SET(libzxtune_RCCS translations/translations.qrc)

#QT5_ADD_RESOURCES(libzxtune_RCC_SRCS ${libzxtune_RCCS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

IF(ZXTUNE_FOUND)
set(LIBS gme hvl lhasa lzma sidplayfp snesspc unrar xmp z80ex analysis binary binary_format core core_plugins_archives core_plugins_containers core_plugins_players dbg devices_aym devices_beeper devices_dac devices_fm devices_saa devices_z80 formats_archived formats_archived_multitrack formats_chiptune formats_packed formats_multitrack l10n_stub parameters platform_version sound strings tools)
ADD_LIBRARY(zxtune MODULE ${libzxtune_SRCS} ${libzxtune_RCC_SRCS} ${libzxtune_HDRS})
add_dependencies(zxtune qmmp ${LIBS})
target_link_libraries(zxtune Qt5::Widgets -lqmmp ${LIBS} ${LIBS} ${ZLIB_LIBRARIES})
install(TARGETS zxtune DESTINATION ${LIB_DIR}/qmmp/Input)
ENDIF(ZXTUNE_FOUND)
