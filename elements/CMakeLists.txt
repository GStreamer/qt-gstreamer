set(GST_PACKAGE_NAME "QtGStreamer" CACHE STRING "The name of this package")
set(GST_PACKAGE_ORIGIN "http://gstreamer.freedesktop.org/" CACHE STRING "The origin url of this package")

add_definitions(-DPACKAGE="qt-gstreamer"
                -DPACKAGE_NAME="${GST_PACKAGE_NAME}"
                -DPACKAGE_ORIGIN="${GST_PACKAGE_ORIGIN}"
                -DPACKAGE_VERSION="${QTGSTREAMER_VERSION}")

if (GSTREAMER_BASE_LIBRARY_FOUND AND GSTREAMER_VIDEO_LIBRARY_FOUND)
    include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} ${GSTREAMER_INCLUDE_DIR}
                        ${GLIB2_INCLUDE_DIR} ${GSTREAMER_VIDEO_INCLUDE_DIR})
    add_definitions(-DGST_DISABLE_XML -DGST_DISABLE_LOADSAVE)

    automoc4_add_library(gstqwidgetvideosink MODULE gstqwidgetvideosink.cpp)
    target_link_libraries(gstqwidgetvideosink ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
                                              ${GOBJECT_LIBRARIES} ${GSTREAMER_LIBRARY}
                                              ${GSTREAMER_BASE_LIBRARY} ${GSTREAMER_VIDEO_LIBRARY})
    install(TARGETS gstqwidgetvideosink DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/gstreamer-0.10)
endif()
