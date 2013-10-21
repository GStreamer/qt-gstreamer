project(qtgst-example-qmlplayer2)

if (NOT BUILDING_QTGSTREAMER)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules)
    find_package(Qt4or5 COMPONENTS Core Gui Quick2 Qml REQUIRED)
    find_package(Qt5GStreamer REQUIRED)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
else()
    # allow the example to run from the build tree without installing QtGStreamer
    add_definitions(
        -DQTVIDEOSINK_PATH="${QtGStreamer_BINARY_DIR}/elements/gstqtvideosink"
        -DUNINSTALLED_IMPORTS_DIR="${QtGStreamer_BINARY_DIR}/src/qml/quick2"
    )
endif()

include_directories(${QTGSTREAMER_INCLUDES})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")
add_definitions(${QTGSTREAMER_DEFINITIONS})

set(qmlplayer2_SOURCES main.cpp player.cpp)
qt4or5_add_resources(qmlplayer2_rcc_SOURCES qmlplayer2.qrc)

add_executable(qmlplayer2
    ${qmlplayer2_SOURCES}
    ${qmlplayer2_rcc_SOURCES}
)
target_link_libraries(qmlplayer2 ${QTGSTREAMER_QUICK_LIBRARIES})
qt4or5_use_modules(qmlplayer2 Core Gui Quick2 Qml)
