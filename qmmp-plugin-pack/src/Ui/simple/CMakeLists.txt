project(libsimple)

cmake_minimum_required(VERSION 2.4.7)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 NEW)
endif(COMMAND cmake_policy)

SET(QT_USE_QTNETWORK TRUE)
INCLUDE(FindQt4)
include(${QT_USE_FILE})

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

# libqmmpui
include_directories(${QMMPUI_INCLUDE_DIRS})
link_directories( ${QMMPUI_LIBRARY_DIRS})


SET(libsimple_SRCS
    actionmanager.cpp
    listwidget.cpp
    main.cpp
    mainwindow.cpp
    popupsettings.cpp
    popupwidget.cpp
    positionslider.cpp
    renamedialog.cpp
    shortcutdialog.cpp
    shortcutitem.cpp
    simplefactory.cpp
    simplesettings.cpp
    visualmenu.cpp
    addurldialog.cpp
)

SET(libsimple_HDRS
    shortcutitem.h
)

SET(libsimple_MOC_HDRS
    actionmanager.h
    listwidget.h
    mainwindow.h
    popupsettings.h
    popupwidget.h
    positionslider.h
    renamedialog.h
    shortcutdialog.h
    simplefactory.h
    simplesettings.h
    visualmenu.h
    addurldialog.h
)

SET(libsimple_RCCS translations/translations.qrc)

QT4_ADD_RESOURCES(libsimple_RCC_SRCS ${libsimple_RCCS})

QT4_WRAP_CPP(libsimple_MOC_SRCS ${libsimple_MOC_HDRS})

# user interface


SET(libsimple_UIS
    forms/mainwindow.ui
    forms/popupsettings.ui
    forms/renamedialog.ui
    forms/shortcutdialog.ui
    forms/simplesettings.ui
    forms/addurldialog.ui
)

QT4_WRAP_UI(libsimple_UIS_H ${libsimple_UIS})
# Don't forget to include output directory, otherwise
# the UI file won't be wrapped!
include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(simple MODULE ${libsimple_SRCS} ${libsimple_MOC_SRCS} ${libsimple_UIS_H} ${libsimple_RCC_SRCS} ${libsimple_HDRS})
target_link_libraries(simple ${QT_LIBRARIES} ${QMMP_LDFLAGS} ${QMMPUI_LDFLAGS})
install(TARGETS simple DESTINATION ${LIB_DIR}/qmmp/Ui)

