project(QtGstreamer)
cmake_minimum_required(VERSION 2.6)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(BUILDING_QTGSTREAMER TRUE)
set(QTGSTREAMER_VERSION 0.1)
set(QTGSTREAMER_SOVERSION 0)

option(QTGSTREAMER_STATIC "Build QtGStreamer as a static library" ON)
option(QTGSTREAMER_TESTS "Build QtGStreamer's tests" OFF)
option(QTGSTREAMER_EXAMPLES "Build QtGStreamer's examples" ON)

find_package(Qt4 COMPONENTS QtCore QtGui QtTest REQUIRED)
find_package(QtGstreamer REQUIRED)
find_package(Automoc4 REQUIRED)
find_package(GStreamer 0.10.30 REQUIRED)
find_package(GStreamerPluginsBase 0.10.30 COMPONENTS interfaces video REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(GObject REQUIRED)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat-security -Wundef -Wpointer-arith -Wcast-align -fno-common")
endif ()

add_subdirectory(codegen)
add_subdirectory(src)
add_subdirectory(elements)

if (QTGSTREAMER_TESTS)
    add_subdirectory(tests)
endif()

if (QTGSTREAMER_EXAMPLES)
    add_subdirectory(examples)
endif()

# Add uninstall target. Taken from the KDE4 scripts
configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/cmake_uninstall.cmake.in" "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake" @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")

# Add doc target
include(UseDoxygen)
