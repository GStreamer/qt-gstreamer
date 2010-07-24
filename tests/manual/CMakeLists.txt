include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

qt4_wrap_ui(QWIDGETVIDEOSINKTEST_UI qwidgetvideosinktest.ui)
automoc4_add_executable(qwidgetvideosinktest qwidgetvideosinktest.cpp ${QWIDGETVIDEOSINKTEST_UI})
target_link_libraries(qwidgetvideosinktest ${QT_QTGUI_LIBRARIES} ${QTGSTREAMER_LIBRARIES})
