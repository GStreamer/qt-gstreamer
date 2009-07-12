include_directories(${GSTREAMER_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR}
                    ${LIBXML2_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(QtGstreamer_SRCS
    qgstobject.cpp
    qgstelement.cpp
    qgstbin.cpp
    qgstpipeline.cpp
    qgstelementfactory.cpp
)

automoc4_add_library(QtGstreamer SHARED ${QtGstreamer_SRCS})
set_target_properties(QtGstreamer PROPERTIES SOVERSION 0 VERSION 0.0.1)
target_link_libraries(QtGstreamer ${QT_QTCORE_LIBRARY} ${GSTREAMER_LIBRARIES} ${GOBJECT_LIBRARIES})
