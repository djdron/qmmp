project(libffap)

cmake_minimum_required(VERSION 2.8.0)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)




# qt plugin
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

REMOVE_DEFINITIONS(-Wall)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})


include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})

SET(libffap_SRCS
  decoder_ffap.cpp
  decoder_ffapcue.cpp
  decoderffapfactory.cpp
  ffapmetadatamodel.cpp
  cueparser.cpp
  replaygainreader.cpp
  ffap.c


)

SET(libffap_HDRS
  decoder_ffap.h
  decoder_ffapcue.h
  cueparser.h
  replaygainreader.h
  ffap.h
)

SET(libffap_MOC_HDRS
  decoderffapfactory.h
  ffapmetadatamodel.h
)

IF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86_64)$")
  set(CMAKE_ASM_NASM_COMPILER "yasm")
  enable_language(ASM_NASM)
  SET(libffap_SRCS ${libffap_SRCS} dsputil_yasm.asm x86inc.asm)
  IF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86)$")
     ADD_DEFINITIONS(-DARCH_X86_32)
     set_source_files_properties(x86inc.asm dsputil_yasm.asm PROPERTIES COMPILE_FLAGS "-m x86")
     message("-- Architecture: x86")
  ELSEIF(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
     ADD_DEFINITIONS(-DARCH_X86_64)
     set_source_files_properties(x86inc.asm dsputil_yasm.asm PROPERTIES COMPILE_FLAGS "-m amd64")
     message("-- Architecture: x86_64")   
  ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86)$")
ELSE(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86_64)$")
  ADD_DEFINITIONS(-DARCH_UNKNOWN)
  message("-- Architecture: unknown")
ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86_64)$")


FOREACH (SOURCE ${libffap_SRCS})
  IF (NOT ${SOURCE} MATCHES "asm$")
    set_source_files_properties(${SOURCE} PROPERTIES COMPILE_FLAGS "-Wall")
  ENDIF()
ENDFOREACH()

SET(libffap_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libffap_RCC_SRCS ${libffap_RCCS})

QT4_WRAP_CPP(libffap_MOC_SRCS ${libffap_MOC_HDRS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})


ADD_LIBRARY(ffap MODULE ${libffap_SRCS} ${libffap_MOC_SRCS} ${libffap_RCC_SRCS} ${libffap_HDRS})
add_dependencies(ffap qmmp)
target_link_libraries(ffap ${QT_LIBRARIES} -lqmmp ${ffap_LDFLAGS} ${TAGLIB_LDFLAGS})
install(TARGETS ffap DESTINATION ${LIB_DIR}/qmmp/Input)
