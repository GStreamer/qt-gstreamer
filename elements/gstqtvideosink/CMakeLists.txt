include_directories(${CMAKE_CURRENT_BINARY_DIR})

glib2_genmarshal(gstqtvideosinkmarshal
    VOID:POINTER,FLOAT,FLOAT,FLOAT,FLOAT
    VOID:POINTER,DOUBLE,DOUBLE,DOUBLE,DOUBLE
)

set(GstQtVideoSink_SRCS
    bufferformat.cpp
    genericsurfacepainter.cpp
    gstqtvideosink.cpp
    gstqtvideosinkplugin.cpp
    gstqtvideosinksurface.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/gstqtvideosinkmarshal.c
)

if (QT_QTOPENGL_FOUND AND OPENGL_FOUND)
    set(GstQtVideoSink_SRCS ${GstQtVideoSink_SRCS} openglsurfacepainter.cpp)
    set(GstQtVideoSink_GL_LIBS ${OPENGL_gl_LIBRARY} ${QT_QTOPENGL_LIBRARY})
    include_directories(${OPENGL_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR})
else()
    add_definitions(-DGST_QT_VIDEO_SINK_NO_OPENGL)
endif()

automoc4_add_library(gstqtvideosink MODULE ${GstQtVideoSink_SRCS})
target_link_libraries(gstqtvideosink
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${GOBJECT_LIBRARIES}
    ${GSTREAMER_LIBRARY}
    ${GSTREAMER_BASE_LIBRARY}
    ${GSTREAMER_VIDEO_LIBRARY}
    ${GstQtVideoSink_GL_LIBS}
)
install(TARGETS gstqtvideosink DESTINATION ${PLUGINS_INSTALL_DIR})
