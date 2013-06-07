include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

add_definitions(
    -DQTVIDEOSINK_NAME="${QTVIDEOSINK_NAME}"
    -DQTGLVIDEOSINK_NAME="${QTGLVIDEOSINK_NAME}"
    -DQWIDGETVIDEOSINK_NAME="${QWIDGETVIDEOSINK_NAME}"
)

qt4or5_wrap_ui(QWIDGETVIDEOSINKTEST_UI qwidgetvideosinktest.ui)
add_executable(qwidgetvideosinktest qwidgetvideosinktest.cpp ${QWIDGETVIDEOSINKTEST_UI})
target_link_libraries(qwidgetvideosinktest ${QTGSTREAMER_LIBRARIES})
qt4or5_use_modules(qwidgetvideosinktest Core Gui Widgets)

qt4or5_wrap_ui(VIDEOWIDGETTEST_UI videowidgettest.ui)
add_executable(videowidgettest videowidgettest.cpp ${VIDEOWIDGETTEST_UI})
target_link_libraries(videowidgettest ${QTGSTREAMER_UI_LIBRARIES})
qt4or5_use_modules(videowidgettest Core Gui Widgets)

qt4or5_wrap_ui(VIDEOWIDGETPIPELINETEST_UI videowidgetpipelinetest.ui)
add_executable(videowidgetpipelinetest videowidgetpipelinetest.cpp ${VIDEOWIDGETPIPELINETEST_UI})
target_link_libraries(videowidgetpipelinetest ${QTGSTREAMER_UI_LIBRARIES})
qt4or5_use_modules(videowidgetpipelinetest Core Gui Widgets)

qt4or5_wrap_ui(VIDEOORIENTATIONTEST_UI videoorientationtest.ui)
add_executable(videoorientationtest videoorientationtest.cpp ${VIDEOORIENTATIONTEST_UI})
target_link_libraries(videoorientationtest ${QTGSTREAMER_UI_LIBRARIES})
qt4or5_use_modules(videoorientationtest Core Gui Widgets)
