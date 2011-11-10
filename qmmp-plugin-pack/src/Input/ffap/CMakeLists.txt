project(libffap)

cmake_minimum_required(VERSION 2.6.0)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)

enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_COMPILER "yasm")

# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libqmmp
include_directories(${QMMP_INCLUDE_DIRS})
link_directories( ${QMMP_LIBRARY_DIRS})


include_directories(${TAGLIB_INCLUDE_DIRS})
link_directories(${TAGLIB_LIBRARY_DIRS})
ADD_DEFINITIONS(${TAGLIB_CFLAGS})

SET(libffap_SRCS
  decoder_ffap.cpp
  decoderffapfactory.cpp
  ffapmetadatamodel.cpp
  cueparser.cpp
  replaygainreader.cpp
  ffap.c
  dsputil_yasm.asm
  x86inc.asm
)

SET(libffap_HDRS
  decoder_ffap.h
  cueparser.h
  replaygainreader.h
  ffap.h
)

SET(libffap_MOC_HDRS
  decoderffapfactory.h
  ffapmetadatamodel.h
)


#foreach (SOURCE ${libffap_SRCS})
#    if (${SOURCE} MATCHES "asm$")
        set_source_files_properties(x86inc.asm PROPERTIES COMPILE_FLAGS "-w")
#    endif()
#endforeach() 

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
