include_directories(${CMAKE_CURRENT_BINARY_DIR})

glib2_genmarshal(gstqtvideosinkmarshal
    VOID:POINTER,FLOAT,FLOAT,FLOAT,FLOAT
    VOID:POINTER,DOUBLE,DOUBLE,DOUBLE,DOUBLE
)

set(GstQtVideoSink_SRCS
    utils.cpp
    bufferformat.cpp
    genericsurfacepainter.cpp

    qtvideosinkdelegate.cpp
    qwidgetvideosinkdelegate.cpp

    gstqtvideosinkplugin.cpp
    gstqtvideosinkbase.cpp
    gstqtvideosink.cpp
    gstqwidgetvideosink.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/gstqtvideosinkmarshal.c
)

if (QT_QTOPENGL_FOUND AND (OPENGL_FOUND OR OPENGLES2_FOUND))
    set(GstQtVideoSink_GL_SRCS
        openglsurfacepainter.cpp
        gstqtglvideosinkbase.cpp
        gstqtglvideosink.cpp
    )
    set(GstQtVideoSink_test_GL_SRCS
        openglsurfacepainter.cpp
    )

    if (OPENGLES2_FOUND)
        set(GstQtVideoSink_GL_LIBS ${OPENGLES2_LIBRARY} ${QT_QTOPENGL_LIBRARY})
        include_directories(${OPENGLES2_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR})
    else()
        set(GstQtVideoSink_GL_LIBS ${OPENGL_gl_LIBRARY} ${QT_QTOPENGL_LIBRARY})
        include_directories(${OPENGL_INCLUDE_DIR} ${QT_QTOPENGL_INCLUDE_DIR})
    endif()
else()
    add_definitions(-DGST_QT_VIDEO_SINK_NO_OPENGL)
endif()

automoc4_add_library(gstqtvideosink MODULE ${GstQtVideoSink_SRCS} ${GstQtVideoSink_GL_SRCS})
target_link_libraries(gstqtvideosink
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${GOBJECT_LIBRARIES}
    ${GSTREAMER_LIBRARY}
    ${GSTREAMER_BASE_LIBRARY}
    ${GSTREAMER_VIDEO_LIBRARY}
    ${GSTREAMER_INTERFACES_LIBRARY}
    ${GstQtVideoSink_GL_LIBS}
)
install(TARGETS gstqtvideosink DESTINATION ${PLUGINS_INSTALL_DIR})


if (QTGSTREAMER_TESTS)
    automoc4_add_executable(qtvideosink_autotest
        autotest.cpp
        utils.cpp
        bufferformat.cpp
        genericsurfacepainter.cpp
        ${GstQtVideoSink_test_GL_SRCS}
    )
    target_link_libraries(qtvideosink_autotest
        ${QT_QTTEST_LIBRARY}
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${GOBJECT_LIBRARIES}
        ${GSTREAMER_LIBRARY}
        ${GSTREAMER_BASE_LIBRARY}
        ${GSTREAMER_VIDEO_LIBRARY}
        ${GSTREAMER_INTERFACES_LIBRARY}
        ${GstQtVideoSink_GL_LIBS}
    )
    #add_test(NAME qtvideosink_autotest COMMAND qtvideosink_autotest)
endif()
