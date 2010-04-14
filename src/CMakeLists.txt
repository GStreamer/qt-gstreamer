include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(QtGstreamer_SRCS
    QGlib/quark.cpp
    QGlib/type.cpp
    QGlib/paramspec.cpp
    QGlib/object.cpp
    QGlib/value.cpp
    QGlib/closure.cpp

    QGst/structure.cpp
    QGst/caps.cpp
    QGst/miniobject.cpp
    QGst/object.cpp
    QGst/pad.cpp
    QGst/element.cpp
    QGst/pluginfeature.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/gen.cpp
)

set(INSTALLED_HEADERS
    QGlib/global.h
    QGlib/gdeclarations.h
    QGlib/quark.h       QGlib/Quark
    QGlib/type.h        QGlib/Type
    QGlib/refpointer.h  QGlib/RefPointer
    QGlib/paramspec.h   QGlib/ParamSpec
    QGlib/object.h      QGlib/Object
    QGlib/value.h       QGlib/Value
    QGlib/closure.h     QGlib/Closure

    QGlib/valueimpl/assert.h
    QGlib/valueimpl/valueimpl.h
    QGlib/valueimpl/nativetypes.h
    QGlib/valueimpl/paramspecptr.h
    QGlib/valueimpl/objectptr.h

    QGst/global.h
    QGst/enums.h
    QGst/structure.h    QGst/Structure
    QGst/caps.h         QGst/Caps
    QGst/miniobject.h   QGst/MiniObject
    QGst/object.h       QGst/Object
    QGst/pad.h          QGst/Pad
    QGst/element.h      QGst/Element
    QGst/pluginfeature.h QGst/PluginFeature
)

set(CODEGEN_HEADERS
    QGlib/type.h
    QGlib/paramspec.h
    QGlib/object.h
    QGlib/closure.h

    QGst/enums.h
    QGst/structure.h
    QGst/caps.h
    QGst/miniobject.h
    QGst/object.h
    QGst/pad.h
    QGst/element.h
    QGst/pluginfeature.h
)

add_custom_command(OUTPUT gen.cpp
                   COMMAND codegen
                   ARGS ${CODEGEN_HEADERS} > ${CMAKE_CURRENT_BINARY_DIR}/gen.cpp
                   DEPENDS codegen ${CODEGEN_HEADERS}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

automoc4_add_library(QtGstreamer SHARED ${QtGstreamer_SRCS})
set_target_properties(QtGstreamer PROPERTIES SOVERSION 0 VERSION 0.0.2)
target_link_libraries(QtGstreamer ${QT_QTCORE_LIBRARY} ${GSTREAMER_LIBRARIES}
                                  ${GSTREAMER_INTERFACE_LIBRARY} ${GOBJECT_LIBRARIES})
target_link_libraries(QtGstreamer LINK_INTERFACE_LIBRARIES ${QT_QTCORE_LIBRARY})


install(TARGETS QtGstreamer DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

foreach(header ${INSTALLED_HEADERS})
    get_filename_component(header_path ${header} PATH)
    install(FILES ${header} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/QtGstreamer/${header_path})
endforeach(header ${INSTALLED_HEADERS})
