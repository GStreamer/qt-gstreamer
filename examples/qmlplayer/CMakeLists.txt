project(qtgst-example-qmlplayer)

if (NOT BUILDING_QTGSTREAMER)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules)
    find_package(Qt4or5 COMPONENTS Core Gui Widgets Quick1 REQUIRED)
    if (${QT_VERSION} STREQUAL "5")
        find_package(Qt5GStreamer REQUIRED)
    else()
        find_package(QtGStreamer REQUIRED)
    endif()

    find_package(OpenGL)
    find_package(OpenGLES2)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
else()
    # allow the example to run from the build tree without installing QtGStreamer
    add_definitions(
        -DQTVIDEOSINK_PATH="${QtGStreamer_BINARY_DIR}/elements/gstqtvideosink"
        -DUNINSTALLED_IMPORTS_DIR="${QtGStreamer_BINARY_DIR}/src/qml/quick1"
    )
endif()

include_directories(${QTGSTREAMER_INCLUDES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")
add_definitions(${QTGSTREAMER_DEFINITIONS})

if (Qt4or5_OpenGL_FOUND AND (OPENGL_FOUND OR OPENGLES2_FOUND))
    if (OPENGLES2_FOUND)
        include_directories(${OPENGLES2_INCLUDE_DIR})
    else()
        include_directories(${OPENGL_INCLUDE_DIR})
    endif()
else()
    add_definitions(-DQMLPLAYER_NO_OPENGL)
endif()

set(qmlplayer_SOURCES main.cpp player.cpp)
qt4or5_add_resources(qmlplayer_rcc_SOURCES qmlplayer.qrc)

add_executable(qmlplayer
    ${qmlplayer_SOURCES}
    ${qmlplayer_rcc_SOURCES}
)
target_link_libraries(qmlplayer ${QTGSTREAMER_UI_LIBRARIES})
qt4or5_use_modules(qmlplayer Core Gui Widgets Quick1)
if (Qt4or5_OpenGL_FOUND AND (OPENGL_FOUND OR OPENGLES2_FOUND))
    qt4or5_use_modules(qmlplayer OpenGL)
endif()
