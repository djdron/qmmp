project(libplaylistoption)

cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


# qt plugin
ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../
)

# libqmmpui
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

SET(libplaylistoption_SRCS
    playlistoption.cpp
)

SET(libplaylistoption_MOC_HDRS
    playlistoption.h
)

SET(libplaylistoption_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libplaylistoption_RCC_SRCS ${libplaylistoption_RCCS})

QT4_WRAP_CPP(libplaylistoption_MOC_SRCS ${libplaylistoption_MOC_HDRS})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(playlistoption MODULE ${libplaylistoption_SRCS} ${libplaylistoption_MOC_SRCS} 
           ${libplaylistoption_RCC_SRCS})
add_dependencies(playlistoption qmmpui)
target_link_libraries(playlistoption ${QT_LIBRARIES} -lqmmpui -lqmmp)
install(TARGETS playlistoption DESTINATION ${LIB_DIR}/qmmp/CommandLineOptions)
