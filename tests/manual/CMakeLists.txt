include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

qt4_wrap_ui(QWIDGETVIDEOSINKTEST_UI qwidgetvideosinktest.ui)
automoc4_add_executable(qwidgetvideosinktest qwidgetvideosinktest.cpp ${QWIDGETVIDEOSINKTEST_UI})
target_link_libraries(qwidgetvideosinktest ${QT_QTGUI_LIBRARIES} ${QTGSTREAMER_LIBRARIES})


qt4_wrap_ui(VIDEOWIDGETTEST_UI videowidgettest.ui)
automoc4_add_executable(videowidgettest videowidgettest.cpp ${VIDEOWIDGETTEST_UI})
target_link_libraries(videowidgettest ${QTGSTREAMER_UI_LIBRARIES})

qt4_wrap_ui(VIDEOWIDGETPIPELINETEST_UI videowidgetpipelinetest.ui)
automoc4_add_executable(videowidgetpipelinetest videowidgetpipelinetest.cpp ${VIDEOWIDGETPIPELINETEST_UI})
target_link_libraries(videowidgetpipelinetest ${QTGSTREAMER_UI_LIBRARIES})

qt4_wrap_ui(VIDEOORIENTATIONTEST_UI videoorientationtest.ui)
automoc4_add_executable(videoorientationtest videoorientationtest.cpp ${VIDEOORIENTATIONTEST_UI})
target_link_libraries(videoorientationtest ${QTGSTREAMER_UI_LIBRARIES})
