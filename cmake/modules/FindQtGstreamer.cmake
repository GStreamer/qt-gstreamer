# - Try to find QtGstreamer
# Once done this will define
#
#  QTGSTREAMER_FOUND - system has QtGstreamer
#  QTGSTREAMER_INCLUDE_DIR - the QtGstreamer include directory
#  QTGSTREAMER_INCLUDES - the include directories needed to use QtGstreamer
#  QTGSTREAMER_LIBRARY - the QtGstreamer library
#  QTGSTREAMER_LIBRARIES - the libraries needed to use QtGstreamer
#  QTGSTREAMER_DEFINITIONS - definitions recommended for using QtGstreamer
#  QTGSTREAMER_FLAGS - extra compiler switches recommended for using QtGstreamer
#
# Copyright (c) 2010, George Kiagiadakis <kiagiadakis.george@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


# if variables are already in cache or we are building QtGstreamer
if((QTGSTREAMER_LIBRARY AND QTGSTREAMER_INCLUDE_DIR) OR BUILDING_QTGSTREAMER)
    set(QtGstreamer_FIND_QUIETLY TRUE)
else()
    set(QtGstreamer_FIND_QUIETLY FALSE)
endif()

set(_QTGSTREAMER_LINK_TO_QT_REQUIRED FALSE)

if(BUILDING_QTGSTREAMER)
    set(QTGSTREAMER_LIBRARY QtGstreamer)
    set(QTGSTREAMER_UI_LIBRARY QtGstreamerUi)
    set(QTGSTREAMER_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src)
else()
    # Attempt to find the generated QtGstreamerTargets.cmake in the same directory
    get_filename_component(_QTGSTREAMER_CONFIG_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
    find_file(_QTGSTREAMER_TARGETS_FILE QtGstreamerTargets.cmake PATHS ${_QTGSTREAMER_CONFIG_DIR} NO_DEFAULT_PATH)

    if(_QTGSTREAMER_TARGETS_FILE-NOTFOUND)
        # Targets file not found. Do a typical search for QtGstreamer.
        # Normally, this path is never executed. It is just provided as a fallback in case something goes wrong.
        find_library(QTGSTREAMER_LIBRARY QtGstreamer
                     PATHS "${_QTGSTREAMER_CONFIG_DIR}/../../lib")
        find_library(QTGSTREAMER_UI_LIBRARY QtGstreamerUi
                     PATHS "${_QTGSTREAMER_CONFIG_DIR}/../../lib")
        find_path(QTGSTREAMER_INCLUDE_DIR QGst/global.h
                  PATHS "${_QTGSTREAMER_CONFIG_DIR}/../../include"
                  PATH_SUFFIXES QtGstreamer)
        set(_QTGSTREAMER_LINK_TO_QT_REQUIRED TRUE)
    else()
        # Targets file found. Use imported QtGstreamer target and relative include path.
        # We assume that this file has been installed in $PREFIX/lib/QtGstreamer/,
        # so the include path should evaluate to $PREFIX/include/QtGstreamer
        include(${_QTGSTREAMER_TARGETS_FILE})
        set(QTGSTREAMER_LIBRARY QtGstreamer)
        set(QTGSTREAMER_UI_LIBRARY QtGstreamerUi)
        get_filename_component(QTGSTREAMER_INCLUDE_DIR "${_QTGSTREAMER_CONFIG_DIR}/../../include/QtGstreamer" ABSOLUTE)
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(QtGstreamer DEFAULT_MSG QTGSTREAMER_INCLUDE_DIR
                                                          QTGSTREAMER_LIBRARY QTGSTREAMER_UI_LIBRARY)

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
        set(QTGSTREAMER_LIBRARIES ${QTGSTREAMER_LIBRARY} ${QT_QTCORE_LIBRARY})
        set(QTGSTREAMER_UI_LIBRARIES ${QTGSTREAMER_UI_LIBRARY} ${QTGSTREAMER_LIBRARY}
                                    ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
    else()
        set(QTGSTREAMER_LIBRARIES ${QTGSTREAMER_LIBRARY})
        set(QTGSTREAMER_UI_LIBRARIES ${QTGSTREAMER_UI_LIBRARY})
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

mark_as_advanced(QTGSTREAMER_LIBRARY QTGSTREAMER_INCLUDE_DIR)
