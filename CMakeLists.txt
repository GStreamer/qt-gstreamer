project(QtGStreamer)
cmake_minimum_required(VERSION 2.8)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(QTGSTREAMER_VERSION 0.10.2)

option(QTGSTREAMER_STATIC "Build QtGStreamer as a static library" OFF)
option(QTGSTREAMER_TESTS "Build QtGStreamer's tests" OFF)
option(QTGSTREAMER_EXAMPLES "Build QtGStreamer's examples" ON)
option(QTGSTREAMER_CODEGEN "Build and use QtGStreamer's codegen" OFF)
option(USE_GST_PLUGIN_DIR "Install gstreamer plugins at the system location" ON)
option(USE_QT_PLUGIN_DIR "Install qt plugins at the system location" ON)

include(MacroLogFeature)

find_package(Qt4 COMPONENTS QtCore QtGui)
macro_log_feature(QT4_FOUND "Qt 4" "Required for building everything" "http://qt.nokia.com/" TRUE "4.7")
macro_log_feature(QT_QTOPENGL_FOUND "QtOpenGL"
                                    "Required for OpenGL acceleration in qtvideosink and QtGStreamerUi"
                                    "http://qt.nokia.com/" FALSE "4.7")
macro_log_feature(QT_QTDECLARATIVE_FOUND "QtDeclarative" "Required for building QML support"
                                         "http://qt.nokia.com/" FALSE "4.7")

if (QTGSTREAMER_TESTS)
    macro_log_feature(QT_QTTEST_FOUND "QtTest" "Required for building unit tests"
                                      "http://qt.nokia.com/" FALSE "4.7")
    if (NOT QT_QTTEST_FOUND)
        set(QTGSTREAMER_TESTS OFF)
    endif()
endif()

find_package(Boost 1.39)
macro_log_feature(Boost_FOUND "Boost" "Required for building QtGLib" "http://www.boost.org/" TRUE "1.39")

# this just sets the QTGSTREAMER_* cmake variables
set(BUILDING_QTGSTREAMER TRUE)
find_package(QtGStreamer REQUIRED)

if (${CMAKE_VERSION} VERSION_LESS "2.8.6")
    find_package(Automoc4)
    macro_log_feature(Automoc4_FOUND "Automoc 4" "Required for the build system to generate moc files properly"
                                     "https://projects.kde.org/projects/kdesupport/automoc" TRUE "0.9.88")
else()
    set(Automoc4_FOUND TRUE)
    set(CMAKE_AUTOMOC TRUE)

    macro(automoc4_add_executable)
        add_executable(${ARGV})
    endmacro()

    macro(automoc4_add_library)
        add_library(${ARGV})
    endmacro()
endif()

find_package(GStreamer 0.10.33 COMPONENTS base)
macro_log_feature(GSTREAMER_FOUND "GStreamer" "Required to build QtGStreamer"
                                  "http://gstreamer.freedesktop.org/" TRUE "0.10.33")
macro_log_feature(GSTREAMER_BASE_LIBRARY_FOUND "GStreamer base library"
                                               "Used for building the qwidgetvideosink element"
                                               "http://gstreamer.freedesktop.org/" FALSE "0.10.33")

find_package(GStreamerPluginsBase 0.10.33 COMPONENTS app interfaces video)
macro_log_feature(GSTREAMER_APP_LIBRARY_FOUND "GStreamer app library"
                                              "Required to build QtGStreamerUtils"
                                              "http://gstreamer.freedesktop.org/" TRUE "0.10.33")
macro_log_feature(GSTREAMER_INTERFACES_LIBRARY_FOUND "GStreamer interfaces library"
                                                "Required to build QtGStreamer"
                                                "http://gstreamer.freedesktop.org/" TRUE "0.10.33")
macro_log_feature(GSTREAMER_VIDEO_LIBRARY_FOUND "GStreamer video library"
                                                "Used for building the qwidgetvideosink element"
                                                "http://gstreamer.freedesktop.org/" FALSE "0.10.33")

find_package(GLIB2)
macro_log_feature(GLIB2_FOUND "GLib" "Required to build QtGLib" "http://www.gtk.org/" TRUE)

find_package(GObject)
macro_log_feature(GOBJECT_FOUND "GObject" "Required to build QtGLib" "http://www.gtk.org/" TRUE)

find_package(OpenGL)
macro_log_feature(OPENGL_FOUND "OpenGL"
                               "Required for OpenGL rendering support in qtvideosink (desktop only)"
                               "http://www.opengl.org" FALSE)

find_package(OpenGLES2)
macro_log_feature(OPENGLES2_FOUND "OpenGLES"
                                  "Required for OpenGLES rendering support in qtvideosink (embedded only)"
                                  "http://www.opengl.org" FALSE "2.0")

if (QTGSTREAMER_CODEGEN AND CMAKE_CROSSCOMPILING)
    message(WARNING "Codegen use requested, but we are crosscompiling. Disabling...")
    set(QTGSTREAMER_CODEGEN OFF)
endif()

if (QTGSTREAMER_CODEGEN)
    find_package(FLEX)
    macro_log_feature(FLEX_FOUND "Flex" "Required to build codegen, a helper code generator"
                                 "http://flex.sourceforge.net/" TRUE)

    find_package(BISON)
    macro_log_feature(BISON_FOUND "Bison" "Required to build codegen, a helper code generator"
                                  "http://www.gnu.org/software/bison/" TRUE)
endif()


if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat-security -Wundef -Wpointer-arith -fno-common")
endif ()

set(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX} CACHE STRING "Destination directory for libraries")
set(INCLUDES_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include/QtGStreamer)

if (USE_GST_PLUGIN_DIR)
    set(PLUGINS_INSTALL_DIR ${GSTREAMER_PLUGIN_DIR})
else()
    set(PLUGINS_INSTALL_DIR ${LIB_INSTALL_DIR}/gstreamer-0.10)
endif()

if (USE_QT_PLUGIN_DIR)
    set(IMPORTS_INSTALL_DIR ${QT_IMPORTS_DIR})
else()
    set(IMPORTS_INSTALL_DIR ${LIB_INSTALL_DIR}/qt4/imports)
endif()

if (QTGSTREAMER_CODEGEN AND FLEX_FOUND AND BISON_FOUND)
    add_subdirectory(codegen)
endif()

if (Automoc4_FOUND) # avoid errors about missing automoc4_add_* macros
    add_subdirectory(src)
    add_subdirectory(elements)

    if (QTGSTREAMER_TESTS)
        add_subdirectory(tests)
    endif()

    if (QTGSTREAMER_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()

# Add uninstall target. Taken from the KDE4 scripts
configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/cmake_uninstall.cmake.in" "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake" @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake")

# Add doc target
include(UseDoxygen)
macro_log_feature(DOXYGEN_FOUND "Doxygen" "Used to generate the API documentation"
                                "http://www.doxygen.org/" FALSE)

macro_display_feature_log()
