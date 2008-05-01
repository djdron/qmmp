project(libalsa)

cmake_minimum_required(VERSION 2.4.0)


INCLUDE(UsePkgConfig)

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

# alsa
PKGCONFIG(alsa ALSA_INCLUDE_DIR ALSA_LINK_DIR ALSA_LINK_FLAGS ALSA_CFLAGS)

IF(NOT ALSA_LINK_FLAGS)
  SET(ALSA_LINK_FLAGS -lasound)
ENDIF(NOT ALSA_LINK_FLAGS)

SET(libalsa_SRCS
  outputalsa.cpp
  outputalsafactory.cpp
  settingsdialog.cpp
)

SET(libalsa_MOC_HDRS
  outputalsa.h
  outputalsafactory.h
  settingsdialog.h
)

SET(libalsa_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libalsa_RCC_SRCS ${libalsa_RCCS})

QT4_WRAP_CPP(libalsa_MOC_SRCS ${libalsa_MOC_HDRS})

# user interface


SET(libalsa_UIS
    settingsdialog.ui
)

QT4_WRAP_UI(libalsa_UIS_H ${libalsa_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(alsa SHARED ${libalsa_SRCS} ${libalsa_MOC_SRCS} ${libalsa_UIS_H}
  ${libalsa_RCC_SRCS})
target_link_libraries(alsa ${QT_LIBRARIES} -lqmmp ${ALSA_LINK_FLAGS})
install(TARGETS alsa DESTINATION ${LIB_DIR}/qmmp/Output PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE  GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

