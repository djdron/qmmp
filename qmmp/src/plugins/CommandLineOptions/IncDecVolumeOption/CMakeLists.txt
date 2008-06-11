project(libincdecvolumeoption)

cmake_minimum_required(VERSION 2.4.0)

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
  ${CMAKE_CURRENT_BINARY_DIR}/../../../
)

# libqmmpui
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmpui)

SET(libincdecvolumeoption_SRCS
    incdecvolumeoption.cpp
)

SET(libincdecvolumeoption_MOC_HDRS
    incdecvolumeoption.h
)

SET(libincdecvolumeoption_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libincdecvolumeoption_RCC_SRCS ${libincdecvolumeoption_RCCS})

QT4_WRAP_CPP(libincdecvolumeoption_MOC_SRCS ${libincdecvolumeoption_MOC_HDRS})


# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(incdecvolumeoption SHARED ${libincdecvolumeoption_SRCS} ${libincdecvolumeoption_MOC_SRCS} 
           ${libincdecvolumeoption_RCC_SRCS})
target_link_libraries(incdecvolumeoption ${QT_LIBRARIES} -lqmmpui)
install(TARGETS incdecvolumeoption DESTINATION ${LIB_DIR}/qmmp/CommandLineOptions)
