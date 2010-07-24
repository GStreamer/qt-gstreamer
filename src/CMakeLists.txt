set(QtGstreamer_SRCS
    QGlib/quark.cpp
    QGlib/type.cpp
    QGlib/paramspec.cpp
    QGlib/object.cpp
    QGlib/value.cpp
    QGlib/signal.cpp
    QGlib/error.cpp

    QGst/global.cpp
    QGst/structs.cpp
    QGst/structure.cpp
    QGst/caps.cpp
    QGst/miniobject.cpp
    QGst/object.cpp
    QGst/pad.cpp
    QGst/ghostpad.cpp
    QGst/element.cpp
    QGst/pluginfeature.cpp
    QGst/elementfactory.cpp
    QGst/bin.cpp
    QGst/childproxy.cpp
    QGst/pipeline.cpp
    QGst/message.cpp
    QGst/bus.cpp
    QGst/parse.cpp
    QGst/urihandler.cpp
    QGst/xoverlay.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/gen.cpp
)

set(QtGstreamerUi_SRCS
    QGst/Ui/videowidget.cpp
)

set(INSTALLED_HEADERS
    QGlib/global.h
    QGlib/quark.h       QGlib/Quark
    QGlib/type.h        QGlib/Type
    QGlib/refpointer.h  QGlib/RefPointer
    QGlib/paramspec.h   QGlib/ParamSpec
    QGlib/object.h      QGlib/Object
    QGlib/value.h       QGlib/Value
    QGlib/signal.h      QGlib/Signal
    QGlib/emitimpl_p.h
    QGlib/connectimpl_p.h
    QGlib/error.h       QGlib/Error

    QGst/global.h       QGst/Global
    QGst/enums.h
    QGst/structs.h      QGst/Fourcc
                        QGst/Fraction
                        QGst/IntRange
                        QGst/DoubleRange
                        QGst/FractionRange
    QGst/structure.h    QGst/Structure
    QGst/caps.h         QGst/Caps
    QGst/miniobject.h   QGst/MiniObject
    QGst/object.h       QGst/Object
    QGst/pad.h          QGst/Pad
    QGst/ghostpad.h     QGst/GhostPad
    QGst/element.h      QGst/Element
    QGst/pluginfeature.h QGst/PluginFeature
    QGst/elementfactory.h QGst/ElementFactory
    QGst/bin.h          QGst/Bin
    QGst/childproxy.h   QGst/ChildProxy
    QGst/pipeline.h     QGst/Pipeline
    QGst/message.h      QGst/Message
    QGst/bus.h          QGst/Bus
    QGst/parse.h        QGst/Parse
    QGst/urihandler.h   QGst/UriHandler
    QGst/xoverlay.h     QGst/XOverlay

    QGst/Ui/videowidget.h QGst/Ui/VideoWidget
)

set(CODEGEN_HEADERS
    QGlib/type.h
    QGlib/paramspec.h
    QGlib/object.h
    QGlib/signal.h

    QGst/enums.h
    QGst/structs.h
    QGst/structure.h
    QGst/caps.h
    QGst/miniobject.h
    QGst/object.h
    QGst/pad.h
    QGst/ghostpad.h
    QGst/element.h
    QGst/pluginfeature.h
    QGst/elementfactory.h
    QGst/bin.h
    QGst/childproxy.h
    QGst/pipeline.h
    QGst/message.h
    QGst/bus.h
    QGst/urihandler.h
    QGst/xoverlay.h
)

set(CODEGEN_INCLUDES
    -Igst/gst.h
    -Igst/interfaces/xoverlay.h
)

# Setup the environment
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${GSTREAMER_INCLUDE_DIR}
                    ${GLIB2_INCLUDE_DIR} ${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS} -DGST_DISABLE_DEPRECATED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")


# Add command to generate gen.cpp using codegen
add_custom_command(OUTPUT gen.cpp
                   COMMAND codegen
                   ARGS ${CODEGEN_INCLUDES} ${CODEGEN_HEADERS} > ${CMAKE_CURRENT_BINARY_DIR}/gen.cpp
                   DEPENDS codegen ${CODEGEN_HEADERS}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


# Build and link QtGstreamer
if(STATIC_QTGSTREAMER)
    automoc4_add_library(QtGstreamer STATIC ${QtGstreamer_SRCS})
else()
    automoc4_add_library(QtGstreamer SHARED ${QtGstreamer_SRCS})
endif()

set_target_properties(QtGstreamer PROPERTIES SOVERSION ${QTGSTREAMER_SOVERSION}
                                             VERSION ${QTGSTREAMER_VERSION})
target_link_libraries(QtGstreamer ${QT_QTCORE_LIBRARY} ${GSTREAMER_LIBRARIES}
                                  ${GSTREAMER_INTERFACE_LIBRARY} ${GOBJECT_LIBRARIES})
target_link_libraries(QtGstreamer LINK_INTERFACE_LIBRARIES ${QT_QTCORE_LIBRARY})


# Build and link QtGstreamerUi
if(STATIC_QTGSTREAMER)
    automoc4_add_library(QtGstreamerUi STATIC ${QtGstreamerUi_SRCS})
else()
    automoc4_add_library(QtGstreamerUi SHARED ${QtGstreamerUi_SRCS})
endif()

set_target_properties(QtGstreamerUi PROPERTIES SOVERSION ${QTGSTREAMER_SOVERSION}
                                               VERSION ${QTGSTREAMER_VERSION})
target_link_libraries(QtGstreamerUi ${QT_QTGUI_LIBRARY} QtGstreamer)
target_link_libraries(QtGstreamerUi LINK_INTERFACE_LIBRARIES ${QT_QTGUI_LIBRARY} QtGstreamer)


# Install the library
install(TARGETS QtGstreamer DESTINATION ${CMAKE_INSTALL_PREFIX}/lib EXPORT QtGstreamerTargets)
install(TARGETS QtGstreamerUi DESTINATION ${CMAKE_INSTALL_PREFIX}/lib EXPORT QtGstreamerTargets)

# Install the cmake scripts that are used to find the installed library from external projects
install(EXPORT QtGstreamerTargets DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/QtGstreamer)
install(FILES ${CMAKE_SOURCE_DIR}/cmake/modules/FindQtGstreamer.cmake
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/QtGstreamer
        RENAME QtGstreamerConfig.cmake)

# Install the headers
foreach(header ${INSTALLED_HEADERS})
    get_filename_component(header_path ${header} PATH)
    install(FILES ${header} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/QtGstreamer/${header_path})
endforeach(header ${INSTALLED_HEADERS})
