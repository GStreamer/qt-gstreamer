# - Try to find QtGStreamer
# Once done this will define
#
#  QTGSTREAMER_FOUND - system has QtGStreamer
#  QTGSTREAMER_INCLUDE_DIR - the QtGStreamer include directory
#  QTGSTREAMER_INCLUDES - the include directories needed to use QtGStreamer
#  QTGLIB_LIBRARY - the QtGLib library
#  QTGLIB_LIBRARIES - the libraries needed to use QtGLib
#  QTGSTREAMER_LIBRARY - the QtGStreamer library
#  QTGSTREAMER_LIBRARIES - the libraries needed to use QtGStreamer
#  QTGSTREAMER_UI_LIBRARY - the QtGStreamerUi library
#  QTGSTREAMER_UI_LIBRARIES - the libraries needed to use QtGStreamerUi
#  QTGSTREAMER_UTILS_LIBRARY - the QtGStreamerUtils library
#  QTGSTREAMER_UTILS_LIBRARIES - the libraries needed to use QtGStreamerUtils
#  QTGSTREAMER_DEFINITIONS - definitions recommended for using QtGStreamer
#  QTGSTREAMER_FLAGS - extra compiler switches recommended for using QtGStreamer
#
# Copyright (c) 2010, George Kiagiadakis <kiagiadakis.george@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


# if variables are already in cache or we are building QtGStreamer
if((QTGSTREAMER_LIBRARY AND QTGSTREAMER_INCLUDE_DIR) OR BUILDING_QTGSTREAMER)
    set(QtGStreamer_FIND_QUIETLY TRUE)
else()
    set(QtGStreamer_FIND_QUIETLY FALSE)
endif()

set(_QTGSTREAMER_LINK_TO_QT_REQUIRED FALSE)

if(BUILDING_QTGSTREAMER)
    set(QTGLIB_LIBRARY QtGLib)
    set(QTGSTREAMER_LIBRARY QtGStreamer)
    set(QTGSTREAMER_UI_LIBRARY QtGStreamerUi)
    set(QTGSTREAMER_UTILS_LIBRARY QtGStreamerUtils)
    set(QTGSTREAMER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src)
else()
    # Attempt to find the generated QtGStreamerTargets.cmake in the same directory
    get_filename_component(_QTGSTREAMER_CONFIG_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
    find_file(_QTGSTREAMER_TARGETS_FILE QtGStreamerTargets.cmake PATHS ${_QTGSTREAMER_CONFIG_DIR} NO_DEFAULT_PATH)

    if(NOT _QTGSTREAMER_TARGETS_FILE)
        # Targets file not found. Do a typical search for QtGStreamer.
        # Normally, this path is never executed. It is just provided as a fallback in case something goes wrong.
        find_library(QTGLIB_LIBRARY QtGLib-2.0
                     PATHS "${_QTGSTREAMER_CONFIG_DIR}/../../lib")
        find_library(QTGSTREAMER_LIBRARY QtGStreamer-0.10
                     PATHS "${_QTGSTREAMER_CONFIG_DIR}/../../lib")
        find_library(QTGSTREAMER_UI_LIBRARY QtGStreamerUi-0.10
                     PATHS "${_QTGSTREAMER_CONFIG_DIR}/../../lib")
        find_library(QTGSTREAMER_UTILS_LIBRARY QtGStreamerUtils-0.10
                     PATHS "${_QTGSTREAMER_CONFIG_DIR}/../../lib")
        find_path(QTGSTREAMER_INCLUDE_DIR QGst/global.h
                  PATHS "${_QTGSTREAMER_CONFIG_DIR}/../../include"
                  PATH_SUFFIXES QtGStreamer)
        set(_QTGSTREAMER_LINK_TO_QT_REQUIRED TRUE)
        mark_as_advanced(QTGLIB_LIBRARY QTGSTREAMER_LIBRARY QTGSTREAMER_UI_LIBRARY
                         QTGSTREAMER_UTILS_LIBRARY QTGSTREAMER_INCLUDE_DIR)
    else()
        # Targets file found. Use imported QtGStreamer target and relative include path.
        # We assume that this file has been installed in $PREFIX/lib/QtGStreamer/,
        # so the include path should evaluate to $PREFIX/include/QtGStreamer
        include(${_QTGSTREAMER_TARGETS_FILE})
        set(QTGLIB_LIBRARY QtGLib)
        set(QTGSTREAMER_LIBRARY QtGStreamer)
        set(QTGSTREAMER_UI_LIBRARY QtGStreamerUi)
        set(QTGSTREAMER_UTILS_LIBRARY QtGStreamerUtils)
        get_filename_component(QTGSTREAMER_INCLUDE_DIR "${_QTGSTREAMER_CONFIG_DIR}/../../include/QtGStreamer" ABSOLUTE)
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QtGStreamer DEFAULT_MSG QTGSTREAMER_INCLUDE_DIR QTGLIB_LIBRARY
                                                          QTGSTREAMER_LIBRARY QTGSTREAMER_UI_LIBRARY
                                                          QTGSTREAMER_UTILS_LIBRARY)

if(QTGSTREAMER_FOUND)
    # Find dependencies, if not already found
    if (NOT DEFINED QT_INCLUDE_DIR)
        message(STATUS "Qt hasn't been found yet. Looking...")
        find_package(Qt4 COMPONENTS QtCore QtGui REQUIRED)
    endif()

    if (NOT DEFINED Boost_INCLUDE_DIRS)
        message(STATUS "Boost hasn't been found yet. Looking...")
        find_package(Boost REQUIRED)
    endif()

    # Set misc variables
    set(QTGSTREAMER_INCLUDES ${QTGSTREAMER_INCLUDE_DIR} ${QT_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
    set(QTGSTREAMER_DEFINITIONS "-DQT_NO_KEYWORDS")

    if (_QTGSTREAMER_LINK_TO_QT_REQUIRED)
        set(QTGLIB_LIBRARIES ${QTGLIB_LIBRARY} ${QT_QTCORE_LIBRARY})
        set(QTGSTREAMER_LIBRARIES ${QTGSTREAMER_LIBRARY} ${QTGLIB_LIBRARIES})
        set(QTGSTREAMER_UI_LIBRARIES ${QTGSTREAMER_UI_LIBRARY} ${QT_QTGUI_LIBRARY} ${QTGSTREAMER_LIBRARIES})
        set(QTGSTREAMER_UTILS_LIBRARIES ${QTGSTREAMER_UTILS_LIBRARY} ${QTGSTREAMER_LIBRARIES})
    else()
        set(QTGLIB_LIBRARIES ${QTGLIB_LIBRARY})
        set(QTGSTREAMER_LIBRARIES ${QTGSTREAMER_LIBRARY})
        set(QTGSTREAMER_UI_LIBRARIES ${QTGSTREAMER_UI_LIBRARY})
        set(QTGSTREAMER_UTILS_LIBRARIES ${QTGSTREAMER_UTILS_LIBRARY})
    endif()

    if (CMAKE_COMPILER_IS_GNUCXX)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} "-dumpversion"
                        RESULT_VARIABLE _GCC_DUMPVERSION_RESULT
                        OUTPUT_VARIABLE _GCC_VERSION
                        ERROR_QUIET
                        OUTPUT_STRIP_TRAILING_WHITESPACE)

        if ((${_GCC_DUMPVERSION_RESULT} EQUAL 0)
            AND (${_GCC_VERSION} VERSION_GREATER 4.4.99)
            AND (NOT QTGSTREAMER_DISABLE_CXX0X))

            if (NOT QTGSTREAMER_FLAGS) # be quiet if we try to find QtGStreamer multiple times
                message(STATUS "GCC 4.5 or later detected. Enabling C++0x support in QTGSTREAMER_FLAGS.")
            endif()
            set(QTGSTREAMER_FLAGS "-std=c++0x")
        endif()
    endif()

endif()
