include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(QtGstreamer_SRCS
    QGlib/quark.cpp
    QGlib/type.cpp
    QGlib/paramspec.cpp
    QGlib/object.cpp
    QGlib/value.cpp
    QGlib/closure.cpp

    QGst/object.cpp
)

set(QtGstreamer_HEADERS
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
    QGst/object.h       QGst/Object
)

automoc4_add_library(QtGstreamer SHARED ${QtGstreamer_SRCS})
set_target_properties(QtGstreamer PROPERTIES SOVERSION 0 VERSION 0.0.2)
target_link_libraries(QtGstreamer ${QT_QTCORE_LIBRARY} ${GSTREAMER_LIBRARIES}
                                  ${GSTREAMER_INTERFACE_LIBRARY} ${GOBJECT_LIBRARIES})
target_link_libraries(QtGstreamer LINK_INTERFACE_LIBRARIES ${QT_QTCORE_LIBRARY})


install(TARGETS QtGstreamer DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

foreach(header ${QtGstreamer_HEADERS})
    get_filename_component(header_path ${header} PATH)
    install(FILES ${header} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/QtGstreamer/${header_path})
endforeach(header ${QtGstreamer_HEADERS})
