set(QtGLib_SRCS
    quark.cpp
    type.cpp
    paramspec.cpp
    object.cpp
    value.cpp
    signal.cpp
    error.cpp
    connect.cpp
    wrap.cpp
    init.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/gen.cpp
)

set(QtGLib_INSTALLED_HEADERS
    global.h      Global
    init.h        Init
    quark.h       Quark
    type.h        Type
    refpointer.h  RefPointer
    wrap.h
    paramspec.h   ParamSpec
    object.h      Object
    value.h       Value
    signal.h      Signal
    emitimpl.h
    connect.h     Connect
    connectimpl.h
    error.h       Error
)

file(GLOB QtGLib_CODEGEN_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
list(REMOVE_ITEM QtGLib_CODEGEN_HEADERS "connectimpl.h" "emitimpl.h")

set(QtGLib_CODEGEN_INCLUDES
    -Iglib-object.h
)

# Setup the environment
set(QTGLIB_API_VERSION 2.0)
set(QTGLIB_SOVERSION 0)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${GOBJECT_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR})

# Add command to generate gen.cpp using codegen
run_codegen("QGlib" "${QtGLib_CODEGEN_INCLUDES}" "${QtGLib_CODEGEN_HEADERS}")

# Build and link QtGLib
automoc4_add_library(QtGLib ${SHARED_OR_STATIC} ${QtGLib_SRCS})
set_target_properties(QtGLib PROPERTIES OUTPUT_NAME QtGLib-${QTGLIB_API_VERSION}
                                          SOVERSION ${QTGLIB_SOVERSION}
                                            VERSION ${QTGSTREAMER_VERSION})
target_link_libraries(QtGLib ${QT_QTCORE_LIBRARY} ${GOBJECT_LIBRARIES})
target_link_libraries(QtGLib LINK_INTERFACE_LIBRARIES ${QT_QTCORE_LIBRARY})

# Install
install(TARGETS QtGLib DESTINATION ${LIB_INSTALL_DIR} EXPORT ${EXPORT_TARGET_SET})
install_headers("QGlib" ${QtGLib_INSTALLED_HEADERS})
