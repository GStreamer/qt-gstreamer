project(qtgst-example-player)

if (NOT BUILDING_QTGSTREAMER)
    find_package(QtGStreamer REQUIRED)

    if (${CMAKE_VERSION} VERSION_LESS "2.8.6")
        find_package(Automoc4 REQUIRED)
    else()
        set(CMAKE_AUTOMOC TRUE)

        macro(automoc4_add_executable)
            add_executable(${ARGV})
        endmacro()
    endif()
endif()

include_directories(${QTGSTREAMER_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

set(player_SOURCES main.cpp player.cpp mediaapp.cpp)

automoc4_add_executable(player ${player_SOURCES})
target_link_libraries(player ${QTGSTREAMER_UI_LIBRARIES})
