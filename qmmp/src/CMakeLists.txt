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
  aboutdialog.cpp
  addurldialog.cpp
  balancebar.cpp
  button.cpp
  configdialog.cpp
  display.cpp
  dock.cpp
  eqgraph.cpp
  eqpreset.cpp
  eqslider.cpp
  eqtitlebar.cpp
  eqwidget.cpp
  filedialog.cpp
  fileloader.cpp
  jumptotrackdialog.cpp
  keyboardmanager.cpp
  listwidget.cpp
  logscale.cpp
  mainvisual.cpp
  mainwindow.cpp
  mediafile.cpp
  monostereo.cpp
  mp3player.cpp
  number.cpp
  pixmapwidget.cpp
  playlistcontrol.cpp
  playlist.cpp
  playlistformat.cpp
  playlistmodel.cpp
  playlistslider.cpp
  playlisttitlebar.cpp
  playstate.cpp
  playstatus.cpp
  pluginitem.cpp
  positionbar.cpp
  preseteditor.cpp
  qmmpstarter.cpp
  skin.cpp
  skinreader.cpp
  symboldisplay.cpp
  textscroller.cpp
  timeindicator.cpp
  titlebar.cpp
  togglebutton.cpp
  unixdomainsocket.cpp
  visualmenu.cpp
  volumebar.cpp
  fft.c
  shadedvisual.cpp
  titlebarcontrol.cpp
  shadedbar.cpp
)

SET(libsrc_MOC_HDRS
  aboutdialog.h
  addurldialog.h
  balancebar.h
  button.h
  configdialog.h
  display.h
  dock.h
  eqgraph.h
  eqpreset.h
  eqslider.h
  eqtitlebar.h
  eqwidget.h
  fft.h
  filedialog.h
  fileloader.h
  inlines.h
  jumptotrackdialog.h
  keyboardmanager.h
  listwidget.h
  logscale.h
  mainvisual.h
  mainwindow.h
  mediafile.h
  monostereo.h
  number.h
  pixmapwidget.h
  playlistcontrol.h
  playlistformat.h
  playlist.h
  playlistmodel.h
  playlistslider.h
  playlisttitlebar.h
  playstate.h
  playstatus.h
  pluginitem.h
  positionbar.h
  preseteditor.h
  qmmpstarter.h
  skin.h
  skinreader.h
  symboldisplay.h
  textscroller.h
  timeindicator.h
  titlebar.h
  togglebutton.h
  unixdomainsocket.h
  version.h
  visualmenu.h
  volumebar.h
  shadedvisual.h
  titlebarcontrol.h
  shadedbar.h
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


QT4_WRAP_UI(libsrc_UIS_H ${libsrc_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(qmmp ${libsrc_SRCS} ${libsrc_MOC_SRCS} ${libsrc_UIS_H}
  ${libsrc_RCC_SRCS})
target_link_libraries(qmmp ${QT_LIBRARIES} -lqmmp)
install(TARGETS qmmp DESTINATION bin PERMISSIONS PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE  GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
