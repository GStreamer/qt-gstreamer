include_directories(${GSTREAMER_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR}
                    ${LIBXML2_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(QtGstreamer_SRCS
    qgstglobal.cpp
    qgstobject.cpp
    qgstelement.cpp
    qgstbin.cpp
    qgstpipeline.cpp
    qgstelementfactory.cpp
    qgstpad.cpp
    qgstghostpad.cpp
    qgstbus.cpp
    qgvalue.cpp
    qgstcaps.cpp
    qgststructure.cpp
)

set(QtGstreamer_HEADERS
    qgstglobal.h
    qgstobject.h
    qgstelement.h
    qgstbin.h
    qgstpipeline.h
    qgstelementfactory.h
    qgstpad.h
    qgstghostpad.h
    qgstbus.h
    qgvalue.h
    qgstcaps.h
    qgststructure.h
    qgstdeclarations.h
) 

automoc4_add_library(QtGstreamer SHARED ${QtGstreamer_SRCS})
set_target_properties(QtGstreamer PROPERTIES SOVERSION 0 VERSION 0.0.1)
target_link_libraries(QtGstreamer ${QT_QTCORE_LIBRARY} ${GSTREAMER_LIBRARIES} ${GOBJECT_LIBRARIES})

install(TARGETS QtGstreamer DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${QtGstreamer_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/QtGstreamer)
