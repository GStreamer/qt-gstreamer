project(qtgst-example-qmlplayer)

if (NOT BUILDING_QTGSTREAMER)
    find_package(Qt4 COMPONENTS QtCore QtGui QtDeclarative REQUIRED)
    find_package(QtGStreamer REQUIRED)

    if (${CMAKE_VERSION} VERSION_LESS "2.8.6")
        find_package(Automoc4 REQUIRED)
    else()
        set(CMAKE_AUTOMOC TRUE)

        macro(automoc4_add_executable)
            add_executable(${ARGV})
        endmacro()
    endif()
else()
    # allow the example to run from the build tree without installing QtGStreamer
    add_definitions(
        -DQTVIDEOSINK_PATH="${QtGStreamer_BINARY_DIR}/elements/gstqtvideosink"
        -DUNINSTALLED_IMPORTS_DIR="${QtGStreamer_BINARY_DIR}/src/qml"
    )
endif()

include_directories(${QTGSTREAMER_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")
add_definitions(${QTGSTREAMER_DEFINITIONS})

if (QT_QTOPENGL_FOUND)
    set(qmlplayer_gl_libs ${QT_QTOPENGL_LIBRARY})
else()
    add_definitions(-DQMLPLAYER_NO_OPENGL)
endif()

set(qmlplayer_SOURCES main.cpp player.cpp)
qt4_add_resources(qmlplayer_rcc_SOURCES qmlplayer.qrc)

automoc4_add_executable(qmlplayer
    ${qmlplayer_SOURCES}
    ${qmlplayer_rcc_SOURCES}
)
target_link_libraries(qmlplayer
    ${QTGSTREAMER_UI_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${qmlplayer_gl_libs}
)
