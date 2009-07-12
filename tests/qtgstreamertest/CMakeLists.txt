include_directories(${GSTREAMER_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR}
                    ${LIBXML2_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

automoc4_add_executable(qtgstreamertest qtgstreamertest.cpp)
target_link_libraries(qtgstreamertest ${QT_QTTEST_LIBRARY} ${GSTREAMER_LIBRARIES}
                                      ${GOBJECT_LIBRARIES} QtGstreamer)
