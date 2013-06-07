# This file serves as an example of how to use cmake with QtGStreamer.
# It can be used for building this example either in the QtGStreamer source tree or standalone.

project(qtgst-example-appsink-src)

if (NOT BUILDING_QTGSTREAMER)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/modules)
    find_package(Qt4or5 COMPONENTS Core REQUIRED)
    if (${QT_VERSION} STREQUAL "5")
        find_package(Qt5GStreamer REQUIRED)
    else()
        find_package(QtGStreamer REQUIRED)
    endif()
endif()

include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

add_executable(appsink-src main.cpp)
target_link_libraries(appsink-src ${QTGSTREAMER_UTILS_LIBRARIES})
qt4or5_use_modules(appsink-src Core)
