# - Try to find an installed version of Qt based on QT_VERSION
# Once done this will define
#
#  USE_QT4 - Building with Qt4
#  USE_QT5 - Building with Qt5
#  QT_MIN_VERSION - qt minimum
#
# Copyright (c) 2013, Fluendo S.L. <support@fluendo.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (${QT_VERSION} STREQUAL "5")
    set(USE_QT5 TRUE)
    set(QT_MIN_VERSION 5.0.1)
    message(STATUS "Using Qt5 (min: ${QT_MIN_VERSION})")
elseif (${QT_VERSION} STREQUAL "4")
    set(USE_QT4 TRUE)
    set(QT_MIN_VERSION 4.7)
    message(STATUS "Using Qt4 (min:${QT_MIN_VERSION})")
endif()

if (USE_QT5)
    find_package(Qt5Core)
    find_package(Qt5Gui)
    find_package(Qt5Widgets)
    if (Qt5Core_FOUND AND Qt5Gui_FOUND AND Qt5Widgets_FOUND)
        set(QT_FOUND true)
        set(QT_QTCORE_FOUND true)
        set(QT_QTGUI_FOUND true)
        set(QT_QTCORE_LIBRARY ${Qt5Core_LIBRARIES})
        set(QT_QTGUI_LIBRARY ${Qt5Widgets_LIBRARIES})
        set(QT_INCLUDE_DIR ${Qt5Widgets_INCLUDE_DIRS})

        function(_QT5_QUERY_QMAKE VAR RESULT)
          get_target_property(_QMAKE ${Qt5Core_QMAKE_EXECUTABLE} IMPORTED_LOCATION)
          execute_process(COMMAND "${_QMAKE}" -query ${VAR}
            RESULT_VARIABLE return_code
            OUTPUT_VARIABLE output
            OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_STRIP_TRAILING_WHITESPACE)
          if(NOT return_code)
            file(TO_CMAKE_PATH "${output}" output)
            set(${RESULT} ${output} PARENT_SCOPE)
          endif()
        endfunction()

        _qt5_query_qmake(QT_INSTALL_IMPORTS QT_IMPORTS_DIR)
    endif()
    find_package(Qt5Declarative)
    if (Qt5Declarative_FOUND)
        set(QT_QTDECLARATIVE_FOUND true)
        set(QT_QTDECLARATIVE_LIBRARY ${Qt5Declarative_LIBRARIES})
        set(QT_QTDECLARATIVE_INCLUDE_DIR ${Qt5Declarative_INCLUDE_DIRS})
    endif()
    find_package(Qt5Test)
    if (Qt5Test_FOUND)
        set(QT_QTTEST_FOUND true)
        set(QT_QTTEST_LIBRARY ${Qt5Test_LIBRARIES})
        set(QT_QTTEST_INCLUDE_DIR ${Qt5Test_INCLUDE_DIRS})
    endif ()
    find_package(Qt5OpenGL)
    if (Qt5OpenGL_FOUND)
        set(QT_QTOPENGL_FOUND true)
        set(QT_QTOPENGL_LIBRARY ${Qt5OpenGL_LIBRARIES})
        set(QT_QTOPENGL_INCLUDE_DIR ${Qt5OpenGL_INCLUDE_DIRS})
    endif()
    set(PKG_CONFIG_QTCORE_DEP Qt5Core)
    set(PKG_CONFIG_QTGUI_DEP Qt5Widgets)
elseif (USE_QT4)
    find_package(Qt4 COMPONENTS QtCore QtGui)
    set(PKG_CONFIG_QTCORE_DEP QtCore)
    set(PKG_CONFIG_QTGUI_DEP QtGui)
else()
    message(FATAL_ERROR "Qt supported versions are: 4 and 5")
endif()

# qt_helper_wrap_ui(outfiles inputfile ... )
macro(QT_HELPER_WRAP_UI)
    if (USE_QT5)
        qt5_wrap_ui(${ARGV})
    elseif (USE_QT4)
        qt4_wrap_ui(${ARGV})
    endif()
endmacro()

# qt_helper_add_resources(outfiles inputfile ... )
macro(QT_HELPER_ADD_RESOURCES)
    if (USE_QT5)
        qt5_add_resources(${ARGV})
    elseif (USE_QT4)
        qt4_add_resources(${ARGV})
    endif()
endmacro()
