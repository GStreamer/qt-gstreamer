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
    discoverer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/gen.cpp
)

set(QtGStreamerUi_SRCS
    Ui/videowidget.cpp
    Ui/graphicsvideosurface.cpp
    Ui/graphicsvideowidget.cpp
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
    discoverer.h        Discoverer

    Ui/global.h
    Ui/videowidget.h            Ui/VideoWidget
    Ui/graphicsvideosurface.h   Ui/GraphicsVideoSurface
    Ui/graphicsvideowidget.h    Ui/GraphicsVideoWidget

    Utils/global.h
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
    -Igst/pbutils/gstdiscoverer.h
    -Igst/pbutils/pbutils-enumtypes.h
    -IQGlib/Quark
)


# Setup the environment
set(QTGSTREAMER_API_VERSION 0.10)
set(QTGSTREAMER_SOVERSION 0)
set(QTGSTREAMER_UI_SOVERSION 0)
set(QTGSTREAMER_UTILS_SOVERSION 0)
include_directories(
    ${GSTREAMER_INCLUDE_DIR}
    ${GSTREAMER_INTERFACES_INCLUDE_DIR}
    ${GSTREAMER_BASE_INCLUDE_DIR}
    ${GSTREAMER_APP_INCLUDE_DIR}
    ${GSTREAMER_PBUTILS_INCLUDE_DIR}
    ${GLIB2_INCLUDE_DIR}
)
add_definitions(-DGST_DISABLE_XML -DGST_DISABLE_LOADSAVE)

if (Qt4or5_OpenGL_FOUND AND (OPENGL_FOUND OR OPENGLES2_FOUND))
    if (OPENGLES2_FOUND)
        include_directories(${OPENGLES2_INCLUDE_DIR})
    else()
        include_directories(${OPENGL_INCLUDE_DIR})
    endif()
else()
    add_definitions(-DQTGSTREAMER_UI_NO_OPENGL)
endif()

# Add command to generate gen.cpp using codegen
run_codegen("QGst" "${QtGStreamer_CODEGEN_INCLUDES}" "${QtGStreamer_CODEGEN_HEADERS}")

# Build and link QtGStreamer
add_library(${QTGSTREAMER_LIBRARY} ${SHARED_OR_STATIC} ${QtGStreamer_SRCS})
set_target_properties(${QTGSTREAMER_LIBRARY} PROPERTIES OUTPUT_NAME ${QTGSTREAMER_LIBRARY}-${QTGSTREAMER_API_VERSION}
                                               SOVERSION ${QTGSTREAMER_SOVERSION}
                                                 VERSION ${QTGSTREAMER_VERSION})
target_link_libraries(${QTGSTREAMER_LIBRARY} LINK_PUBLIC ${QTGLIB_LIBRARY})
target_link_libraries(${QTGSTREAMER_LIBRARY} LINK_PRIVATE ${GOBJECT_LIBRARIES}
                                  ${GSTREAMER_LIBRARY} ${GSTREAMER_INTERFACES_LIBRARY}
                                  ${GSTREAMER_PBUTILS_LIBRARY})
qt4or5_use_modules(${QTGSTREAMER_LIBRARY} LINK_PUBLIC Core)

# Build and link QtGStreamerUi
add_library(${QTGSTREAMER_UI_LIBRARY} ${SHARED_OR_STATIC} ${QtGStreamerUi_SRCS})
set_target_properties(${QTGSTREAMER_UI_LIBRARY} PROPERTIES OUTPUT_NAME ${QTGSTREAMER_UI_LIBRARY}-${QTGSTREAMER_API_VERSION}
                                                 SOVERSION ${QTGSTREAMER_UI_SOVERSION}
                                                   VERSION ${QTGSTREAMER_VERSION})
target_link_libraries(${QTGSTREAMER_UI_LIBRARY} LINK_PUBLIC ${QTGSTREAMER_LIBRARY})
qt4or5_use_modules(${QTGSTREAMER_UI_LIBRARY} LINK_PUBLIC Widgets Gui)
qt4or5_use_modules(${QTGSTREAMER_UI_LIBRARY} LINK_PRIVATE Core)
if (Qt4or5_OpenGL_FOUND AND (OPENGL_FOUND OR OPENGLES2_FOUND))
    qt4or5_use_modules(${QTGSTREAMER_UI_LIBRARY} LINK_PRIVATE OpenGL)
endif()

# Build and link QtGStreamerUtils
add_library(${QTGSTREAMER_UTILS_LIBRARY} ${SHARED_OR_STATIC} ${QtGStreamerUtils_SRCS})
set_target_properties(${QTGSTREAMER_UTILS_LIBRARY} PROPERTIES OUTPUT_NAME ${QTGSTREAMER_UTILS_LIBRARY}-${QTGSTREAMER_API_VERSION}
                                                    SOVERSION ${QTGSTREAMER_UTILS_SOVERSION}
                                                      VERSION ${QTGSTREAMER_VERSION})
target_link_libraries(${QTGSTREAMER_UTILS_LIBRARY} LINK_PUBLIC ${QTGSTREAMER_LIBRARY})
target_link_libraries(${QTGSTREAMER_UTILS_LIBRARY} LINK_PRIVATE ${GSTREAMER_LIBRARY} ${GSTREAMER_APP_LIBRARY})
qt4or5_use_modules(${QTGSTREAMER_UTILS_LIBRARY} LINK_PRIVATE Core)

# Install
install(TARGETS ${QTGSTREAMER_LIBRARY} EXPORT ${EXPORT_TARGET_SET} ${QTGSTREAMER_INSTALL_TARGET_DEFAULT_ARGS})
install(TARGETS ${QTGSTREAMER_UI_LIBRARY} EXPORT ${EXPORT_TARGET_SET} ${QTGSTREAMER_INSTALL_TARGET_DEFAULT_ARGS})
install(TARGETS ${QTGSTREAMER_UTILS_LIBRARY} EXPORT ${EXPORT_TARGET_SET} ${QTGSTREAMER_INSTALL_TARGET_DEFAULT_ARGS})
install_headers("QGst" ${QtGStreamer_INSTALLED_HEADERS})
