project(QtGStreamer)
cmake_minimum_required(VERSION 2.8)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(QTGSTREAMER_VERSION 0.10.1)

option(QTGSTREAMER_STATIC "Build QtGStreamer as a static library" OFF)
option(QTGSTREAMER_TESTS "Build QtGStreamer's tests" OFF)
option(QTGSTREAMER_EXAMPLES "Build QtGStreamer's examples" ON)

include(MacroLogFeature)

find_package(Qt4 COMPONENTS QtCore QtGui QtTest)
macro_log_feature(QT4_FOUND "Qt 4" "Required for building everything" "http://qt.nokia.com/" TRUE "4.5")

find_package(Boost 1.39)
macro_log_feature(Boost_FOUND "Boost" "Required for building QtGLib" "http://www.boost.org/" TRUE "1.39")

# this just sets the QTGSTREAMER_* cmake variables
set(BUILDING_QTGSTREAMER TRUE)
find_package(QtGStreamer REQUIRED)

find_package(Automoc4)
macro_log_feature(Automoc4_FOUND "Automoc 4" "Required for the build system to generate moc files properly"
                                 "https://projects.kde.org/projects/kdesupport/automoc" TRUE "0.9.88")

find_package(GStreamer 0.10.31 COMPONENTS base)
macro_log_feature(GSTREAMER_FOUND "GStreamer" "Required to build QtGStreamer"
                                  "http://gstreamer.freedesktop.org/" TRUE "0.10.31")
macro_log_feature(GSTREAMER_BASE_LIBRARY_FOUND "GStreamer base library"
                                               "Used for building the qwidgetvideosink element"
                                               "http://gstreamer.freedesktop.org/" FALSE "0.10.31")

find_package(GStreamerPluginsBase 0.10.31 COMPONENTS app interfaces video)
macro_log_feature(GSTREAMER_APP_LIBRARY_FOUND "GStreamer app library"
                                              "Required to build QtGStreamerUtils"
                                              "http://gstreamer.freedesktop.org/" TRUE "0.10.31")
macro_log_feature(GSTREAMER_INTERFACES_LIBRARY_FOUND "GStreamer interfaces library"
                                                "Required to build QtGStreamer"
                                                "http://gstreamer.freedesktop.org/" TRUE "0.10.31")
macro_log_feature(GSTREAMER_VIDEO_LIBRARY_FOUND "GStreamer video library"
                                                "Used for building the qwidgetvideosink element"
                                                "http://gstreamer.freedesktop.org/" FALSE "0.10.31")

find_package(GLIB2)
macro_log_feature(GLIB2_FOUND "GLib" "Required to build QtGLib" "http://www.gtk.org/" TRUE)

find_package(GObject)
macro_log_feature(GOBJECT_FOUND "GObject" "Required to build QtGLib" "http://www.gtk.org/" TRUE)

find_package(FLEX)
macro_log_feature(FLEX_FOUND "Flex" "Required to build codegen, a helper code generator"
                             "http://flex.sourceforge.net/" TRUE)

find_package(BISON)
macro_log_feature(BISON_FOUND "Bison" "Required to build codegen, a helper code generator"
                              "http://www.gnu.org/software/bison/" TRUE)


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
macro_log_feature(DOXYGEN_FOUND "Doxygen" "Used to generate the API documentation"
                                "http://www.doxygen.org/" FALSE)

macro_display_feature_log()
