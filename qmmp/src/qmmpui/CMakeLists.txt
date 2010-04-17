project(libqmmpui)

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

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# libqmmp
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

link_directories(${CMAKE_INSTALL_PREFIX}/${LIB_DIR})
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../qmmp)

SET(libqmmpui_SRCS
    general.cpp
    generalhandler.cpp
    playlistparser.cpp
    commandlinemanager.cpp
    filedialog.cpp
    qtfiledialog.cpp
    abstractplaylistitem.cpp
    fileloader.cpp
    playstate.cpp
    playlistmodel.cpp
    playlistitem.cpp
    mediaplayer.cpp
    playlistsettings.cpp
    detailsdialog.cpp
    tageditor.cpp
    playlistmanager.cpp
    metadataformatter.cpp
    templateeditor.cpp
)

SET(libqmmpui_MOC_HDRS
    generalfactory.h
    general.h
    generalhandler.h
    playlistparser.h
    playlistformat.h
    commandlinemanager.h
    commandlineoption.h
    filedialog.h
    filedialogfactory.h
    qtfiledialog.h
    abstractplaylistitem.h
    fileloader.h
    playstate.h
    playlistmodel.h
    playlistitem.h
    mediaplayer.h
    playlistsettings.h
    detailsdialog.h
    tageditor.h
    playlistmanager.h
    metadataformatter.h
    templateeditor.h
)

SET(libqmmpui_DEVEL_HDRS
    abstractplaylistitem.h
    generalfactory.h
    general.h
    generalhandler.h
    playlistformat.h
    playlistparser.h
    commandlinemanager.h
    commandlineoption.h
    filedialog.h
    filedialogfactory.h
    abstractplaylistitem.h
    playlistitem.h
    playlistmodel.h
    mediaplayer.h
    detailsdialog.h
    playlistmanager.h
    metadataformatter.h
    templateeditor.h
)


SET(libqmmpui_UIS
    detailsdialog.ui
    tageditor.ui
    templateeditor.ui
)

QT4_WRAP_UI(libqmmpui_UIS_H ${libqmmpui_UIS})

QT4_ADD_RESOURCES(libqmmpui_RCC_SRCS translations/libqmmpui_locales.qrc)

QT4_WRAP_CPP(libqmmpui_MOC_SRCS ${libqmmpui_MOC_HDRS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(qmmpui SHARED ${libqmmpui_SRCS} ${libqmmpui_MOC_SRCS} ${libqmmpui_RCC_SRCS} ${libqmmpui_UIS_H})
target_link_libraries(qmmpui ${QT_LIBRARIES} -lqmmp)
add_dependencies(qmmpui libqmmp)
SET_TARGET_PROPERTIES(qmmpui PROPERTIES VERSION ${QMMP_VERSION} SOVERSION ${QMMP_SOVERSION})
install(TARGETS qmmpui LIBRARY DESTINATION ${LIB_DIR}
                       RUNTIME DESTINATION bin
                       ARCHIVE DESTINATION ${LIB_DIR})
install(FILES ${libqmmpui_DEVEL_HDRS} DESTINATION include/qmmpui)
