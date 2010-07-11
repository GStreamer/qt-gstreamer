project(QtGstreamer)
cmake_minimum_required(VERSION 2.6)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(BUILDING_QTGSTREAMER TRUE)

find_package(Qt4 COMPONENTS QtCore QtTest REQUIRED)
find_package(QtGstreamer REQUIRED)
find_package(Automoc4 REQUIRED)
find_package(GStreamer REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(GObject REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

include(${QT_USE_FILE})

if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat-security -Wundef -Wpointer-arith -Wcast-align -fno-common")
endif ()

add_subdirectory(codegen)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)

# Add uninstall target. Taken from the KDE4 scripts
configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/cmake_uninstall.cmake.in" "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake" @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")
