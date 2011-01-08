set(QtGStreamer_SRCS
    global.cpp
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
    ${CMAKE_CURRENT_BINARY_DIR}/gen.cpp
)

set(QtGStreamerUi_SRCS
    Ui/videowidget.cpp
)

set(QtGStreamer_INSTALLED_HEADERS
    global.h            Global
    enums.h
    structs.h           Fourcc
                        Fraction
                        IntRange
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

    Ui/videowidget.h    Ui/VideoWidget
)

set(QtGStreamer_CODEGEN_HEADERS
    enums.h
    structs.h
    structure.h
    caps.h
    miniobject.h
    object.h
    pad.h
    ghostpad.h
    element.h
    pluginfeature.h
    elementfactory.h
    bin.h
    childproxy.h
    pipeline.h
    message.h
    bus.h
    urihandler.h
    videoorientation.h
    xoverlay.h
    streamvolume.h
    colorbalance.h
    propertyprobe.h
    query.h
    clock.h
    buffer.h
    event.h
)

set(QtGStreamer_CODEGEN_INCLUDES
    -Igst/gst.h
    -Igst/interfaces/interfaces-enumtypes.h
    -Igst/interfaces/xoverlay.h
    -Igst/interfaces/streamvolume.h
    -Igst/interfaces/colorbalance.h
    -Igst/interfaces/videoorientation.h
    -Igst/interfaces/propertyprobe.h
    -IQGlib/Quark
)


# Setup the environment
set(QTGSTREAMER_API_VERSION 0.10)
set(QTGSTREAMER_SOVERSION 0)
set(QTGSTREAMER_UI_SOVERSION 0)
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
target_link_libraries(QtGStreamer ${QT_QTCORE_LIBRARY} ${QTGLIB_LIBRARY} ${GOBJECT_LIBRARIES}
                                  ${GSTREAMER_LIBRARY} ${GSTREAMER_INTERFACES_LIBRARY})
target_link_libraries(QtGStreamer LINK_INTERFACE_LIBRARIES ${QT_QTCORE_LIBRARY} ${QTGLIB_LIBRARY})

# Build and link QtGStreamerUi
automoc4_add_library(QtGStreamerUi ${SHARED_OR_STATIC} ${QtGStreamerUi_SRCS})
set_target_properties(QtGStreamerUi PROPERTIES OUTPUT_NAME QtGStreamerUi-${QTGSTREAMER_API_VERSION}
                                                 SOVERSION ${QTGSTREAMER_UI_SOVERSION}
                                                   VERSION ${QTGSTREAMER_VERSION})
target_link_libraries(QtGStreamerUi ${QT_QTGUI_LIBRARY} ${QTGSTREAMER_LIBRARY})

# Install
install(TARGETS QtGStreamer DESTINATION ${LIB_INSTALL_DIR} EXPORT ${EXPORT_TARGET_SET})
install(TARGETS QtGStreamerUi DESTINATION ${LIB_INSTALL_DIR} EXPORT ${EXPORT_TARGET_SET})
install_headers("QGst" ${QtGStreamer_INSTALLED_HEADERS})
