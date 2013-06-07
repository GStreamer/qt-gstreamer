project(qtgst-example-player)

if (NOT BUILDING_QTGSTREAMER)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules)
    find_package(Qt4or5 COMPONENTS Core Gui Widgets REQUIRED)
    if (${QT_VERSION} STREQUAL "5")
        find_package(Qt5GStreamer REQUIRED)
    else()
        find_package(QtGStreamer REQUIRED)
    endif()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

set(player_SOURCES main.cpp player.cpp mediaapp.cpp)

add_executable(player ${player_SOURCES})
target_link_libraries(player ${QTGSTREAMER_UI_LIBRARIES})
qt4or5_use_modules(player Core Gui Widgets)
