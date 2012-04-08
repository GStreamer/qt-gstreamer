# This file serves as an example of how to use cmake with QtGStreamer.
# It can be used for building this example either in the QtGStreamer source tree or standalone.

project(qtgst-example-appsink-src)

if (NOT BUILDING_QTGSTREAMER)
    find_package(QtGStreamer REQUIRED)
endif()

include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

add_executable(appsink-src main.cpp)
target_link_libraries(appsink-src ${QTGSTREAMER_UTILS_LIBRARIES})
