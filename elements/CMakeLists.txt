if (GSTREAMER_VIDEO_LIBRARY_FOUND)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR} ${GSTREAMER_INCLUDE_DIR}
                    ${GLIB2_INCLUDE_DIR} ${GSTREAMER_VIDEO_INCLUDE_DIR})
add_definitions(-DGST_DISABLE_DEPRECATED)

automoc4_add_library(gstqwidgetvideosink MODULE gstqwidgetvideosink.cpp)
target_link_libraries(gstqwidgetvideosink ${QT_QTGUI_LIBRARY} ${GSTREAMER_LIBRARY}
                                          ${GOBJECT_LIBRARIES} ${GSTREAMER_VIDEO_LIBRARY})
install(TARGETS gstqwidgetvideosink DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/gstreamer-0.10)

endif()
