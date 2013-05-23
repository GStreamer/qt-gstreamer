project(qtgst-example-player)

if (NOT BUILDING_QTGSTREAMER)
    find_package(QtGStreamer REQUIRED)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

set(player_SOURCES main.cpp player.cpp mediaapp.cpp)

add_executable(player ${player_SOURCES})
target_link_libraries(player ${QTGSTREAMER_UI_LIBRARIES})
