# This file serves as an example of how to use cmake with QtGStreamer.
# It can be used for building this example either in the QtGStreamer source tree or standalone.

find_package(QtGStreamer REQUIRED)
include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")
set(player_SOURCES main.cpp player.cpp mediaapp.cpp)
automoc4_add_executable(player ${player_SOURCES})
target_link_libraries(player ${QTGSTREAMER_UI_LIBRARIES})
