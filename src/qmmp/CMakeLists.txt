project(libqmmp)

cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 OLD)
    cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy)

# qt lib
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)
ADD_DEFINITIONS(-DLIB_DIR=\\\"\\/${LIB_DIR}\\\")
ADD_DEFINITIONS(-DQMMP_VERSION=${QMMP_VERSION})
ADD_DEFINITIONS(-DQMMP_STR_VERSION=\\\"${QMMP_VERSION}\\\")
ADD_DEFINITIONS(-DQMMP_INSTALL_PREFIX=\\\"${CMAKE_INSTALL_PREFIX}\\\")

IF(SVN_VERSION) #add svn revision
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/svn_revision.sh
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE SVN_REVISION
                OUTPUT_STRIP_TRAILING_WHITESPACE)
ADD_DEFINITIONS(-DSVN_REVISION=\\\"${SVN_REVISION}\\\")
ELSE(SVN_VERSION)

ENDIF(SVN_VERSION)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(libqmmp_SRCS
    visual.cpp
    recycler.cpp
    decoder.cpp
    output.cpp
    fileinfo.cpp
    equ/iir.c
    equ/iir_cfs.c
    equ/iir_fpu.c
    soundcore.cpp
    effect.cpp
    qmmp.cpp
    statehandler.cpp
    volumecontrol.cpp
    metadatamodel.cpp
    tagmodel.cpp
    qmmpaudioengine.cpp
    abstractengine.cpp
    audioparameters.cpp
    inputsource.cpp
    fileinputsource.cpp
    emptyinputsource.cpp
    metadatamanager.cpp
    replaygain.cpp
    qmmpsettings.cpp
    audioconverter.cpp
    eqsettings.cpp
    statechangedevent.cpp
    metadatachangedevent.cpp
)

SET(libqmmp_MOC_HDRS
    visual.h
    visualfactory.h
    recycler.h
    buffer.h
    decoder.h
    output.h
    fileinfo.h
    outputfactory.h
    equ/iir_cfs.h
    equ/iir_fpu.h
    equ/iir.h
    decoderfactory.h
    soundcore.h
    effectfactory.h
    effect.h
    qmmp.h
    statehandler.h
    volumecontrol.h
    metadatamodel.h
    tagmodel.h
    qmmpaudioengine.h
    abstractengine.h
    audioparameters.h
    inputsource.h
    inputsourcefactory.h
    fileinputsource.h
    emptyinputsource.h
    enginefactory.h
    metadatamanager.h
    replaygain.h
    qmmpsettings.h
    audioconverter.h
    eqsettings.h
    statechangedevent.h
    metadatachangedevent.h
)

SET(libqmmp_DEVEL_HDRS
    visual.h
    decoderfactory.h
    visualfactory.h
    recycler.h
    buffer.h
    decoder.h
    output.h
    fileinfo.h
    outputfactory.h
    soundcore.h
    effectfactory.h
    effect.h
    qmmp.h
    statehandler.h
    volumecontrol.h
    metadatamodel.h
    tagmodel.h
    abstractengine.h
    audioparameters.h
    inputsource.h
    inputsourcefactory.h
    enginefactory.h
    metadatamanager.h
    qmmpsettings.h
    eqsettings.h
)

QT4_WRAP_CPP(libqmmp_MOC_SRCS ${libqmmp_MOC_HDRS})

# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(libqmmp SHARED ${libqmmp_SRCS} ${libqmmp_MOC_SRCS})
target_link_libraries(libqmmp ${QT_LIBRARIES})
SET_TARGET_PROPERTIES(libqmmp PROPERTIES VERSION ${QMMP_VERSION} SOVERSION ${QMMP_SOVERSION} OUTPUT_NAME qmmp)
install(TARGETS libqmmp LIBRARY DESTINATION ${LIB_DIR}
                        RUNTIME DESTINATION bin
                        ARCHIVE DESTINATION ${LIB_DIR})
install(FILES ${libqmmp_DEVEL_HDRS} DESTINATION include/qmmp)
