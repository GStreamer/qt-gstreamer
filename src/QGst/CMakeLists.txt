set(QtGStreamer_SRCS
    init.cpp
    objectstore.cpp
    value.cpp
    structure.cpp
    caps.cpp
    miniobject.cpp
    object.cpp
    pad.cpp
    ghostpad.cpp
    element.cpp
    pluginfeature.cpp
    elementfactory.cpp
    bin.cpp
    childproxy.cpp
    pipeline.cpp
    message.cpp
    bus.cpp
    parse.cpp
    urihandler.cpp
    videoorientation.cpp
    xoverlay.cpp
    streamvolume.cpp
    colorbalance.cpp
    propertyprobe.cpp
    query.cpp
    clock.cpp
    buffer.cpp
    event.cpp
    clocktime.cpp
    taglist.cpp
    bufferlist.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/gen.cpp
)

set(QtGStreamerUi_SRCS
    Ui/videowidget.cpp
)

set(QtGStreamerUtils_SRCS
    Utils/applicationsink.cpp
    Utils/applicationsource.cpp
)

set(QtGStreamer_INSTALLED_HEADERS
    global.h            Global
    init.h              Init
    enums.h
    structs.h           Fourcc
                        Fraction
                        IntRange
                        Int64Range
                        DoubleRange
                        FractionRange
    structure.h         Structure
    caps.h              Caps
    miniobject.h        MiniObject
    object.h            Object
    pad.h               Pad
    ghostpad.h          GhostPad
    element.h           Element
    pluginfeature.h     PluginFeature
    elementfactory.h    ElementFactory
    bin.h               Bin
    childproxy.h        ChildProxy
    pipeline.h          Pipeline
    message.h           Message
    bus.h               Bus
    parse.h             Parse
    urihandler.h        UriHandler
    videoorientation.h  VideoOrientation
    xoverlay.h          XOverlay
    streamvolume.h      StreamVolume
    colorbalance.h      ColorBalance
    propertyprobe.h     PropertyProbe
    query.h             Query
    clock.h             Clock
    buffer.h            Buffer
    event.h             Event
    clocktime.h         ClockTime
    taglist.h           TagList
    bufferlist.h        BufferList

    Ui/global.h
    Ui/videowidget.h    Ui/VideoWidget

    Utils/applicationsink.h     Utils/ApplicationSink
    Utils/applicationsource.h   Utils/ApplicationSource
)

file(GLOB QtGStreamer_CODEGEN_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
list(REMOVE_ITEM QtGStreamer_CODEGEN_HEADERS "global.h")

set(QtGStreamer_CODEGEN_INCLUDES
    -Igst/gst.h
    -Igst/interfaces/interfaces-enumtypes.h
    -Igst/interfaces/xoverlay.h
    -Igst/interfaces/streamvolume.h
    -Igst/interfaces/colorbalance.h
    -Igst/interfaces/videoorientation.h
    -Igst/interfaces/propertyprobe.h
    -Igst/app/gstappsrc.h
    -IQGlib/Quark
)


# Setup the environment
set(QTGSTREAMER_API_VERSION 0.10)
set(QTGSTREAMER_SOVERSION 0)
set(QTGSTREAMER_UI_SOVERSION 0)
set(QTGSTREAMER_UTILS_SOVERSION 0)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${GSTREAMER_INCLUDE_DIR}
                    ${GSTREAMER_INTERFACES_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR})
add_definitions(-DGST_DISABLE_XML -DGST_DISABLE_LOADSAVE)

# Add command to generate gen.cpp using codegen
run_codegen("QGst" "${QtGStreamer_CODEGEN_INCLUDES}" "${QtGStreamer_CODEGEN_HEADERS}")

# Build and link QtGStreamer
automoc4_add_library(QtGStreamer ${SHARED_OR_STATIC} ${QtGStreamer_SRCS})
set_target_properties(QtGStreamer PROPERTIES OUTPUT_NAME QtGStreamer-${QTGSTREAMER_API_VERSION}
                                               SOVERSION ${QTGSTREAMER_SOVERSION}
                                                 VERSION ${QTGSTREAMER_VERSION})
target_link_libraries(QtGStreamer ${QTGLIB_LIBRARY} ${GOBJECT_LIBRARIES}
                                  ${GSTREAMER_LIBRARY} ${GSTREAMER_INTERFACES_LIBRARY})
target_link_libraries(QtGStreamer LINK_INTERFACE_LIBRARIES ${QTGLIB_LIBRARY})

# Build and link QtGStreamerUi
automoc4_add_library(QtGStreamerUi ${SHARED_OR_STATIC} ${QtGStreamerUi_SRCS})
set_target_properties(QtGStreamerUi PROPERTIES OUTPUT_NAME QtGStreamerUi-${QTGSTREAMER_API_VERSION}
                                                 SOVERSION ${QTGSTREAMER_UI_SOVERSION}
                                                   VERSION ${QTGSTREAMER_VERSION})
target_link_libraries(QtGStreamerUi ${QT_QTGUI_LIBRARY} ${QTGSTREAMER_LIBRARY})

# Build and link QtGStreamerUtils
automoc4_add_library(QtGStreamerUtils ${SHARED_OR_STATIC} ${QtGStreamerUtils_SRCS})
set_target_properties(QtGStreamerUtils PROPERTIES OUTPUT_NAME QtGStreamerUtils-${QTGSTREAMER_API_VERSION}
                                                    SOVERSION ${QTGSTREAMER_UTILS_SOVERSION}
                                                      VERSION ${QTGSTREAMER_VERSION})
target_link_libraries(QtGStreamerUtils ${QTGSTREAMER_LIBRARY} ${GSTREAMER_LIBRARY} ${GSTREAMER_APP_LIBRARY})
target_link_libraries(QtGStreamerUtils LINK_INTERFACE_LIBRARIES ${QTGSTREAMER_LIBRARY})

# Install
install(TARGETS QtGStreamer DESTINATION ${LIB_INSTALL_DIR} EXPORT ${EXPORT_TARGET_SET})
install(TARGETS QtGStreamerUi DESTINATION ${LIB_INSTALL_DIR} EXPORT ${EXPORT_TARGET_SET})
install(TARGETS QtGStreamerUtils DESTINATION ${LIB_INSTALL_DIR} EXPORT ${EXPORT_TARGET_SET})
install_headers("QGst" ${QtGStreamer_INSTALLED_HEADERS})
