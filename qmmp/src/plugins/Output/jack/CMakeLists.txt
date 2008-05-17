project(libjack)

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

# libqmmp
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../../../qmmp)

# libjack and samplerate
pkg_check_modules(JACK jack samplerate)

include_directories(${JACK_INCLUDE_DIRS})
link_directories(${JACK_LIBRARY_DIRS})

SET(libjack_SRCS
  outputjackfactory.cpp
  outputjack.cpp
  bio2jack.c
)

SET(libjack_MOC_HDRS
  outputjackfactory.h
  outputjack.h
  bio2jack.h
)

SET(libjack_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libjack_RCC_SRCS ${libjack_RCCS})

QT4_WRAP_CPP(libjack_MOC_SRCS ${libjack_MOC_HDRS})


IF(JACK_FOUND)
ADD_LIBRARY(jack SHARED ${libjack_SRCS} ${libjack_MOC_SRCS} ${libjack_RCC_SRCS})
target_link_libraries(jack ${QT_LIBRARIES} -lqmmp ${JACK_LDFLAGS} ${JACK_CFLAGS})
install(TARGETS jack DESTINATION ${LIB_DIR}/qmmp/Output)
ENDIF(JACK_FOUND)

