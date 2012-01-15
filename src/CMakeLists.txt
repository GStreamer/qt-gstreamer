set(EXPORT_TARGET_SET QtGStreamerTargets)

if (QTGSTREAMER_STATIC)
    set(SHARED_OR_STATIC "STATIC")
else()
    set(SHARED_OR_STATIC "SHARED")
    if (CMAKE_COMPILER_IS_GNUCXX AND NOT WIN32)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden")
    endif ()
endif()

# Macro to run codegen from the subdirs
macro(run_codegen _dir_name _includes _headers)
    if (QTGSTREAMER_CODEGEN)
        set(_prefixed_headers "")
        foreach(_header ${_headers})
            list(APPEND _prefixed_headers ${_dir_name}/${_header})
        endforeach()

        add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/gen.cpp
                           COMMAND codegen
                           ARGS ${_includes} ${_prefixed_headers}
                                > ${CMAKE_CURRENT_SOURCE_DIR}/gen.cpp
                           DEPENDS codegen ${_headers}
                           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
    endif()

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gen.cpp
                       COMMAND ${CMAKE_COMMAND}
                       ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/gen.cpp
                            ${CMAKE_CURRENT_BINARY_DIR}/gen.cpp
                       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen.cpp
                       COMMENT "Copying gen.cpp to the build directory")
endmacro()

# Macro to install headers from the subdirs
macro(install_headers _dir_name)
    foreach(header ${ARGN})
        get_filename_component(header_path ${header} PATH)
        install(FILES ${header} DESTINATION ${INCLUDES_INSTALL_DIR}/${_dir_name}/${header_path})
    endforeach()
endmacro()

# Setup common environment
include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

# Build the libraries
add_subdirectory(QGlib)
add_subdirectory(QGst)

# Build the qml plugin
if (QT_QTDECLARATIVE_FOUND)
    add_subdirectory(qml)
endif()

# Install the cmake scripts that are used to find the installed library from external projects
install(EXPORT ${EXPORT_TARGET_SET} DESTINATION ${LIB_INSTALL_DIR}/QtGStreamer)
install(FILES ${CMAKE_SOURCE_DIR}/cmake/modules/FindQtGStreamer.cmake
        DESTINATION ${LIB_INSTALL_DIR}/QtGStreamer
        RENAME QtGStreamerConfig.cmake)

# Install pkgconfig files
file(GLOB_RECURSE PC_IN_FILES "*.pc.in")
foreach(pc_in_file ${PC_IN_FILES})
    get_filename_component(pc_out_file ${pc_in_file} NAME)
    string(REPLACE ".pc.in" ".pc" pc_out_file ${pc_out_file})
    configure_file(${pc_in_file} ${CMAKE_CURRENT_BINARY_DIR}/${pc_out_file} @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${pc_out_file} DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
endforeach()
