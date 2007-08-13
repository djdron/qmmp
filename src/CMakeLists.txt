project(libsrc)

cmake_minimum_required(VERSION 2.4.0)

SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTNETWORK TRUE)

INCLUDE(UsePkgConfig)
INCLUDE(FindQt4)

find_package(Qt4 REQUIRED) # find and setup Qt4 for this project
include(${QT_USE_FILE})

ADD_DEFINITIONS( -Wall )
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_THREAD)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

SET(QT_INCLUDES
  ${QT_INCLUDES}
  ${CMAKE_CURRENT_BINARY_DIR}/../../../
)

# libqmmp
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../lib)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../lib)
link_directories(${CMAKE_INSTALL_PREFIX}/lib)

SET(libsrc_SRCS
  mainwindow.cpp
  mp3player.cpp
  fileloader.cpp
  button.cpp
  display.cpp
  skin.cpp
  titlebar.cpp
  positionbar.cpp
  number.cpp
  playlist.cpp
  mediafile.cpp
  listwidget.cpp
  playlistmodel.cpp
  pixmapwidget.cpp
  playlisttitlebar.cpp
  configdialog.cpp
  playlistslider.cpp
  dock.cpp
  eqwidget.cpp
  eqtitlebar.cpp
  eqslider.cpp
  togglebutton.cpp
  eqgraph.cpp
  mainvisual.cpp
  fft.c
  logscale.cpp
  textscroller.cpp
  monostereo.cpp
  playstatus.cpp
  pluginitem.cpp
  volumebar.cpp
  balancebar.cpp
  playstate.cpp
  symboldisplay.cpp
  playlistformat.cpp
  playlistcontrol.cpp
  qmmpstarter.cpp
  tcpserver.cpp
  guard.cpp
  eqpreset.cpp
  preseteditor.cpp
  jumptotrackdialog.cpp
  aboutdialog.cpp
  timeindicator.cpp
  keyboardmanager.cpp
  addurldialog.cpp
  filedialog.cpp
)

SET(libsrc_MOC_HDRS
  mainwindow.h
  fileloader.h
  button.h
  display.h
  skin.h
  titlebar.h
  positionbar.h
  number.h
  playlist.h
  mediafile.h
  listwidget.h
  playlistmodel.h
  pixmapwidget.h
  playlisttitlebar.h
  configdialog.h
  playlistslider.h
  dock.h
  eqwidget.h
  eqtitlebar.h
  eqslider.h
  togglebutton.h
  eqgraph.h
  mainvisual.h
  inlines.h
  fft.h
  logscale.h
  textscroller.h
  monostereo.h
  playstatus.h
  pluginitem.h
  volumebar.h
  balancebar.h
  playstate.h
  symboldisplay.h
  playlistformat.h
  playlistcontrol.h
  version.h
  tcpserver.h
  qmmpstarter.h
  guard.h
  eqpreset.h
  preseteditor.h
  jumptotrackdialog.h
  aboutdialog.h
  timeindicator.h
  keyboardmanager.h
  addurldialog.h
  filedialog.h
)

SET(libsrc_RCCS images/images.qrc stuff.qrc translations/qmmp_locales.qrc)

QT4_ADD_RESOURCES(libsrc_RCC_SRCS ${libsrc_RCCS})

QT4_AUTOMOC(${libsrc_MOC_SRC})
QT4_WRAP_CPP(libsrc_MOC_SRCS ${libsrc_MOC_HDRS})

# user interface


SET(libsrc_UIS
  configdialog.ui
  preseteditor.ui
  jumptotrackdialog.ui
  aboutdialog.ui
  addurldialog.ui
)

SET(src_FILES
  ../bin/qmmp
)

QT4_WRAP_UI(libsrc_UIS_H ${libsrc_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(qmmp.real ${libsrc_SRCS} ${libsrc_MOC_SRCS} ${libsrc_UIS_H}
  ${libsrc_RCC_SRCS})
target_link_libraries(qmmp.real ${QT_LIBRARIES} -lqmmp)
install(TARGETS qmmp.real DESTINATION bin PERMISSIONS WORLD_EXECUTE OWNER_READ OWNER_WRITE)
install(FILES ${src_FILES} DESTINATION bin PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ 
WORLD_EXECUTE WORLD_READ)
