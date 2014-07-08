project(QtGStreamer)
cmake_minimum_required(VERSION 2.8.9)
enable_testing()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set(QTGSTREAMER_VERSION 1.2.0)

option(QTGSTREAMER_STATIC "Build QtGStreamer as a static library" OFF)
option(QTGSTREAMER_TESTS "Build QtGStreamer's tests" OFF)
option(QTGSTREAMER_EXAMPLES "Build QtGStreamer's examples" ON)
option(QTGSTREAMER_CODEGEN "Build and use QtGStreamer's codegen" OFF)
option(USE_GST_PLUGIN_DIR "Install gstreamer plugins at the system location" ON)
option(USE_QT_PLUGIN_DIR "Install qt plugins at the system location" ON)

include(GNUInstallDirs)
include(MacroLogFeature)

set(Qt4_MIN_VERSION 4.7)
set(Qt5_MIN_VERSION 5.0.0)
find_package(Qt4or5 COMPONENTS Core Gui Widgets OPTIONAL_COMPONENTS OpenGL Quick1 Quick2 Qml Test)
macro_log_feature(Qt4or5_FOUND "Qt" "Required for building everything"
                               "http://qt-project.org/" TRUE "${Qt4or5_MIN_VERSION}")
macro_log_feature(Qt4or5_OpenGL_FOUND "QtOpenGL"
                                "Required for OpenGL acceleration in qtvideosink and QtGStreamerUi"
                                "http://qt-project.org/" FALSE "${Qt4or5_MIN_VERSION}")
macro_log_feature(Qt4or5_Quick1_FOUND "QtQuick1 (QtDeclarative)"
                                "Required for building QtQuick1 support"
                                "http://qt-project.org/" FALSE "${Qt4or5_MIN_VERSION}")
if (${QT_VERSION} STREQUAL "5")
    macro_log_feature(Qt4or5_Quick2_FOUND "QtQuick2 (QtQuick)"
                                "Required for building QtQuick2 support"
                                "http://qt-project.org/" TRUE "${Qt4or5_MIN_VERSION}")
    macro_log_feature(Qt4or5_Qml_FOUND "QtQml"
                                "Required for building QtQuick2 support"
                                "http://qt-project.org/" TRUE "${Qt4or5_MIN_VERSION}")
endif()

if (QTGSTREAMER_TESTS)
    macro_log_feature(Qt4or5_Test_FOUND "QtTest" "Required for building unit tests"
                                      "http://qt-project.org/" FALSE "${Qt4or5_MIN_VERSION}")
    if (NOT Qt4or5_Test_FOUND)
        set(QTGSTREAMER_TESTS OFF)
    endif()
endif()

find_package(Boost 1.39)
macro_log_feature(Boost_FOUND "Boost" "Required for building QtGLib" "http://www.boost.org/" TRUE "1.39")

# set the QTGSTREAMER_* cmake variables
set(BUILDING_QTGSTREAMER TRUE)
if (${QT_VERSION} STREQUAL "5")
    set(USE_QT5 TRUE)
    set(QTGLIB_LIBRARY Qt5GLib)
    set(QTGSTREAMER_LIBRARY Qt5GStreamer)
    set(QTGSTREAMER_QUICK_LIBRARY Qt5GStreamerQuick)
    set(QTGSTREAMER_UI_LIBRARY Qt5GStreamerUi)
    set(QTGSTREAMER_UTILS_LIBRARY Qt5GStreamerUtils)
    set(QTGSTREAMER_PACKAGE_NAME Qt5GStreamer)
    set(QTVIDEOSINK_NAME qt5videosink)
    set(QTGLVIDEOSINK_NAME qt5glvideosink)
    set(QWIDGETVIDEOSINK_NAME qwidget5videosink)
elseif (${QT_VERSION} STREQUAL "4")
    set(USE_QT4 TRUE)
    set(QTGLIB_LIBRARY QtGLib)
    set(QTGSTREAMER_LIBRARY QtGStreamer)
    set(QTGSTREAMER_QUICK_LIBRARY "")
    set(QTGSTREAMER_UI_LIBRARY QtGStreamerUi)
    set(QTGSTREAMER_UTILS_LIBRARY QtGStreamerUtils)
    set(QTGSTREAMER_PACKAGE_NAME QtGStreamer)
    set(QTVIDEOSINK_NAME qtvideosink)
    set(QTGLVIDEOSINK_NAME qtglvideosink)
    set(QWIDGETVIDEOSINK_NAME qwidgetvideosink)
endif()
set(QTGSTREAMER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src)
include(QtGStreamerConfigCommon)

find_package(GStreamer 1.0.0 COMPONENTS base)
macro_log_feature(GSTREAMER_FOUND "GStreamer" "Required to build QtGStreamer"
                                  "http://gstreamer.freedesktop.org/" TRUE "1.0.0")
macro_log_feature(GSTREAMER_BASE_LIBRARY_FOUND "GStreamer base library"
                                               "Used for building the ${QTVIDEOSINK_NAME} element"
                                               "http://gstreamer.freedesktop.org/" FALSE "1.0.0")

find_package(GStreamerPluginsBase 1.0.0 COMPONENTS app audio video pbutils)
macro_log_feature(GSTREAMER_APP_LIBRARY_FOUND "GStreamer app library"
                                              "Required to build QtGStreamerUtils"
                                              "http://gstreamer.freedesktop.org/" TRUE "1.0.0")
macro_log_feature(GSTREAMER_AUDIO_LIBRARY_FOUND "GStreamer audio library"
                                                "Required to build QtGStreamer"
                                                "http://gstreamer.freedesktop.org/" TRUE "1.0.0")
macro_log_feature(GSTREAMER_VIDEO_LIBRARY_FOUND "GStreamer video library"
                                                "Required to build QtGStreamer"
                                                "http://gstreamer.freedesktop.org/" TRUE "1.0.0")
macro_log_feature(GSTREAMER_PBUTILS_LIBRARY_FOUND "GStreamer pbutils library"
                                                "Used for building the Discoverer API"
                                                "http://gstreamer.freedesktop.org/" TRUE "1.0.0")

find_package(GLIB2)
macro_log_feature(GLIB2_FOUND "GLib" "Required to build QtGLib" "http://www.gtk.org/" TRUE)

find_package(GObject)
macro_log_feature(GOBJECT_FOUND "GObject" "Required to build QtGLib" "http://www.gtk.org/" TRUE)

set(CMAKE_REQUIRED_INCLUDES ${QTGSTREAMER_INCLUDES})
include(CheckCXXSourceCompiles)
check_cxx_source_compiles("
#include <QtCore/QtGlobal>
#if !defined(QT_OPENGL_ES)
#error \"No OpenGLES\"
#endif
int main() {}
" USE_OPENGLES)

if (USE_OPENGLES)
    find_package(OpenGLES2)
    macro_log_feature(OPENGLES2_FOUND "OpenGLES"
                          "Required for OpenGLES rendering support in ${QTVIDEOSINK_NAME}"
                          "http://www.opengl.org" FALSE "2.0")
else()
    find_package(OpenGL)
    macro_log_feature(OPENGL_FOUND "OpenGL"
                          "Required for OpenGL rendering support in ${QTVIDEOSINK_NAME}"
                          "http://www.opengl.org" FALSE)
endif()

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


set(QTGSTREAMER_INSTALL_TARGET_DEFAULT_ARGS
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
set(QTGSTREAMER_PC_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
set(QTGSTREAMER_CMAKE_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${QTGSTREAMER_PACKAGE_NAME})
set(QTGSTREAMER_INCLUDES_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${QTGSTREAMER_PACKAGE_NAME})

if (USE_GST_PLUGIN_DIR)
    set(QTGSTREAMER_GST_PLUGINS_INSTALL_DIR ${GSTREAMER_PLUGIN_DIR})
else()
    set(QTGSTREAMER_GST_PLUGINS_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/gstreamer-${GSTREAMER_ABI_VERSION})
endif()

if (USE_QT_PLUGIN_DIR)
    set(QTGSTREAMER_QTQUICK1_INSTALL_DIR ${QT_IMPORTS_DIR})
    set(QTGSTREAMER_QTQUICK2_INSTALL_DIR ${QT_QML_DIR})
else()
    if (USE_QT5)
        set(QTGSTREAMER_QTQUICK1_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/imports)
        set(QTGSTREAMER_QTQUICK2_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/qt5/qml)
    else()
        set(QTGSTREAMER_QTQUICK1_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/qt4/imports)
    endif()
endif()


if (QTGSTREAMER_CODEGEN AND FLEX_FOUND AND BISON_FOUND)
    add_subdirectory(codegen)
endif()

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
